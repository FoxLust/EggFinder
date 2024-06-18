// Library apa saja yang digunakan
#include <NewPing.h>
#include <BluetoothSerial.h>

// Melakukan link agar library mudah di panggil
BluetoothSerial SerialBT;

// Pin yang digunakan untuk menghubungkan RaspberryPi
#define RXD2 16
#define TXD2 17

// Pengaturan untuk motor driver
#define KecepatanNormal 220
#define EN1 19
#define EN2 18
#define IN1 33
#define IN2 32
#define IN3 22
#define IN4 23

// Relay yang jadi motor driver
#define IN5 5
#define IN6 4

// Pin untuk pengaturan mode
#define MODE1 13

// Pin untuk menyalakan lampu
#define LAMPU1 25
#define LAMPU2 26

// Pengaturan untuk sensor ultrasonik
#define TRIG1 14
#define ECHO1 35
#define TRIG2 27
#define ECHO2 34
#define MAX_DISTANCE 200
NewPing sonar1(TRIG1, ECHO1, MAX_DISTANCE);
NewPing sonar2(TRIG2, ECHO2, MAX_DISTANCE);

void setup()
{
  // Mengaktifkan Serial untuk berkomunikasi antara perangkat
  SerialBT.begin("QuackBot");
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  // pengaturan mode
  pinMode(MODE1, INPUT_PULLUP);

  // Penyesuaian pin untuk tangan dan membuat relay tangan menjadi high karena relay low trigger
  pinMode(IN5, OUTPUT);
  pinMode(IN6, OUTPUT);
  digitalWrite(IN5, HIGH);
  digitalWrite(IN6, HIGH);

  // Penyesuaian pin untuk lampu dan membuat relay lampu menjadi high karena relay low trigger
  pinMode(LAMPU1, OUTPUT);
  pinMode(LAMPU2, OUTPUT);
  digitalWrite(LAMPU1, HIGH);
  digitalWrite(LAMPU2, HIGH);

  // penyesuaian pin motor driver sesuai kebutuhan
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // penyesuaian pin sensor ultrasonik sesuai kebutuhan
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
}

// membuat void fungsi agar mempermudah pemanggilan fungsi menggunakan satu perintah

// fungsi untuk menyalakan lampu
void lampu(int StatusLamp)
{
  if (StatusLamp == 1) {
    Serial.println("menyalakan lampu dekat");
    digitalWrite(LAMPU1, LOW);
    digitalWrite(LAMPU2, HIGH);
  } else if (StatusLamp == 2) {
    Serial.println("menyalakan lampu jauh");
    digitalWrite(LAMPU1, HIGH);
    digitalWrite(LAMPU2, LOW);
  } else if (StatusLamp == 3) {
    Serial.println("menyalakan jauhdekat");
    digitalWrite(LAMPU1, LOW);
    digitalWrite(LAMPU2, LOW);
  } else {
    Serial.println("mematikan lampu");
    digitalWrite(LAMPU1, HIGH);
    digitalWrite(LAMPU2, HIGH);
  }
}

// fungsi untuk angkat telur ke dalam
void angkat()
{
  Serial.println("angkat telur");
  digitalWrite(IN5, LOW);
  digitalWrite(IN6, HIGH);
  delay(600);
  digitalWrite(IN5, HIGH);
  digitalWrite(IN6, HIGH);
  delay(1200);
  digitalWrite(IN5, HIGH);
  digitalWrite(IN6, LOW);
  delay(300);
  digitalWrite(IN5, HIGH);
  digitalWrite(IN6, HIGH);
}

// fungsi untuk memutar roda ke depan
void maju(int kecepatan)
{
  Serial.println("maju");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(EN1, kecepatan);
  analogWrite(EN2, kecepatan);
}

// fungsi untuk memutar roda ke belakang
void mundur(int kecepatan)
{
  Serial.println("mundur");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(EN1, kecepatan);
  analogWrite(EN2, kecepatan);
}

// fungsi untuk menghentikan perputaran roda
void berhenti()
{
  Serial.println("berhenti");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(EN1, 0);
  analogWrite(EN2, 0);
}

// fungsi untuk memutar roda ke kanan
void kanan(int kecepatan)
{
  Serial.println("belok kanan");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(EN1, kecepatan);
  analogWrite(EN2, kecepatan);
}

// fungsi untuk memutar roda ke kiri
void kiri(int kecepatan)
{
  Serial.println("belok kiri");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(EN1, kecepatan);
  analogWrite(EN2, kecepatan);
}

// fungsi untuk mengukur jarak dari kedua sensor ultrasonik
void jarak()
{
  delay(50);
  unsigned int distanceCm1 = sonar1.ping_cm();
  Serial.print("jarak1 (cm): ");
  Serial.println(distanceCm1);

  delay(50);
  unsigned int distanceCm2 = sonar2.ping_cm();
  Serial.print("jarak2 (cm): ");
  Serial.println(distanceCm2);

  delay(100);
}

void loop()
{
  // Memanggil fungsi untuk membaca jarak alat dengan halangan seperti tembok
  //jarak();

  // Apabila Mode diaktifkan, hanya menerima perintah manual melalui bluetooth
  if (MODE1 == 1)
  {
    Serial.print("Mode Bluetooth Only");
    // Ketika Perangkat bluetooth terhubung, Baca perintah yang dikirim
    if (SerialBT.available()) {
      // Membaca perintah apabila sesuai panggil fungsi
      char command = SerialBT.read();
      switch (command) {
        case 'X':
          angkat();
          break;
        case 'W':
          lampu(1);
          break;
        case 'w':
          lampu(0);
          break;
        case 'U':
          lampu(2);
          break;
        case 'u':
          lampu(0);
          break;
        case 'V':
          lampu(3);
          break;
        case 'v':
          lampu(0);
          break;
        case 's':
          maju(80);
          break;
        case 'S':
          berhenti();
          break;
        case 'F':
          maju(KecepatanNormal);
          break;
        case 'B':
          mundur(KecepatanNormal);
          break;
        case 'R':
          kanan(KecepatanNormal);
          break;
        case 'L':
          kiri(KecepatanNormal);
          break;
      }
    }
  }
  // Apabila Mode normal, alat menerima perintah dari RaspberryPi dan Bluetooth
  else
  {
    // Baca peintah yang dikirim melalui kabel Serial yang terhubung dengan RaspberryPi
    if (Serial2.available()) {
      // Membaca perintah apabila sesuai panggil fungsi
      char command = Serial2.read();
      switch (command) { 
          case 'X':
            angkat();
            break;
          case 'W':
            lampu(1);
            break;
          case 'w':
            lampu(0);
            break;
          case 'U':
            lampu(2);
            break;
          case 'u':
            lampu(0);
            break;
          case 'V':
            lampu(3);
            break;
          case 'v':
            lampu(0);
            break;
          case 's':
            maju(80);
            break;
          case 'S':
            berhenti();
            break;
          case 'F':
            maju(KecepatanNormal);
            break;
          case 'B':
            mundur(KecepatanNormal);
            break;
          case 'R':
            kanan(KecepatanNormal);
            break;
          case 'L':
            kiri(KecepatanNormal);
            break;
      }
    }
    // Ketika ESP32 sudah tidak menerima perintah melalui Serial
    // ESP32 tetap bisa di hubungkan melalui Bluetooth untuk mengarahkan
    // Alat ke tepi tanpa perlu di angkat

    // Ketika Perangkat bluetooth terhubung, Baca perintah yang dikirim
    if (SerialBT.available()) {
      // Membaca perintah apabila sesuai panggil fungsi
      char command = SerialBT.read();
      switch (command) {
          case 'X':
            angkat();
            break;
          case 'W':
            lampu(1);
            break;
          case 'w':
            lampu(0);
            break;
          case 'U':
            lampu(2);
            break;
          case 'u':
            lampu(0);
            break;
          case 'V':
            lampu(3);
            break;
          case 'v':
            lampu(0);
            break;
          case 's':
            maju(80);
            break;
          case 'S':
            berhenti();
            break;
          case 'F':
            maju(KecepatanNormal);
            break;
          case 'B':
            mundur(KecepatanNormal);
            break;
          case 'R':
            kanan(KecepatanNormal);
            break;
          case 'L':
            kiri(KecepatanNormal);
            break;
      }
    }
  }

  // Apabila jarak alat dari sensor kiri/kanan di bawah 10cm, berhenti dan menjauh
  // if (distanceCm1 <= 10) {
  //   berhenti();
  //   delay(500);
  //   kiri(255);
  //   delay(1000);
  //   maju(255);
  // }
  // if (distanceCm2 <= 10) {
  //   berhenti();
  //   delay(500);
  //   kanan(255);
  //   delay(1000);
  //   maju(255);
  // }
}
