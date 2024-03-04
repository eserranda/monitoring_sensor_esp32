#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HardwareSerial.h>

#define BUZZER_PIN 16        // Sesuaikan dengan pin yang Anda gunakan untuk buzzer
#define BUZZER_CHANNEL 0     // Kanal PWM untuk buzzer
#define LED_CHANNEL2 1       // Kanal PWM untuk LED 2
#define BUZZER_RESOLUTION 8  // Resolusi PWM untuk buzzer


#define LED_PIN1 18  // Pin LED 1
#define LED_PIN2 17  // Pin LED 2

const char *ssid = "Yos";
const char *password = "yoelmgs01";

const char *damkarStatus = "http://damkar.helloelrand.com/api/damkar-status";

String apiKey = "posko_btp";

unsigned long lastSensorReadTime = 0;
const int sendHttpReadInterval = 3000;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);  // Konfigurasi pin LED sebagai output

  ledcSetup(BUZZER_CHANNEL, 5000, BUZZER_RESOLUTION);  // Konfigurasi kanal PWM untuk buzzer
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);           // Menghubungkan pin buzzer ke kanal PWM

  ledcSetup(LED_CHANNEL2, 5000, BUZZER_RESOLUTION);  // Konfigurasi kanal PWM untuk LED 2
  ledcAttachPin(LED_PIN2, LED_CHANNEL2);             // Menghubungkan pin LED 2 ke kanal PWM


  checkWiFiConnection();
  delay(3000);
}

void loop() {

  if (millis() - lastSensorReadTime > sendHttpReadInterval) {
    Serial.println("Cek Mode...");
    sendHttpRequest(damkarStatus, apiKey);  // Baca status
    lastSensorReadTime = millis();          // Catat waktu terakhir pembacaan data
  }
}

void sendHttpRequest(const char *damkarStatus, String apiKey) {
  HTTPClient http;
  http.begin(damkarStatus);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST("apiKey=" + String(apiKey));

  if (httpCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Code: " + String(httpCode));
    Serial.println("Response: " + response);

    if (response.startsWith("\"") && response.endsWith("\"")) {
      response = response.substring(1, response.length() - 1);
    }

    if (response == "1") {
      Serial.println("Nyalakan alarm");
      turnOnBuzzer();
    } else if (response == "0") {
      Serial.println("Matikan alarm");
      turnOffBuzzer();
    }
  } else {
    Serial.println("Failed to make HTTP request");
  }
  http.end();
}

void turnOnBuzzer() {
  for (int j = 0; j < 3; j++) {  // Sirine berbunyi tiga kali
    for (int i = 0; i < 255; i++) {
      ledcWrite(BUZZER_CHANNEL, i);             // Mengatur lebar pulsa PWM untuk bunyi sirine
      int brightness = map(i, 0, 255, 0, 255);  // Mengatur kecerahan LED sesuai dengan tingkat suara sirine
      ledcWrite(LED_CHANNEL2, brightness);      // Mengatur kecerahan LED 2 sesuai dengan tingkat suara sirine
      delay(5);
    }

    for (int i = 255; i > 0; i--) {
      ledcWrite(BUZZER_CHANNEL, i);             // Mengatur lebar pulsa PWM untuk bunyi sirine
      int brightness = map(i, 0, 255, 0, 255);  // Mengatur kecerahan LED sesuai dengan tingkat suara sirine
      ledcWrite(LED_CHANNEL2, brightness);      // Mengatur kecerahan LED 2 sesuai dengan tingkat suara sirine
      delay(5);
    }
  }
}

void turnOffBuzzer() {
  // Isi dengan logika untuk mematikan buzzer
  // Misalnya:
  ledcWrite(BUZZER_CHANNEL, 0);  // Atur lebar pulsa PWM menjadi 0 untuk mematikan bunyi buzzer
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
      digitalWrite(LED_PIN1, HIGH);  // turn the LED on
      delay(1000);
      digitalWrite(LED_PIN1, LOW);  // turn the LED on
      Serial.println("Connecting to WiFi...");
      delay(500);
    }
    if (WiFi.status() == WL_CONNECTED) {
      digitalWrite(LED_PIN1, HIGH);  // turn the LED on
      Serial.println("Successfully connected to wifi");
    } else {
      Serial.println("Failed to connect to WiFi");
    }
  }
}

// void bunyikanAlarm() {
//   int frekuensiAwal = 500;     // Frekuensi awal alarm fire warning
//   int frekuensiAkhir = 2500;   // Frekuensi akhir alarm fire warning
//   int langkahFrekuensi = 100;  // Langkah perubahan frekuensi
//   int durasiBunyi = 300;       // Durasi setiap bunyi dalam milidetik
//   int jedaAntarBunyi = 200;    // Jeda antara setiap bunyi dalam milidetik

//   int ledPin = 13;
//   ledHijau(ledPin, OUTPUT);

//   while (!matikanAlarm) {  // false

//     for (int j = frekuensiAwal; j <= frekuensiAkhir; j += langkahFrekuensi) {
//       tone(buzzerPin, j);          // Mulai bunyi dengan frekuensi j
//       digitalWrite(ledPin, HIGH);  // Nyalakan LED selama bunyi
//       delay(3);                    // Tahan bunyi dan LED selama 3 ms (sesuaikan dengan kebutuhan)
//       digitalWrite(ledPin, LOW);   // Matikan LED
//     }

//     delay(durasiBunyi);  // Biarkan bunyi selama durasi yang ditentukan

//     digitalWrite(ledPin, LOW);
//     delay(jedaAntarBunyi);
//   }
//   noTone(buzzerPin);
//   digitalWrite(ledPin, LOW);
// }