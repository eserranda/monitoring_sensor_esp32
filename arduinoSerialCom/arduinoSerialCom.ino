#include <SoftwareSerial.h>
#include <Servo.h>


#define Servo_PWM 6
Servo MG995_Servo;

SoftwareSerial Serial2(2, 3);  // 2 tx0, 3 rx1

int buzzerPin = 5;
int ledPin = 13;

String data;
String gpsData;

String receivedMessage;

bool matikanAlarm = false;
bool nyalakanAlarm = false;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);

  pinMode(buzzerPin, OUTPUT);
  MG995_Servo.attach(Servo_PWM);
  MG995_Servo.write(0);
  delay(1000);
  MG995_Servo.detach();  //Stop
}

void loop() {
  if (Serial2.available()) {             // Periksa apakah ada data yang tersedia dari ESP32
    char receivedChar = Serial2.read();  // Baca karakter yang diterima dari ESP32
    receivedMessage += receivedChar;     // Tambahkan karakter ke dalam pesan yang diterima

    if (receivedChar == '\n') {
      if (receivedMessage.equals("Emergency\n")) {
        Serial.println(receivedMessage);
        Serial.println("Emergency Log");
        nyalakanAlarm = true;
        bunyikanSirine(matikanAlarm);
      } else if (receivedMessage.equals("Kebakaran\n")) {
        Serial.println(receivedMessage);
        Serial.println("Kebakaran Log");
        nyalakanAlarm = true;
        bunyikanAlarmApi(matikanAlarm);
      } else if (receivedMessage.equals("BukaRegulator\n")) {
        Serial.println(receivedMessage);
        Serial.println("BukaRegulator Log");
        MG995_Servo.attach(Servo_PWM);
        MG995_Servo.write(180);
        delay(1000);
        MG995_Servo.detach();
      }
      receivedMessage = "";
    }
  }
}

void bunyikanAlarmKebocoranGas(bool &matikanAlarm) {
  int frekuensiAwal = 500;     // Frekuensi awal alarm fire warning
  int frekuensiAkhir = 2500;   // Frekuensi akhir alarm fire warning
  int langkahFrekuensi = 100;  // Langkah perubahan frekuensi
  int durasiBunyi = 300;       // Durasi setiap bunyi dalam milidetik
  int jedaAntarBunyi = 200;    // Jeda antara setiap bunyi dalam milidetik

  int ledPin = 13;
  pinMode(ledPin, OUTPUT);

  while (!matikanAlarm) {  // false
                           // Baca data dari Serial2K
    if (Serial2.available() > 0) {
      String receivedMessage = Serial2.readStringUntil('\n');
      if (receivedMessage.equals("AlarmOff")) {
        Serial.println("AlarmOff Log");
        matikanAlarm = false;
        nyalakanAlarm = false;
        break;  // Keluar dari loop jika pesan diterima
      }
    }

    // Eksekusi bunyi
    for (int j = frekuensiAwal; j <= frekuensiAkhir; j += langkahFrekuensi) {
      tone(buzzerPin, j);          // Mulai bunyi dengan frekuensi j
      digitalWrite(ledPin, HIGH);  // Nyalakan LED selama bunyi
      delay(3);                    // Tahan bunyi dan LED selama 3 ms (sesuaikan dengan kebutuhan)
      digitalWrite(ledPin, LOW);   // Matikan LED
    }

    delay(durasiBunyi);  // Biarkan bunyi selama durasi yang ditentukan

    digitalWrite(ledPin, LOW);
    delay(jedaAntarBunyi);
  }
  noTone(buzzerPin);
  digitalWrite(ledPin, LOW);
}


void bunyikanAlarmApi(bool &matikanAlarm) {
  int frekuensiAwal = 500;    // Frekuensi awal alarm
  int frekuensiAkhir = 1500;  // Frekuensi akhir alarm
  int langkahFrekuensi = 20;  // Langkah perubahan frekuensi
  int durasiBunyi = 300;      // Durasi setiap bunyi dalam milidetik
  int jedaAntarBunyi = 200;   // Jeda antara setiap bunyi dalam milidetik

  int ledPin = 13;
  pinMode(ledPin, OUTPUT);

  while (!matikanAlarm) {  // false
    if (Serial2.available() > 0) {
      String receivedMessage = Serial2.readStringUntil('\n');
      if (receivedMessage.equals("AlarmOff")) {
        Serial.println("AlarmOff Log");
        matikanAlarm = false;
        nyalakanAlarm = false;
        break;
      }
    }

    for (int j = frekuensiAwal; j <= frekuensiAkhir; j += langkahFrekuensi) {
      tone(buzzerPin, j);          // Mulai bunyi dengan frekuensi j
      digitalWrite(ledPin, HIGH);  // Nyalakan LED
      delay(5);                    // Tahan bunyi dan LED selama 5 ms (sesuaikan dengan kebutuhan)
      digitalWrite(ledPin, LOW);   // Matikan LED
    }
    delay(durasiBunyi);  // Biarkan bunyi selama durasi yang ditentukan
    digitalWrite(ledPin, LOW);
    delay(jedaAntarBunyi);  // Tunggu jeda sebelum bunyi berikutnya
  }
  noTone(buzzerPin);
  digitalWrite(ledPin, LOW);
}


void bunyikanSirine(bool &matikanAlarm) {
  int frekuensiAwal = 300;    // Frekuensi awal sirine
  int frekuensiAkhir = 1000;  // Frekuensi akhir sirine
  int langkahFrekuensi = 10;  // Langkah perubahan frekuensi
  int durasiBunyi = 20;       // Durasi setiap bunyi dalam milidetik

  int ledPin = 13;
  pinMode(ledPin, OUTPUT);

  while (!matikanAlarm) {
    if (Serial2.available() > 0) {
      String receivedMessage = Serial2.readStringUntil('\n');
      if (receivedMessage.equals("AlarmEmergencyOff")) {
        Serial.println("AlarmEmergencyOff Log");
        matikanAlarm = false;
        nyalakanAlarm = false;
        break;  // Keluar dari loop jika pesan diterima
      }
    }

    // Loop untuk bunyi sirine
    for (int i = frekuensiAwal; i <= frekuensiAkhir && !matikanAlarm; i += langkahFrekuensi) {
      tone(buzzerPin, i);          // Mulai bunyi dengan frekuensi i
      digitalWrite(ledPin, HIGH);  // Nyalakan LED
      delay(durasiBunyi);          // Tahan bunyi selama durasi yang ditentukan
      digitalWrite(ledPin, LOW);   // Matikan LED
    }

    noTone(buzzerPin);  // Hentikan bunyi
    digitalWrite(ledPin, LOW);
  }
}