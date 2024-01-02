#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char *ssid = "test";
const char *password = "123456789";

const char *uri1 = "http://damkar.helloelrand.com/api/data1";
const char *uri2 = "http://damkar.helloelrand.com/api/data2";


const unsigned long interval = 5000;   
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long currentMillis = millis();

  // Buat permintaan HTTP setiap interval
  if (currentMillis - previousMillis >= interval) {
    sendHttpRequest(uri1, "Data 1");
    sendHttpRequest(uri2, "Data 2");

    // Catat waktu terakhir permintaan dibuat
    previousMillis = currentMillis;
  }
}

void sendHttpRequest(const char *url, const char *data) {
  HTTPClient http;

  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST("data=" + String(data));

  if (httpCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Code: " + String(httpCode));
    Serial.println("Response: " + response);
  } else {
    Serial.println("Failed to make HTTP request");
  }

  http.end();
}

