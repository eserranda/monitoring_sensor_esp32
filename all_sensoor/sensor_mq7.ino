#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>

//Include the library
#include <MQUnifiedsensor.h>
/************************Hardware Related Macros************************************/
#define Board ("ESP-32")  // Wemos ESP-32 or other board, whatever have ESP32 core.

//https://www.amazon.com/HiLetgo-ESP-WROOM-32-Development-Microcontroller-Integrated/dp/B0718T232Z (Although Amazon shows ESP-WROOM-32 ESP32 ESP-32S, the board is the ESP-WROOM-32D)
#define PinMQ7 (35)  //check the esp32-wroom-32d.jpg image on ESP32 folder
// #define PinMQ2 (36)  //check the esp32-wroom-32d.jpg image on ESP32 folder

/***********************Software Related Macros************************************/
#define TypeMQ7 ("MQ-7")  //MQ2 or other MQ Sensor, if change this verify your a and b values.
// #define TypeMQ2 ("MQ-2")          //MQ2 or other MQ Sensor, if change this verify your a and b values.
#define Voltage_Resolution (3.3)  // 3V3 <- IMPORTANT. Source: https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/
#define ADC_Bit_Resolution (12)   // ESP-32 bit resolution. Source: https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/
#define RatioMQ7CleanAir (27.5)   //RS / R0 = 27.5 ppm
// #define RatioMQ2CleanAir (9.83)   //RS / R0 = 9.83 ppm
/*****************************Globals***********************************************/

#define DHT_SENSOR_PIN 15
#define DHT_SENSOR_TYPE DHT22

MQUnifiedsensor MQ7(Board, Voltage_Resolution, ADC_Bit_Resolution, PinMQ7, TypeMQ7);
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

const char* ssid = "Yos";
const char* password = "yoelmgs01";

const int sensor_api = 32;
const int sensor_gas = 34;

const int SMOKE_THRESHOLD = 800;

String send_data = "http://192.168.2.195/ferin/public/esp";
String apiKey = "Sensor02";  // Kunci dari segala-galanya
// String apiKey = "Sensor01";  // Kunci dari segala-galanya


const unsigned long interval = 2000;  // Interval waktu dalam milidetik (3 detik)
unsigned long previousMillis = 0;


void setup() {

  //Init the serial port communication - to debug the library
  Serial.begin(115200);  //Init serial port
  delay(10);

  pinMode(sensor_api, INPUT);
  dht_sensor.begin();  // initialize the DHT sensor
  //Set math model to calculate the PPM concentration and the value of constants
  MQ7.setRegressionMethod(1);  //_PPM =  a*ratio^b
  // MQ2.setRegressionMethod(1);  //_PPM =  a*ratio^b

  MQ7.setA(99.042);
  MQ7.setB(-1.518);  // Configure the equation to to calculate H2 concentration

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

  Serial.print("Gas Sensor  : ");
  Serial.print(nilai_sensor_gas);
  Serial.println(" PPM");

  Serial.print("Api Sensor  : ");
  Serial.print(nilai_sensor_api);
  Serial.println(" PPM");

  Serial.print("CO Sensor   : ");
  Serial.print(nilai_sensor_co);
  Serial.println(" PPM");

  Serial.print("Suhu Sensor : ");
  Serial.print(nilai_sensor_suhu);
  Serial.println("°C");
  Serial.println("");

  if (nilai_sensor_gas > 1800) {
    Serial.println("Gas Terdeteksi");
  }

  // unsigned long currentMillis = millis();  // Waktu saat ini
  // if (currentMillis - previousMillis >= interval) {
  //   previousMillis = currentMillis;  // Simpan waktu permintaan terakhir
  //   String url = send_data + "?nilai_sensor_gas=" + nilai_sensor_gas + "&nilai_sensor_co=" + nilai_sensor_co + "&nilai_sensor_api=" + nilai_sensor_api + "&nilai_sensor_suhu=" + nilai_sensor_suhu + "&apiKey=" + apiKey;

  //   HTTPClient http;

  //   http.begin(url);
  //   int httpcode = http.GET();
  //   if (httpcode > 0) {
  //     String response = http.getString();
  //     // Serial.println("Response: " + response);
  //   } else {
  //     Serial.println("Failed to make HTTP request");
  //   }
  //   http.end(); 
  // }

  // String url = send_data + "?nilai_sensor_gas=" + nilai_sensor_gas + "&nilai_sensor_co=" + nilai_sensor_co + "&nilai_sensor_api=" + nilai_sensor_api + "&nilai_sensor_suhu=" + nilai_sensor_suhu + "&apiKey=" + apiKey;

  // HTTPClient http;

  // http.begin(url);
  // int httpcode = http.GET();
  // if (httpcode > 0) {
  //   String response = http.getString();
  //   // Serial.println("Response: " + response);
  // } else {
  //   Serial.println("Failed to make HTTP request");
  // }
  // http.end();

  delay(500);
}
