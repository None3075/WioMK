# WioMK: Sistema IoT para Aulas Inteligentes

## SavaDataInterface

### Descripción
SavaDataInterface es un módulo del sistema WioMK que permite monitorizar variables ambientales en el aula (temperatura, humedad, calidad del aire y luminosidad) y evaluar el proceso de enseñanza-aprendizaje. Esta versión está optimizada para maximizar la confiabilidad en la recolección y envío de datos, prescindiendo de la interfaz gráfica en la pantalla del dispositivo.

### Características Principales
- Monitorización continua de variables ambientales del aula
- Gestión de sesiones de clase (inicio, fin, descansos)
- Transmisión de datos a servidor web en tiempo real
- Optimizado para máxima estabilidad y duración de batería

## Instalación y Configuración

### 1. Configuración del Entorno WiFi
Edite el archivo `ClassAssistant.cpp` para configurar su red:

```cpp
const char* ssid = "SU_RED_WIFI";
const char* password = "SU_CONTRASEÑA";
```

### 2. Configuración del Servidor
En el mismo archivo, configure la dirección del servidor:

```cpp
const char* serverUrl = "http://SU_IP_SERVIDOR:8000/upload/";
```

### 3. Configuración de Autenticación
Edite `tracker.cpp` (línea 107) para establecer su token:

```cpp
http.addHeader("userToken", "SU_TOKEN"); // default is "Patata"
```

### 4. Carga del Firmware
Cargue el sketch `SavaDataInterface.ino` a la Wio Terminal usando Arduino IDE.

## Uso del Dispositivo

### Interfaz Física
SavaDataInterface utiliza un sistema de control mediante botones físicos:

- **Botón A (Izquierda)**: Desactivado en esta versión
- **Botón B (Centro)**: Inicia y termina la sesión de clase
- **Botón C (Derecha)**: Inicia o finaliza el descanso/break dentro de una sesion

### Ciclo de Funcionamiento
1. **Inicio de Sesión**: Presione el botón B para iniciar la sesión
2. **Durante la Sesión**: El dispositivo recolecta datos ambientales automáticamente y los envia al servidor web
3. **Descanso**: Presione el botón C para iniciar el descanso
3. **Descanso**: Presione el botón C para terminar el descanso
4. **Finalización**: Presione nuevamente el botón B para finalizar la sesión

### Monitorización
Los datos se envían automáticamente al servidor web configurado. No se muestra información en la pantalla del dispositivo debido a limitaciones tecnicas del propio dispositivo.

## Detalles Técnicos
El dispositivo envía datos en formato CSV con las siguientes métricas:
- Temperatura (°C)
- Humedad relativa (%)
- Calidad del aire (ppm CO₂)
- Luminosidad (lux)
- Eventos de sesión (inicio, descanso, fin)

Los datos son identificados con timestamp para su posterior análisis.