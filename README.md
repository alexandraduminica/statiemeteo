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

<img width="1067" height="965" alt="image" src="https://github.com/user-attachments/assets/d18854ce-95ca-4c64-aa3d-ff897ff52a90" />




<img width="849" height="1600" alt="image" src="https://github.com/user-attachments/assets/b2b87906-40d7-4e6e-a7d2-2a0a035f1002" />


<img width="2559" height="1510" alt="image" src="https://github.com/user-attachments/assets/bfd3794d-ea3a-41f0-b00d-c3387468cad4" />

