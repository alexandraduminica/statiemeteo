Mini - TV Weather Station

1. Project Description

  This project represents a meteorological monitoring station built around ESP32 microcontroller. The system integrates local data acquisition via physical hardware sensors with Cloud computing and REST APIs, aiming to monitor indoor environmental parameters and correlate them with regional weather forecasts.

  The system implements a complete data pipeline (Edge-to-Cloud-to-EndUser):
  - Edge Data Fetching: Interrogates the OpenWeatherMap REST API over HTTPS to retrieve real-time weather conditions and meteorological status codes for the selected region, more exactly Bucharest.
  - Local Hardware Display: Renders the acquired telemetry locally onto a color TFT ST7735 display utilizing the SPI protocol.
  - Cloud Publishing (MQTT): Dispatches both local and fetched external telemetry asynchronoulsy across the internet to a public MQTT broker (HiveMQ).
  - Dual-Dashboard (Web & Mobile): Facilitates remote data tracking thorugh a dedicated mobile application (IoT MQTT Panel) and an interactive, responsive web page.

2. System Arhitecture

  The diagram below illustrates the multi-tier structural topology, information routing and network protocol orchestration utilzied in this IoT station:

<img width="483" height="747" alt="image" src="https://github.com/user-attachments/assets/dddca07c-741c-49f2-bae5-e991fd315688" />

3. Hardware Components and Techincal Specifications

   a. Microcontroller ESP32 DevKitBoard (38-Pin Version)
     - Processor: Dual-Core Xtensa 32-bit LX6 SoC, clocking speeds up to          240 MHz.
     - Connectivity: Integrated Wi-Fi and Bluetooth Low Energy stack.
     - Project Role: Centralized Processing Edge Node. Handles the                TCP/IP stack, queries I2C bus, parses incoming HTTPS JSON                  strings, renders the local GUI on the SPI screen and manages               asynchronus MQTT publishing.
    - Pinout Map: GPIO21 - SDA (Sensor I2C Bus Data), GPIO22 - SCL               (Sensor I2C Bus Clock), GPIO13 - SCK/Clock (TFT Screen SPI BUS),           GPIO14 - MOSI/SDA (TFT Screen SPI Bus Data), GPIO27 - DC                   (Data/Command Selection - TFT), GPIO4 - RST (Hardware Reset -              TFT), GPIO5 - CS (Chip Select - TFT)

   b. Sensors: AHT20 + BMP280 Combo Module
     This space-saving module incorporates two specialized silicon sensor   dies on a single breakout board, utilizing unique Hex addresses on a       shared I2C physical backplane:
     - BMP280 Barometric Sensor: measures athmospheric preassure ranging          from 300 to 1100 hPa and ambient temperature with an accuracy of           +/- 0.5 Celsius degrees.
     - AHT20 Humidity Sensor: measures relative humidity across 0-100%.
  
   c. Display: ST7735 Color Screen
    -  Technology: Active TFT LCD screen
    -  Interface: High Speed SPI
    -  Role: Provides immediate local visual telemetry. Displays network connection milestones ("Connecting to WiFi...") and renders real-time data.

<img width="1067" height="965" alt="image" src="https://github.com/user-attachments/assets/d18854ce-95ca-4c64-aa3d-ff897ff52a90" />

4. Functional Algorithm
    1. Hardware Initialization: The system mounts a software Wire instance on the desigated I2C pins and verifies that target chips are active. The ST7735 display engine is initialiez to render the custom boot layout.
    2. Network Synchronization: The ESP32 boots its station modul, authenticates with the local WI-FI access point, and obtains a dynamic IP address via DHCP. The step-by-step progress is logged onto the TFT display.
    3. REST API Harvesting: At scheduled intervals, the station dispatches a secure HTTPS GET request to the OpenWeatherMap endpoint using Bucharest's city ID. The raw JSON payload is parsed using the Arduino.Json library to extract current regional temperature benchmarks and the numeric weather condition ID.
    4. Local Environment Sampling: Every 5 seconds, the ESP32 queries the internal registers of the physical sensors via the I2C bus, gathering highly accurate room temperature, pressure and relative humidity percentage.
    5. Edge UI Refresh: The updated datasets (local temperature, local humidity, barometric pressure, external temperature and translated condition text) are cleanly refreshed on the ST7735 screen
    6. Cloud Transmission (MQTT Push): The system verifies its state with the public HiveMQ broker.
       
5. User Interfaces (Dashboard)

    A. Mobile Dashboard (IoT MQTT Panel): The terminal interface maps live feeds into specialized user components on a smartphone by binding to the HiveMQ message cloud. The app processes the following distinct paths published by the ESP32:
   - statie/meteo/locala/temperatura: Renders a dynamic radial gauge            highlighting indoor temperature values.
   - statie/meteo/locala/umiditate: Renders an instrumentation dial             reflecting room humidity levels.
   - statie/meteo/locala/presiune: Houses an explicit text field                outputting local ambient pressure.
   - statie/meteo/online/temperatura: Tracks the OpenWeather base station       regional reference layer.
   - statie/meteo/online/id_vreme & stare_text: Feeds string blocks             displaying current translated weather data.

    B. Web Dashboard: The interfata_web.html frontend application resides in the root directory and is automatically compiled into a live, public-facing dashboard. The application utilizes an MQTT over WebSockets library to establish a persistent connection with the public HiveMQ cloud instance straight from the client's web browser.

<img width="849" height="1600" alt="image" src="https://github.com/user-attachments/assets/b2b87906-40d7-4e6e-a7d2-2a0a035f1002" />

    
<img width="2559" height="1510" alt="image" src="https://github.com/user-attachments/assets/bfd3794d-ea3a-41f0-b00d-c3387468cad4" />

6. Software Stack and Dependencies
  The firmware arhitecture was compiled inside the PlatformIO ecosystem targetting the Arduion framework abstraction layers:
  - Adafruit_GFX & Adafruit_ST7735 - High speed hardware control for the SPI TFT monitor.
  - Adafruit_BMP280_library & Adafruit_AHT20 - Low-level register drivers for I2C sensory stack.
  - ArduinoJson - zero-copy parsing of massive OpenWeatherMap REST payloads.
  - PubSubClient - Robust asynchronus message routing handling MQTT over TCP connections.





