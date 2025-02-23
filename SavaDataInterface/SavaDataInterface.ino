//Libraries 
#include <Wire.h>

#include <Digital_Light_TSL2561.h>
#include "SmartInterface.h"


// Use SAMD51's DMAC to read ADC1 on the Microphone input and alternately store results in two memory arrays
// Use DMAC channel0
#define NO_RESULTS 256
const int WINDOW_SIZE = 512;
#define LED_PIN D0                                                  // for debug
#define VolumeGainFactorBits 0
#define AmpMax 1600
volatile boolean results0Ready = false;
volatile boolean results1Ready = false;
uint16_t adcResults0[NO_RESULTS];                                  // ADC results array 0
uint16_t adcResults1[NO_RESULTS];                                  // ADC results array 1
uint32_t maxdB;
uint32_t dB;
typedef struct           // DMAC descriptor structure
{
  uint16_t btctrl;
  uint16_t btcnt;
  uint32_t srcaddr;
  uint32_t dstaddr;
  uint32_t descaddr;
} dmacdescriptor ;

volatile dmacdescriptor wrb[DMAC_CH_NUM] __attribute__ ((aligned (16)));          // Write-back DMAC descriptors
dmacdescriptor descriptor_section[DMAC_CH_NUM] __attribute__ ((aligned (16)));    // DMAC channel descriptors
dmacdescriptor descriptor __attribute__ ((aligned (16)));                         // Place holder descriptor


uint32_t updateTime = 0;       // time for next update

int old_analog =  -999; // Value last displayed
int old_digital = -999; // Value last displayed

int average_index = 0;
int readings[WINDOW_SIZE] = {0};

bool first =false;

float t=20;
float concentration=0;
float h=0;
int light = 0;
bool pressed = false;

ProgramState current_state = show_data;
ClassAssistant classAssistant;
bool class_started = false;
unsigned long started_notification = 0;
bool pause_resume_notification = false;

NotificationAlert pauseRecomendation(5, 5*60, "A break from class is recommended");
NotificationAlert blockRecomendation(5, 10*60, "a block breakthrough of the class is recommended");

NotificationAlert lowTemperaturetAlert(5, 15*60, "Low temperature detected");
float min_t = 15;
NotificationAlert highTemperaturetAlert(5, 15*60, "High temperature detected");
float max_t = 35;
NotificationAlert lightAlert(5, 10*60, "Insuficient light detected");
int min_light = 300;
NotificationAlert humiditytAlert(5, 10*60, "Insuficient humidity detected");
float min_h = 300;
NotificationAlert airtAlert(5, 10*60, "Bad air quality detected");
float min_concentration = 0;

unsigned long lastTrackerCall = 0;

#define N_NOTIFICATIONS 7
NotificationAlert* notifications[N_NOTIFICATIONS] = {
  &pauseRecomendation,
  &blockRecomendation,
  &lowTemperaturetAlert,
  &highTemperaturetAlert,
  &lightAlert,
  &humiditytAlert,
  &airtAlert
};

Tracker tracker;


void wifiSetup(){
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi.");
}

void setup()
{
  SERIAL.begin(9600); //Start SERIAL communication
  wifiSetup();
  pinMode(WIO_MIC, INPUT);
  analogReference(AR_DEFAULT);
  
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
  pinMode(WIO_BUZZER, OUTPUT);
  Wire.begin();
  TSL2561.init();
  scd30.initialize();
    // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_PIN, OUTPUT);

  DMAC->BASEADDR.reg = (uint32_t)descriptor_section;                          // Specify the location of the descriptors
  DMAC->WRBADDR.reg = (uint32_t)wrb;                                          // Specify the location of the write back descriptors
  DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);                // Enable the DMAC peripheral

  DMAC->Channel[1].CHCTRLA.reg = DMAC_CHCTRLA_TRIGSRC(ADC1_DMAC_ID_RESRDY) |  // Set DMAC to trigger when ADC1 result is ready
                                 DMAC_CHCTRLA_TRIGACT_BURST;                  // DMAC burst transfer
  descriptor.descaddr = (uint32_t)&descriptor_section[1];                     // Set up a circular descriptor
  descriptor.srcaddr = (uint32_t)&ADC1->RESULT.reg;                           // Take the result from the ADC1 RESULT register
  descriptor.dstaddr = (uint32_t)adcResults0 + sizeof(uint16_t) * NO_RESULTS; // Place it in the adcResults0 array
  descriptor.btcnt = NO_RESULTS;                                              // Beat count
  descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_HWORD |                            // Beat size is HWORD (16-bits)
                      DMAC_BTCTRL_DSTINC |                                    // Increment the destination address
                      DMAC_BTCTRL_VALID |                                     // Descriptor is valid
                      DMAC_BTCTRL_BLOCKACT_SUSPEND;                           // Suspend DMAC channel 0 after block transfer
  memcpy(&descriptor_section[0], &descriptor, sizeof(descriptor));            // Copy the descriptor to the descriptor section
  descriptor.descaddr = (uint32_t)&descriptor_section[0];                     // Set up a circular descriptor
  descriptor.srcaddr = (uint32_t)&ADC1->RESULT.reg;                           // Take the result from the ADC1 RESULT register
  descriptor.dstaddr = (uint32_t)adcResults1 + sizeof(uint16_t) * NO_RESULTS; // Place it in the adcResults1 array
  descriptor.btcnt = NO_RESULTS;                                              // Beat count
  descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_HWORD |                            // Beat size is HWORD (16-bits)
                      DMAC_BTCTRL_DSTINC |                                    // Increment the destination address
                      DMAC_BTCTRL_VALID |                                     // Descriptor is valid
                      DMAC_BTCTRL_BLOCKACT_SUSPEND;                           // Suspend DMAC channel 0 after block transfer
  memcpy(&descriptor_section[1], &descriptor, sizeof(descriptor));            // Copy the descriptor to the descriptor section

  NVIC_SetPriority(DMAC_1_IRQn, 0);    // Set the Nested Vector Interrupt Controller (NVIC) priority for TCC1 OVF to 0 (highest)
  NVIC_EnableIRQ(DMAC_1_IRQn);         // Connect TCC1 to Nested Vector Interrupt Controller (NVIC)

  DMAC->Channel[1].CHINTENSET.reg = DMAC_CHINTENSET_SUSP;                     // Activate the suspend (SUSP) interrupt on DMAC channel 0

  ADC1->INPUTCTRL.bit.MUXPOS = ADC_INPUTCTRL_MUXPOS_AIN12_Val;                // Set the analog input to AIN12
  while (ADC1->SYNCBUSY.bit.INPUTCTRL);                                       // Wait for synchronization
  ADC1->SAMPCTRL.bit.SAMPLEN = 0x0a;                                          // Set max Sampling Time Length to half divided ADC clock pulse (2.66us) if set to 0x0
  while (ADC1->SYNCBUSY.bit.SAMPCTRL);                                        // Wait for synchronization
  ADC1->CTRLA.reg = ADC_CTRLA_PRESCALER_DIV256;                               // Divide Clock ADC GCLK by 256 (48MHz/256 = 187.5kHz)
  ADC1->CTRLB.reg = ADC_CTRLB_RESSEL_12BIT |                                  // Set ADC resolution to 12 bits
                    ADC_CTRLB_FREERUN;                                        // Set ADC to free run mode
  while (ADC1->SYNCBUSY.bit.CTRLB);                                           // Wait for synchronization
  ADC1->CTRLA.bit.ENABLE = 1;                                                 // Enable the ADC
  while (ADC1->SYNCBUSY.bit.ENABLE);                                          // Wait for synchronization
  ADC1->SWTRIG.bit.START = 1;                                                 // Initiate a software trigger to start an ADC conversion
  while (ADC1->SYNCBUSY.bit.SWTRIG);                                          // Wait for synchronization
  DMAC->Channel[1].CHCTRLA.bit.ENABLE = 1;                                    // Enable DMAC ADC on channel 1

  SERIAL.print("Initializing SD card...");
  if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI)) {
    SERIAL.println("initialization failed!");
    while (1);
  }
  SERIAL.println("initialization done.");
  rtc.begin();
}

void loop() {

  // Read right button
  if (digitalRead(WIO_KEY_A) == LOW) {
  //  delay(500);
  //  SERIAL.println("button A pressed");
  //  SERIAL.println(pressed);
  //  if (current_state == noise)
  //    current_state = show_data;
  //  else
  //    current_state = noise;
  //  SERIAL.print("Current state noise ");
  //  SERIAL.println(current_state);
  //  if (pressed == true)
  //  {
  //    first=!first;
  //  }
//
  //  pressed = !pressed;
  //  SERIAL.println("first");
  //  SERIAL.println(first);
  //  
  }

  // Read middle button
  else if (digitalRead(WIO_KEY_B) == LOW)
  {
    delay(500);
    SERIAL.println(classAssistant.get_class_is_running());
    SERIAL.println("Button B pressed");
    // Show start - puase - stop menu
    if (!classAssistant.get_class_has_started())
    {
      current_state = start_class;
      SERIAL.print("Current state start class ");
      SERIAL.println(current_state);
    }
    else
    {
      delay(200);
      SERIAL.println("class ended");
      classAssistant.end_class();
      tracker.gather_data(h, light, concentration, t);
      tracker.write_data(true);
      SERIAL.println("Finish end class menu");
    }
  }

  else if (classAssistant.get_class_is_running() && digitalRead(WIO_KEY_C) == LOW)
  {
    delay(200);
    SERIAL.println("Button C is pressed");
    current_state = change_block;
    SERIAL.print("Current state change block ");
    SERIAL.println(current_state);
  }

  if (pause_resume_notification && millis() - started_notification > 5 * 1000)
  {
    pause_resume_notification = false;
    SERIAL.println("stop showing resume notification");
  }


  //Check normal class conditions
  // Check break recomendation
  // if (classAssistant.check_current_time_in_a_row())
  // {
  //   SERIAL.println("Break recommended");
  //   pauseRecomendation.activate();
  // }
  // Check block duration
  if (classAssistant.check_current_block_time() && notifications[1]->get_activation())
  {
    SERIAL.println("Change block recommended");
    notifications[1]->activate();
  }
  // Check temperature level
  if (t < min_t && !notifications[2]->get_activation())
  {
    SERIAL.println("Low temperature");
    notifications[2]->activate();
  }
  else if (t > max_t && !notifications[3]->get_activation())
  {
    SERIAL.println("High temperature");
    notifications[3]->activate();
  }
  // Check light level
  if (light < min_light && !notifications[4]->get_activation())
  {
    SERIAL.println("Low light");
    notifications[4]->activate();
  }
  // Check humidity level
  if (h > min_h && !notifications[5]->get_activation())
  {
    SERIAL.println("High humidity level");
    notifications[5]->activate();
  }
  // Check air quality level
  if (concentration > min_concentration && !notifications[6]->get_activation())
  {
    SERIAL.println("Bad air quality");
    notifications[6]->activate();
  }


  switch (current_state){
  // Read sensor measures and show in screen
  case show_data:
    SERIAL.println("SHOW DATA");
    show_measured_data();
    break;
  case noise:
  SERIAL.println("NOISE");
    if (first == false){
      SERIAL.println("first noise");
      SERIAL.println("------solo entra una vez");
      first=true;
    }
    SERIAL.println("Plot needle");
    updateTime = millis(); // Next update time
    break;
  case start_class:
    //start_class_menu();
    current_state = show_data;
    classAssistant.start_class();
    class_started = true;
    break;
  case change_block:
    if (change_block_menu(String(classAssistant.get_current_block())))
    {
      classAssistant.change_class_block();
    }
    current_state = show_data;
  default:
    break;
  }

}


void show_measured_data()
{
  float result[3] = {0};

  if(scd30.isAvailable())
  {
    scd30.getCarbonDioxideConcentration(result);
    // SERIAL.print("Carbon Dioxide Concentration is: ");
    concentration=result[0];
    // SERIAL.print(result[0]);
    // SERIAL.println(" ppm");
    // SERIAL.println(" ");
    // SERIAL.print("Temperature = ");
    t = result[1];
    // SERIAL.print(result[1]);
    // SERIAL.println(" ℃");
    // SERIAL.println(" ");
    // SERIAL.print("Humidity = ");
    h = result[2];
    // SERIAL.print(result[2]);
    // SERIAL.println(" %");
    // SERIAL.println(" ");
    // SERIAL.println(" ");
    // SERIAL.println(" ");
  }

  

  //int t = dht.readTemperature(); //Assign variable to store temperature 
  //int h = dht.readHumidity(); //Assign variable to store humidity 
  //int light = analogRead(LighsensorPin); //Assign variable to store light sensor values
  light = TSL2561.readVisibleLux(); //Assign variable to store light sensor values
  //SERIAL.println("--------------------------------");
  //SERIAL.println(TSL2561.readVisibleLux());


  // Check if any notification should be displayed
  for (int i = 0; i < N_NOTIFICATIONS; i++)
  {
    notifications[i]->check_notification_time();
    if (notifications[i]->get_activation())
    {
      SERIAL.print("Notification ");
      SERIAL.print(i);
      SERIAL.println(" is showing");
      SERIAL.println(notifications[i]->get_text());
      analogWrite(WIO_BUZZER, 128);
      delay(50);
      return;
    }
    analogWrite(WIO_BUZZER, 0);
  }
  if (millis() - lastTrackerCall >= 10000) { // every 10 seconds
    lastTrackerCall = millis();
    tracker.gather_data(h, light, concentration, t);
    tracker.write_data(false);
  }
  delay(50);
}


void DMAC_1_Handler()                                                     // Interrupt handler for DMAC channel 0
{
  static uint8_t count = 0;                                               // Initialise the count
  if (DMAC->Channel[1].CHINTFLAG.bit.SUSP)                                // Check if DMAC channel 0 has been suspended (SUSP)
  {
    DMAC->Channel[1].CHCTRLB.reg = DMAC_CHCTRLB_CMD_RESUME;               // Restart the DMAC on channel 0
    DMAC->Channel[1].CHINTFLAG.bit.SUSP = 1;                              // Clear the suspend (SUSP)interrupt flag
    if (count)                                                            // Test if the count is 1
    {
      results1Ready = true;                                               // Set the results 1 ready flag
    }
    else
    {
      results0Ready = true;                                               // Set the results 0 ready flag
    }
    count = (count + 1) % 2;                                              // Toggle the count between 0 and 1
  }
}


