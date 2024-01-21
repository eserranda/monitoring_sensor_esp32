#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <MQUnifiedsensor.h>

#define Board ("ESP-32")
#define PinMQ7 (35)
#define TypeMQ7 ("MQ-7")
#define Voltage_Resolution (3.3)
#define ADC_Bit_Resolution (12)
#define RatioMQ7CleanAir (27.5)
#define DHT_SENSOR_PIN 15
#define DHT_SENSOR_TYPE DHT22
#define BUZZER 17

const int button = 18;

MQUnifiedsensor MQ7(Board, Voltage_Resolution, ADC_Bit_Resolution, PinMQ7, TypeMQ7);
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

//------Lcd Oled---------//
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
//-- end Lcd Oled-- //

const char *ssid = "Yos";
const char *password = "yoelmgs01";


const int sensor_api = 32;
const int sensor_gas = 34;

const int SMOKE_THRESHOLD = 800;

// const char *uri = "http://192.168.2.195:8000/api/data";
const char *uri = "http://192.168.21.65:8000/api/data";
String apiKey = "Sensor01";

const unsigned long timeout = 5000;  // waktu menghubungkan ke wifi

// Gambar warna putih, opsi pilih Black
const unsigned char logo_wifi[] PROGMEM = {
  0x07, 0xc0, 0x3f, 0xf8, 0x70, 0x1c, 0xc7, 0xc6, 0x1f, 0xf0, 0x30, 0x18, 0x07, 0xc0, 0x0c, 0x60,
  0x01, 0x00, 0x03, 0x80, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char logo_wifi_black[] PROGMEM = {
  0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe,
  0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe
};

const unsigned char logo_internet[] PROGMEM = {
  0x07, 0xc0, 0x1d, 0x70, 0x29, 0x28, 0x49, 0x24, 0x7f, 0xfc, 0x91, 0x12, 0x91, 0x12, 0xff, 0xfe,
  0x91, 0x12, 0x91, 0x12, 0x7f, 0xfc, 0x49, 0x24, 0x29, 0x28, 0x1d, 0x70, 0x07, 0xc0
};

const unsigned char logo_no_internet[] PROGMEM = {
  0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe,
  0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char black_line[] PROGMEM = {
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF
};

const unsigned char black_line_api[] PROGMEM = {
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF
};

//------------- Alert Sound Effect --------------//
int melody[] = {
  1500, 1500, 1500, 1200, 1200, 1200, 1500, 1500, 1500, 1200, 1200, 1200
};

int noteDuration[] = {
  500, 300, 500, 500, 300, 500, 500, 300, 500, 500, 300, 500
};

int melodyLength = sizeof(melody) / sizeof(melody[0]);

void alertBuzzer() {
  for (int i = 0; i < melodyLength; i++) {
    ledcWriteTone(0, melody[i]);
    delay(noteDuration[i]);
    ledcWrite(0, 0);
    delay(50);
  }
}


// Variabel untuk menyimpan status tombol (HIGH atau LOW)
// int buttonState = HIGH;

unsigned long lastSensorReadTime = 0;
const int sensorReadInterval = 1000;  // Interval pembacaan sensor dalam  milidetik (1 detik)

unsigned long lastCheckWiFiTime = 0;  // Variabel untuk menyimpan waktu terakhir pengecekan WiFi

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Not connected to WiFi, trying to connect...");
    WiFi.begin(ssid, password);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED) {
      if (millis() - start >= timeout) {
        Serial.println("Gagal Menghubungkan ke Wifi!");
        Serial.println("Mode Offline");
        break;
      }
      delay(1000);
      Serial.println("Connecting to WiFi...");
      display.drawBitmap(0, 0, logo_wifi, 15, 15, 1);
      display.display();
      delay(500);
      display.drawBitmap(0, 0, logo_wifi_black, 15, 15, 0);
      display.display();
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected to WiFi");
      display.drawBitmap(0, 0, logo_wifi, 15, 15, 1);
      display.display();
    } else {
      Serial.println("Failed to connect to WiFi");
    }
  }
}


void setup() {
  Serial.begin(115200);
  delay(10);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Jika oled tidak terdetesi maka loop tidak akan di proses
  }

  pinMode(sensor_api, INPUT);
  dht_sensor.begin();
  MQ7.setRegressionMethod(1);

  MQ7.setA(99.042);
  MQ7.setB(-1.518);

  MQ7.init();

  Serial.print("Calibrating please wait.");

  float calcR0 = 0;
  for (int i = 1; i <= 10; i++) {
    MQ7.update();
    calcR0 += MQ7.calibrate(RatioMQ7CleanAir);
    Serial.print(".");
  }
  MQ7.setR0(calcR0 / 10);
  Serial.println("  done!.");

  if (isinf(calcR0)) {
    Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
    while (1)
      ;
  }
  if (calcR0 == 0) {
    Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
    while (1)
      ;
  }

  ledcSetup(0, 2000, 8);
  ledcAttachPin(BUZZER, 0);

  display.clearDisplay();
  checkWiFiConnection();
}

void loop() {
  int buttonState = digitalRead(button);
  static int lastButtonState = HIGH;

  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      ledcSetup(0, 0, 0);
      ledcAttachPin(BUZZER, 0);
    }
    lastButtonState = buttonState;
  }

  if (millis() - lastSensorReadTime > sensorReadInterval) {
    // Serial.println("Read Data Sensors...");
    readSensors();                  // Baca sensor
    lastSensorReadTime = millis();  // Catat waktu terakhir pembacaan sensor
  }

  checkWiFiConnection();
}

void readSensors() {
  MQ7.update();
  float nilai_sensor_co = MQ7.readSensor();
  int nilai_sensor_gas = analogRead(sensor_gas);
  int nilai_sensor_api = digitalRead(sensor_api);
  float nilai_sensor_suhu = dht_sensor.readTemperature();

  Serial.print("Sensor Asap  : ");
  Serial.println(nilai_sensor_co);
  Serial.print("Sensor Gas   : ");
  Serial.println(nilai_sensor_gas);
  Serial.print("Sensor Api   : ");
  Serial.println(nilai_sensor_api);
  Serial.print("Suhu Ruangan : ");
  Serial.print(nilai_sensor_suhu);
  Serial.println(" *C");
  Serial.println();



  if (isnan(nilai_sensor_suhu)) {
    Serial.println("Sensor Suhu Tidak Terhubung");
  }

  displayData(nilai_sensor_gas, nilai_sensor_api, nilai_sensor_co, nilai_sensor_suhu);

  if (nilai_sensor_gas > 1800) {
    Serial.println("Gas Terdeteksi");
    String nilai_sensor_gas = "1";
    String sensor = "sensor_gas";
    sendHttpRequest(uri, apiKey, sensor, nilai_sensor_gas);
    alertBuzzer();
  }

  if (nilai_sensor_api == 0) {
    Serial.println("Api Terseteksi");
    String nilai_sensor_api_str = String(nilai_sensor_api);
    String sensor = "sensor_api";
    sendHttpRequest(uri, apiKey, sensor, nilai_sensor_api_str);
    alertBuzzer();
  }
}

void displayData(int gas, int api, float co, float suhu) {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // display.clearDisplay();

  display.drawBitmap(37, 16, black_line_api, 70, 10, 0);  // Api
  display.drawBitmap(37, 27, black_line, 40, 10, 0);      // Gas
  display.drawBitmap(37, 38, black_line, 40, 10, 0);      // CO
  display.drawBitmap(37, 49, black_line, 40, 10, 0);      // Suhu
  display.drawBitmap(25, 2, black_line_api, 95, 10, 0);   // Keterangan

  display.setCursor(0, 16);
  display.print("Api  : ");
  if (api == 1) {
    display.println("Aman");
  } else {
    display.println("Terdeteksi!");
    display.setCursor(25, 3);
    display.print("Api Terdeteksi");
  }

  display.setCursor(0, 27);
  display.print("Gas  : ");
  display.println(gas);

  if (gas > 2) {
    display.setTextSize(1);
    display.setCursor(25, 3);
    display.print("Gas Terdeteksi");
  }

  display.setCursor(0, 38);
  display.print("Asap : ");
  display.println(co);

  if (co > 2) {
    display.setTextSize(1);
    display.setCursor(25, 3);
    display.print("Asap Terdeteksi");
  }

  display.setCursor(0, 49);
  display.print("Suhu : ");
  display.print(suhu, 1 );
  display.print( (char)247);  // degree symbol
  display.println("C");

  if (suhu > 33) {
    display.setTextSize(1);
    display.setCursor(25, 3);
    display.print("Suhu Tinggi!");
  }
  display.display();
}

void sendHttpRequest(const char *uri, String apiKey, String sensor, String data) {

  HTTPClient http;
  http.begin(uri);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST("apiKey=" + String(apiKey) + "&sensor=" + sensor + "&data=" + data);

  if (httpCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Code: " + String(httpCode));
    Serial.println("Response: " + response);
  } else {
    Serial.println("Failed to make HTTP request");
  }
  http.end();
}