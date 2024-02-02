#include <SoftwareSerial.h>
#include <Servo.h>

#define Servo_PWM 6
Servo MG995_Servo;

SoftwareSerial Serial2(2, 3);  // 2 tx0, 3 rx1
int buzzerPin = 5;
int ledPin = 13;

String data;

bool matikanAlarm = false;
bool nyalakanAlarm = false;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  MG995_Servo.attach(Servo_PWM);
  MG995_Servo.write(0);
  delay(1000);
}

void loop() {
  while (Serial2.available() > 0) {
    delay(10);
    char d = Serial2.read();
    data += d;
  }

  if (data.length() > 0) {
    // Serial.println(data);

    if (data.equals("Gas")) {
      Serial.println("Gas");
      MG995_Servo.write(180);
      delay(1000);
      nyalakanAlarm = true;
      bunyikanAlarmKebocoranGas(matikanAlarm);
    } else if (data.equals("Api")) {
      Serial.println("Api");
      nyalakanAlarm = true;
      bunyikanAlarmApi(matikanAlarm);
      // bunyikanAlarmKebocoranGas(matikanAlarm);

    } else if (data.equals("Asap")) {
      Serial.println("Asap");
      nyalakanAlarm = true;
      bunyikanSirine(matikanAlarm);
    } else if (data.equals("Emergency")) {
      Serial.println("Emergency");
      nyalakanAlarm = true;
      bunyikanSirine(matikanAlarm);
    }

    data = "";
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
    // Baca data dari Serial2
    while (Serial2.available() > 0) {
      char d = Serial2.read();
      data += d;
    }

    // Proses data jika ada
    if (data.length() > 0) {
      if (data.equals("AlarmOff")) {
        Serial.println("AlarmOff");
        matikanAlarm = false;
        nyalakanAlarm = false;
        break;  // Keluar dari loop jika "AlarmOff" diterima
      }
      data = "";
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

  while (!matikanAlarm) {
    while (Serial2.available() > 0) {
      char d = Serial2.read();
      data += d;
    }

    if (data.length() > 0) {
      if (data.equals("AlarmOff")) {
        Serial.println("AlarmOff");
        matikanAlarm = false;
        nyalakanAlarm = false;
        break;  // Keluar dari loop jika "AlarmOff" diterima
      }
      data = "";
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
    // Tambahkan loop untuk membaca data dari Serial2
    while (Serial2.available() > 0) {
      char d = Serial2.read();
      data += d;
    }

    if (data.length() > 0) {
      if (data.equals("AlarmOff")) {
        Serial.println("AlarmOff");
        matikanAlarm = false;
        nyalakanAlarm = false;
        break;  // Keluar dari loop jika "AlarmOff" diterima
      }
      data = "";
    }

    // Loop untuk bunyi sirine
    for (int i = frekuensiAwal; i <= frekuensiAkhir && !matikanAlarm; i += langkahFrekuensi) {
      tone(buzzerPin, i);          // Mulai bunyi dengan frekuensi i
      digitalWrite(ledPin, HIGH);  // Nyalakan LED
      delay(durasiBunyi);           // Tahan bunyi selama durasi yang ditentukan
      digitalWrite(ledPin, LOW);   // Matikan LED
    }

    noTone(buzzerPin);    // Hentikan bunyi
    digitalWrite(ledPin, LOW);
  }
}
