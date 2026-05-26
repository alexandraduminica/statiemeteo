#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>  
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

const char* ssid     = "WIFI";
const char* password = "WIFI_PASSWORD";
String APIKEY        = "API_KEY_OPENWEATHERMAP";
String CityID        = "683506";  
#define ALTITUDE 90.0                    

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const char* topic_temp = "statie/meteo/locala/temperatura";
const char* topic_hum  = "statie/meteo/locala/umiditate";
const char* topic_pres = "statie/meteo/locala/presiune";

const char* topic_temp_online = "statie/meteo/online/temperatura";
const char* topic_weather_id  = "statie/meteo/online/id_vreme";
const char* topic_weather_txt = "statie/meteo/online/stare_text"; 

#define I2C_SDA 21
#define I2C_SCL 22
#define LED_PIN 2
#define BMP280_ADDRESS 0x77 

#define TFT_SCL    13
#define TFT_SDA    14
#define TFT_RST    4
#define TFT_DC     27
#define TFT_CS     5

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_SDA, TFT_SCL, TFT_RST);
Adafruit_BMP280 bmp; 
Adafruit_AHTX0 aht;  
WiFiClient espClient;     
PubSubClient mqttClient(espClient); 
WiFiClient apiClient;     
char* servername = "api.openweathermap.org";

float local_temperature = 0;
float local_humidity = 0;
float local_pressure = 0;
float online_temperature = 0; 
int weatherID = 0;
String weatherDescRomana = "In asteptare..."; 
int iterations = 1800; 

void initSensors();
void connectToWifi();
void reconnectMQTT();
void getWeatherData();
void readLocalSensors();
void publishMQTTData();
void blinkLED();
void updateDisplayST7735(); 
String getWeatherDescription(int id);

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  delay(200);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);           
  tft.fillScreen(ST7735_BLACK); 
  
  tft.setTextSize(1);
  tft.setTextColor(ST7735_CYAN);
  tft.setCursor(10, 20);
  tft.print("Initializare hardware...");


  Wire.begin(I2C_SDA, I2C_SCL);
  initSensors();
  mqttClient.setServer(mqtt_server, mqtt_port);
  

  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(10, 20);
  tft.setTextColor(ST7735_YELLOW);
  tft.print("Conectare WiFi...");
  
  connectToWifi();
  
  tft.fillScreen(ST7735_BLACK); 
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWifi();
  }

  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop(); 

  if (iterations >= 1800) {
    getWeatherData();
    iterations = 0;   
  }

  readLocalSensors();
  publishMQTTData();
  updateDisplayST7735(); 

  iterations++;
  blinkLED();
  delay(5000); 
}

void connectToWifi() {
  WiFi.enableSTA(true);
  WiFi.begin(ssid, password);
  
  Serial.print("[WiFi] Conectare la retea");
  int timeout = 0;

  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    timeout++;
  }
  if(WiFi.status() == WL_CONNECTED) {
     Serial.println("\n[WiFi] Conectat!");
  } else {
     Serial.println("\n[WiFi] Timeout sau Lipsa date Wi-Fi corecte.");
  }
}

void reconnectMQTT() {
  if (!mqttClient.connected() && WiFi.status() == WL_CONNECTED) {
    Serial.print("[MQTT] Se incearca conectarea...");
    String clientId = "ESP32Client-" + String(random(0, 9999));
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("\n[MQTT] CONECTAT!");
    }
  }
}

void publishMQTTData() {
  if(!mqttClient.connected()) return;
  
  mqttClient.publish(topic_temp, String(local_temperature, 1).c_str());
  mqttClient.publish(topic_hum, String(local_humidity, 1).c_str());
  mqttClient.publish(topic_pres, String(local_pressure, 0).c_str());

  if (online_temperature != 0) {
    mqttClient.publish(topic_temp_online, String(online_temperature, 1).c_str());
    mqttClient.publish(topic_weather_id, String(weatherID).c_str());
    mqttClient.publish(topic_weather_txt, weatherDescRomana.c_str()); 
  }
}

void initSensors() {
  bmp.begin(BMP280_ADDRESS);
  aht.begin();
}

void readLocalSensors() {
  local_temperature = bmp.readTemperature();
  local_pressure = bmp.readPressure() / 100.0F; 
  sensors_event_t humidity_event, temp_event;
  aht.getEvent(&humidity_event, &temp_event);
  local_humidity = humidity_event.relative_humidity;
}

void blinkLED() {
  digitalWrite(LED_PIN, HIGH); delay(50); digitalWrite(LED_PIN, LOW);
}

void getWeatherData() {
  if (WiFi.status() != WL_CONNECTED) return;
  if (!apiClient.connect(servername, 80)) return;

  String url = "/data/2.5/weather?id=" + CityID + "&units=metric&APPID=" + APIKEY;
  apiClient.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + servername + "\r\n" + "Connection: close\r\n\r\n");
  
  unsigned long timeout = millis();
  while (apiClient.available() == 0) {
    if (millis() - timeout > 3000) { apiClient.stop(); return; }
  }
  String result = "";
  while (apiClient.available()) { result = apiClient.readStringUntil('\r'); }
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, result);

  weatherID = doc["weather"][0]["id"];
  online_temperature = doc["main"]["temp"]; 
  weatherDescRomana = getWeatherDescription(weatherID);
}

void updateDisplayST7735() {
  tft.setTextSize(1);
  
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  tft.setCursor(10, 5);
  tft.print("WEATHER STATION IoT");
  tft.drawFastHLine(0, 15, 128, ST7735_BLUE);

  tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);
  tft.setCursor(5, 25);
  tft.print("TEMP. CAMERA: ");
  tft.setTextColor(ST7735_CYAN, ST7735_BLACK);
  tft.print(String(local_temperature, 1) + " C ");

  tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);
  tft.setCursor(5, 40);
  tft.print("UMIDITATE:    ");
  tft.setTextColor(ST7735_CYAN, ST7735_BLACK);
  tft.print(String(local_humidity, 0) + " % ");

  tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);
  tft.setCursor(5, 55);
  tft.print("PRESIUNE:     ");
  tft.setTextColor(ST7735_CYAN, ST7735_BLACK);
  tft.print(String(local_pressure, 0) + " hPa");

  tft.drawFastHLine(0, 70, 128, ST7735_BLUE);

  tft.setTextColor(ST7735_MAGENTA, ST7735_BLACK);
  tft.setCursor(5, 80);
  tft.print("BUCURESTI NET: ");
  tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
  tft.print(String(online_temperature, 1) + " C ");

  tft.setTextColor(ST7735_MAGENTA, ST7735_BLACK);
  tft.setCursor(5, 95);
  tft.print("STARE: ");
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  tft.print(weatherDescRomana + "           "); 
}

String getWeatherDescription(int id) {
  if (id == 800) return "Cer Senin";
  if (id == 801) return "Putin Noros";
  if (id == 802) return "Partial Noros";
  if (id == 803 || id == 804) return "Noros";
  if (id >= 200 && id <= 299) return "Furtuna";
  if (id >= 300 && id <= 399) return "Aversa";
  if (id >= 500 && id <= 599) return "Ploi";
  if (id >= 600 && id <= 699) return "Ninsoare";
  if (id >= 700 && id <= 799) return "Ceata";
  return "Meteo Varia";
}