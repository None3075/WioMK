#include "SmartInterface.h"

String possible_class_type[] = {"magistral", "interactive", "exercises", "laboratory"};
String possible_time_type[] = {"morning", "noon", "afternoon"};
String possible_day_type[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
const String username = "Morelab";

void start_class_menu()
{
    SERIAL.println("Start class menu");  
    int selected_button = 0;

    int current_class_type = 0;
    int students = 0;
    int current_time_type = 0;
    int current_day_type = 0;

    bool confirmed = false;
    SERIAL.println("Starting loop");
    while (!confirmed)
    {
        if (digitalRead(WIO_5S_UP) == LOW) {
            delay(200);
            selected_button = increment_with_check(selected_button, -1, 0, 5);
            SERIAL.println("Button UP pressed");
            SERIAL.print("Current button state ");
            SERIAL.println(selected_button);
        }
        else if (digitalRead(WIO_5S_DOWN) == LOW) {
            delay(200);
            selected_button = increment_with_check(selected_button, 1, 0, 5);
            SERIAL.println("Button LOW pressed");
            SERIAL.print("Current button state ");
            SERIAL.println(selected_button);
        }
        else if (digitalRead(WIO_5S_LEFT) == LOW) {
            delay(200);
            switch (selected_button)
            {
                case 0:
                    current_class_type = increment_with_check(current_class_type, 1, 0, 3);
                    SERIAL.println("Button left pressed in class_type");
                    SERIAL.print("Current class type ");
                    SERIAL.println(selected_button);
                    break;
                case 1:
                    students = increment_with_check(students, 5, 0, 200);
                    SERIAL.println("Button left pressed in students");
                    SERIAL.print("Current students ");
                    SERIAL.println(selected_button);
                    break;
                case 2:
                    current_time_type = increment_with_check(current_time_type, 1, 0, 2);
                    SERIAL.println("Button left pressed in time type");
                    SERIAL.print("Current time type ");
                    SERIAL.println(current_time_type);
                    break;
                case 3:
                    current_day_type = increment_with_check(current_day_type, 1, 0, 6);
                    SERIAL.println("Button left pressed in day type");
                    SERIAL.print("Current day type ");
                    SERIAL.println(current_day_type);
                    break;
                default:
                    SERIAL.println("default break left in start class menu");
                    break;
            }
        }
        else if (digitalRead(WIO_5S_RIGHT) == LOW) {
            delay(200);
            switch (selected_button)
            {
                case 0:
                    current_class_type = increment_with_check(current_class_type, -1, 0, 3);
                    SERIAL.println("Button right pressed in class_type");
                    SERIAL.print("Current class type ");
                    SERIAL.println(selected_button);
                    break;
                case 1:
                    students = increment_with_check(students, -5, 0, 0);
                    SERIAL.println("Button right pressed in students");
                    SERIAL.print("Current students ");
                    SERIAL.println(selected_button);
                    break;
                case 2:
                    current_time_type = increment_with_check(current_time_type, -1, 0, 2);
                    SERIAL.println("Button right pressed in time type");
                    SERIAL.print("Current time type ");
                    SERIAL.println(current_time_type);
                    break;
                case 3:
                    current_day_type = increment_with_check(current_day_type, -1, 0, 6);
                    SERIAL.println("Button right pressed in day type");
                    SERIAL.print("Current day type ");
                    SERIAL.println(current_day_type);
                    break;
                default:
                    SERIAL.println("default break right in start class menu");
                    break;
            }
        }
        else if (digitalRead(WIO_5S_PRESS) == LOW) {
            delay(200);
            switch (selected_button)
            {
                case 4:
                    confirmed = true;
                    SERIAL.println("Press button in confirm box");
                    classAssistant.fill_data((ClassTypes)current_class_type, students, (ClassTimeInDay)current_time_type, (Day)current_day_type);
                    break;
                case 5:
                    confirmed = true;
                    SERIAL.println("Press button in cancel box");
                    break;
                default:
                    SERIAL.println("default break pressed in start class menu");
                    break;
            }
        }

        SERIAL.println("Starting menu display");
        SERIAL.println("End start menu lcd display");
        delay(50);
    }
    SERIAL.println("End start menu loop");
}


bool change_block_menu(String block)
{
    int selected_button = 0;
    SERIAL.println("Starting change block menu loop");
    bool confirmed = false;
    bool return_value = false;
    while (!confirmed)
    {
        if (digitalRead(WIO_5S_LEFT) == LOW || digitalRead(WIO_5S_RIGHT) == LOW)
        {
            delay(200);
            selected_button = increment_with_check(selected_button, 1, 0, 1);
            SERIAL.println("Button left or right pressed in block menu");
            SERIAL.print("Current selected button ");
            SERIAL.println(selected_button);
        }
        else if (digitalRead(WIO_5S_PRESS) == LOW)
        {
            delay(200);
            SERIAL.println("Press button");
            confirmed = true;
            if (selected_button == 0)
            {
                SERIAL.println("Confirmed change block");
                return_value = true;
            }
            else
            {
                SERIAL.println("Canceled change block");
                return_value = false;
            }
        }

        SERIAL.println("Starting lcd change block information");
        SERIAL.println("Pushed lcd information");
        delay(50);
    }
    SERIAL.println("Finished change block menu");
    return return_value;
}

int end_class_menu()
{
    SERIAL.println("Started end class menu");
    bool confirmed = false;
    int rate = 5;
    int selected_button = 0;

    while(!confirmed)
    {
        if (selected_button == 0 && (WIO_5S_DOWN) == LOW)
        {
            delay(200);
            rate = increment_with_check(rate, -1, 1, 10);
            SERIAL.println("Button down pressed in end class menu");
            SERIAL.print("Current rate ");
            SERIAL.println(selected_button);
        }
        else if (selected_button == 0 && digitalRead(WIO_5S_UP) == LOW)
        {
            delay(200);
            rate = increment_with_check(rate, 1, 1, 10);
            SERIAL.println("Button up pressed in end class menu");
            SERIAL.print("Current rate ");
            SERIAL.println(selected_button);
        }
        else if (selected_button > 0 && (digitalRead(WIO_5S_LEFT) == LOW || digitalRead(WIO_5S_RIGHT) == LOW))
        {
            delay(200);
            selected_button = increment_with_check(selected_button, 1, 1, 2);
            SERIAL.println("Button left or right pressed in end class menu");
            SERIAL.print("Current selected button ");
            SERIAL.println(selected_button);
        }
        else if (digitalRead(WIO_5S_PRESS) == LOW)
        {
            delay(200);
            if (selected_button == 0)
            {
                selected_button = 1;
                SERIAL.println("Button pressed  going to confirm cancel buttons in end class menu");
                SERIAL.print("Current selected ");
                SERIAL.println(selected_button);
            }
            else if (selected_button == 1)
            {
                confirmed = true;
                SERIAL.println("Confirmed end class menu");
            }
            else
            {
                confirmed = true;
                rate = -1;
                SERIAL.println("Canceled end class menu");
            }

        }

        SERIAL.println("Start lcd showing for end class menu");

        for (int i=10; i > 0; i--)
        {
            SERIAL.print(i);
        }
        SERIAL.println("Finish punctuation boxs");

        SERIAL.println("Pushed lcd information");
        delay(50);
    }

    return rate + 1;
}


int increment_with_check(int n, int delta, int min, int max)
{
    n += delta;
    if (n < min)
        n = max;
    if (n > max)
        n = min;

    return n;
}
