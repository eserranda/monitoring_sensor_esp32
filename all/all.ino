#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

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

MQUnifiedsensor MQ7(Board, Voltage_Resolution, ADC_Bit_Resolution, PinMQ7, TypeMQ7);
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

const char *ssid = "Yos";
const char *password = "yoelmgs01";

const int sensor_api = 32;
const int sensor_gas = 34;

const int SMOKE_THRESHOLD = 800;

const char *uri = "http://192.168.2.195:8000/api/data";
String apiKey = "Sensor01";

const unsigned long interval = 5000;
unsigned long previousMillis = 0;

void setup() {

  Serial.begin(115200);
  delay(10);

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

  Serial.println();
  Serial.println();
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
  MQ7.update();
  float nilai_sensor_co = MQ7.readSensor();
  int nilai_sensor_gas = analogRead(sensor_gas);
  int nilai_sensor_api = digitalRead(sensor_api);
  float nilai_sensor_suhu = dht_sensor.readTemperature();

  if (isnan(nilai_sensor_suhu)) {
    Serial.println("Sensor Suhu Tidak Terhubung");
  }

  // Serial.print("Gas Sensor  : ");
  // Serial.print(nilai_sensor_gas);
  // Serial.println(" PPM");

  // Serial.print("Api Sensor  : ");
  // Serial.print(nilai_sensor_api);
  // Serial.println(" PPM");

  // Serial.print("CO Sensor   : ");
  // Serial.print(nilai_sensor_co);
  // Serial.println(" PPM");

  // Serial.print("Suhu Sensor : ");
  // Serial.print(nilai_sensor_suhu);
  // Serial.println("°C");
  // Serial.println("");

  if (nilai_sensor_gas > 1800) {
    Serial.println("Gas Terdeteksi");
    String nilai_sensor_gas = "1";
    String sensor = "sensor_gas";
    sendHttpRequest(uri, apiKey, sensor, nilai_sensor_gas);
  }

  if (nilai_sensor_api == 0) {
    Serial.println("Api Terseteksi");
    String nilai_sensor_api_str = String(nilai_sensor_api);
    String sensor = "sensor_api";
    sendHttpRequest(uri, apiKey, sensor, nilai_sensor_api_str);
  }

  delay(500);
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