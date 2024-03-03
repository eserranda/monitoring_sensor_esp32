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

#include <SoftwareSerial.h>


#define Board ("ESP-32")
#define PinMQ7 (35)
#define TypeMQ7 ("MQ-7")
#define Voltage_Resolution (3.3)
#define ADC_Bit_Resolution (12)
#define RatioMQ7CleanAir (27.5)
#define DHT_SENSOR_PIN 15
#define DHT_SENSOR_TYPE DHT22

#define BUTTON_PIN 4

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

const char *uri = "http://192.168.2.195:8000/api/data";
// const char *uri = "http://damkar.helloelrand.com/api/data";
String apiKey = "Sensor02";

const unsigned long timeout = 5000;  // waktu menghubungkan ke wifi

unsigned long lastSensorReadTime = 0;
const int sensorReadInterval = 1000;  // Interval pembacaan sensor dalam  milidetik (1 detik)
unsigned long lastCheckWiFiTime = 0;  // Variabel untuk menyimpan waktu terakhir pengecekan WiFi

bool alarmAktif = false;
bool tombolDitekan = false;
bool emergency = false;

bool httpRequestSentSensorGas = false;
bool httpRequestSentSensorApi = false;
bool httpRequestSentSensorAsap = false;
bool httpRequestSentEmergency = false;

// Deklarasikan variabel penanda waktu deteksi asap
unsigned long waktuDeteksiAsap = 0;

// Deklarasikan variabel penanda suhu awal
float suhuAwal = 0;
bool suhuAwalTersimpan = false;

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

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 1, 3);

  delay(10);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Jika oled tidak terdetesi maka loop tidak akan di proses
  }

  pinMode(sensor_api, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

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

  display.clearDisplay();
  checkWiFiConnection();
  MQ7.serialDebug(true);
}

void loop() {
  // if (Serial.available() > 0) {
  //   String data = Serial.readString();
  //   Serial.print("Data yang diterima: ");
  //   Serial.println(data);
  // }

  int buttonState = digitalRead(BUTTON_PIN);

  if (alarmAktif == true && buttonState == LOW && !tombolDitekan) {
    // Serial.println("Matikan Alarm Ditekan!");
    Serial2.print("AlarmOff\n");
    tombolDitekan = true;  // Set variabel penanda
    alarmAktif = false;
  }

  if (emergency == true && buttonState == LOW && !tombolDitekan) {
    // Serial.println("Matikan Alarm Emergency Ditekan!");
    Serial2.print("AlarmEmergencyOff\n");
    tombolDitekan = true;  // Set variabel penanda
    emergency = false;
  }

  if (alarmAktif == false && buttonState == LOW && !tombolDitekan) {
    // Serial.println("Tombol Emergency Ditekan!");
    Serial2.print("Emergency\n");
    tombolDitekan = true;   
    emergency = true;
  }

  if (buttonState == HIGH) {
    tombolDitekan = false;
  }

  delay(20);

  if (millis() - lastSensorReadTime > sensorReadInterval) {
    // Serial.println("Read Data Sensors...");
    readSensors();                  // Baca sensor
    lastSensorReadTime = millis();  // Catat waktu terakhir pembacaan sensor
  }

  checkWiFiConnection();
}

void readSensors() {
  MQ7.update();                                  // Update data, the arduino will read the voltage from the analog pin
  MQ7.readSensor();  // Sensor will read PPM concentration using the model, a and b values set previously or from the setup
  // MQ7.serialDebug();                             // Will print the table on the serial port
  float nilai_sensor_co = MQ7.readSensor();                // Mq-7
  int gassensorAnalog = analogRead(sensor_gas);            // Mq-6
  int nilai_sensor_api = digitalRead(sensor_api);          // Sensor Api
  float nilai_sensor_suhu = dht_sensor.readTemperature();  //Sensor Suhu


  int buttonState = digitalRead(BUTTON_PIN);  // Tombol
  int nilai_sensor_emergency = 0;

  // Serial.print("Sensor Asap  : ");
  // Serial.println(nilai_sensor_co);
  // Serial.print("Sensor Gas   : ");
  // Serial.println(gassensorAnalog);
  // Serial.print("Sensor Api   : ");
  // Serial.println(nilai_sensor_api);
  // Serial.print("Suhu Ruangan : ");
  // Serial.print(nilai_sensor_suhu);
  // Serial.println(" *C");
  // Serial.println();

  if (isnan(nilai_sensor_suhu)) {
    Serial.println("Sensor Suhu Tidak Terhubung");
  }

  displayData(gassensorAnalog, nilai_sensor_api, nilai_sensor_co, nilai_sensor_suhu);

  // ---------------- Logika Kondisi  ------------//

  // Sensor Gas > 2.00, Buka regulator
  if (gassensorAnalog > 200) {
    Serial2.print("BukaRegulator\n");
    // Serial.println("Gas Terdeteksi");
  }


  if (nilai_sensor_suhu > 37.0 && nilai_sensor_co > 2.00 && nilai_sensor_api == 0 && !httpRequestSentSensorApi) {
    // Serial.println("Asap Terdeteksi, Suhu Meningkat, Api Terdeteksi. Terjadi Kebakaran!!");
    Serial2.print("Kebakaran\n");
    alarmAktif = true;
    String sensor_api = "sensor_api";
    String nilai_sensor_api = "1";
    sendHttpRequest(uri, apiKey, sensor_api, nilai_sensor_api);
    httpRequestSentSensorApi = true;
  } 

  // Kembalikan http Req ke False, Siap mengirim notif ke server
  if (nilai_sensor_co <= 2.00 || nilai_sensor_api != 0) {
    httpRequestSentSensorApi = false;
    suhuAwalTersimpan = false;
  }

  // Sensor Gas > 2.00 dan tombol Emergency == 1 (Ditekan)
  if (gassensorAnalog > 200 && alarmAktif == false && buttonState == LOW && !tombolDitekan) {
    // Serial.println("Tombol Emergency Ditekan!");
    Serial2.print("BukaRegulator\n");
    // Serial.println("Gas Terdeteksi, Tombol Emergency Di tekan");
    tombolDitekan = true;  // Set variabel penanda
    emergency = true;
  }


  // if (nilai_sensor_suhu > 37.0 && nilai_sensor_co > 2.00 && gassensorAnalog > 200 && !httpRequestSentEmergency) {
  //   Serial.println("Asap Terdeteksi, Suhu Meningkat, Gas Terdeteksi. Terjadi Kebakaran!!");
  //   Serial2.print("Emergency\n");
  //   alarmAktif = true;
  //   String emergency = "emergency";
  //   String nilai_emergency = "1";
  //   sendHttpRequest(uri, apiKey, emergency, nilai_emergency);
  //   httpRequestSentEmergency = true;
  // }

  // Kembalikan http Req ke False, Agar Siap mengirim notif ke server
  // if (nilai_sensor_co <= 2.00 && gassensorAnalog <= 200) {
  //   httpRequestSentEmergency = false;
  //   suhuAwalTersimpan = false;
  // }

  // Sensor Api
  // if (nilai_sensor_api == 0 && !httpRequestSentSensorApi) {
  //   Serial.println("Api Terseteksi");
  //   Serial2.print("Api");
  //   alarmAktif = true;
  //   String nilai_sensor_api_str = "1";
  //   String sensor_api = "sensor_api";
  //   sendHttpRequest(uri, apiKey, sensor_api, nilai_sensor_api_str);
  //   httpRequestSentSensorApi = true;
  // }

  // if (nilai_sensor_api != 0) {
  //   httpRequestSentSensorApi = false;
  // }
}

void displayData(int gassensorAnalog, int api, float co, float suhu) {
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
  display.println(gassensorAnalog);

  if (gassensorAnalog > 200) {
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
  display.print(suhu, 1);
  display.print((char)247);  // degree symbol
  display.println("C");

  if (suhu > 37) {
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
    // Serial.println("HTTP Code: " + String(httpCode));
    // Serial.println("Response: " + response);
  } else {
    Serial.println("Failed to make HTTP request");
  }
  http.end();
  alarmAktif == false;
}

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
      Serial.println("Succesfully Connected to Wifi");
      display.drawBitmap(0, 0, logo_wifi, 15, 15, 1);
      display.display();
    } else {
      Serial.println("Failed to connect to WiFi");
    }
  }
}