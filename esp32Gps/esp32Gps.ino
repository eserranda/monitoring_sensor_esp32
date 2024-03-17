#include <TinyGPSPlus.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HardwareSerial.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char *ssid = "Yos";
const char *password = "yoelmgs01";

TinyGPSPlus gps;

//------Lcd Oled---------//
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
//-- end Lcd Oled-- //

const char *mode = "http://damkar.helloelrand.com/api/esp-mode";
const char *saveLoc = "http://damkar.helloelrand.com/api/save-loc";

// const char *mode = "http://192.168.2.195:8000/api/esp-mode";
// const char *saveLoc = "http://192.168.2.195:8000/api/save-loc";

String apiKey = "Sensor01";

unsigned long lastSensorReadTime = 0;
const int sendHttpReadInterval = 3000;

bool mode_gps = false;
 

// Gambar warna putih, opsi pilih Black
const unsigned char logo_wifi[] PROGMEM = {
  0x07, 0xc0, 0x3f, 0xf8, 0x70, 0x1c, 0xc7, 0xc6, 0x1f, 0xf0, 0x30, 0x18, 0x07, 0xc0, 0x0c, 0x60,
  0x01, 0x00, 0x03, 0x80, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char logo_wifi_black[] PROGMEM = {
  0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe,
  0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe
};

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Jika oled tidak terdetesi maka loop tidak akan di proses
  }

  display.clearDisplay();
  checkWiFiConnection();
  delay(2000);
}

void loop() {
  // updateSerial();
  if (millis() - lastSensorReadTime > sendHttpReadInterval) {
    Serial.println("Cek Mode...");
    sendHttpRequest(mode, apiKey);  // Baca status
    lastSensorReadTime = millis();  // Catat waktu terakhir pembacaan data
  }

  while (Serial2.available() > 0)
    if (gps.encode(Serial2.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
    // while (true)
    //   ;
    delay(50);
  }
}

void displayInfo() {
  Serial.print(F("Location: "));
  if (gps.location.isValid()) {
    Serial.print("Lat: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print("Lng: ");
    Serial.print(gps.location.lng(), 6);
    // Serial.println();
    double lat = gps.location.lat();
    double lng = gps.location.lng();
    saveLocation(saveLoc, apiKey, lat, lng);
    delay(1000);
  } else {
    Serial.println(F("INVALID"));
  }
}

void saveLocation(const char *saveLoc, String apiKey, double lat, double lng) {
  HTTPClient http;
  http.begin(saveLoc);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String latStr = String(lat, 6);
  String lngStr = String(lng, 6);

  Serial.print("Lat :");
  Serial.println(latStr);
  Serial.print("Lng :");
  Serial.println(lngStr);

  String payload = "apiKey=" + apiKey + "&lat=" + latStr + "&lng=" + lngStr;
  int httpCode = http.POST(payload);
  String response = "";

  if (httpCode == 200) {
    response = http.getString();
    Serial.println("Response: " + response);
    mode_gps = false;
  } else if (httpCode == 404) {
    Serial.println("HTTP Error 404: Not Found");
  } else {
    Serial.println("Failed to make HTTP request");
  }
  http.end();
}

void sendHttpRequest(const char *mode, String apiKey) {
  HTTPClient http;
  http.begin(mode);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST("apiKey=" + String(apiKey));

  if (httpCode == 200) {
    String response = http.getString();
    Serial.println("HTTP Code: " + String(httpCode));
    Serial.println("Response: " + response);

    response.remove(0, 1);
    response.remove(response.length() - 1);

    if (response == "gps") {
      Serial.println("Mode gps");
      mode_gps = true;
      while (Serial2.available() > 0)
        if (gps.encode(Serial2.read()))
          displayInfo();
    } else if (response == "manual") {
      mode_gps = false;
    }
  } else if (httpCode == 404) {
    Serial.println("Not Found!");
  } else if (httpCode == 500) {
    Serial.println("Server Error");
  } else {
    Serial.println("Failed to make HTTP request");
  }
  http.end();
}

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Not connected to WiFi, trying to connect...");
    WiFi.begin(ssid, password);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED) {
      if (millis() - start >= 5000) {
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
      Serial.println("Successfully connected to wifi");
      display.drawBitmap(0, 0, logo_wifi, 15, 15, 1);
      display.display();
    } else {
      Serial.println("Failed to connect to WiFi");
    }
  }
}
// void updateSerial() {
//   delay(500);
//   while (Serial.available()) {
//     Serial2.write(Serial.read());  //Forward what Serial received to Software Serial Port
//   }
//   while (Serial2.available()) {
//     Serial.write(Serial2.read());  //Forward what Software Serial received to Serial Port
//   }
// }
