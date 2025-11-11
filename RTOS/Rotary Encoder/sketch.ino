#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definisikan ukuran layar OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Alamat I2C untuk kedua OLED
#define OLED_ADDRESS 0x3C

// Pin untuk rotary encoder
const int encoderPin1CLK = 8;  // Pin CLK untuk encoder 1 (core 0)
const int encoderPin1DT = 3;   // Pin DT untuk encoder 1 (core 0)
const int encoderPin1SW = 46;  // Pin SW untuk encoder 1 (core 0)

const int encoderPin2CLK = 4;  // Pin CLK untuk encoder 2 (core 1)
const int encoderPin2DT = 5;   // Pin DT untuk encoder 2 (core 1)
const int encoderPin2SW = 6;   // Pin SW untuk encoder 2 (core 1)

// Status nilai encoder
volatile int encoderValue1 = 0;  // Nilai encoder 1 (core 0)
volatile int encoderValue2 = 0;  // Nilai encoder 2 (core 1)

// Inisialisasi OLED pertama (I2C di core 0, menggunakan pin 14 (SDA) dan 13 (SCL))
Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Inisialisasi OLED kedua (I2C di core 1, menggunakan pin 20 (SDA) dan 19 (SCL))
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, -1);

// Fungsi untuk membaca rotary encoder 1 (core 0)
void core0Task(void *parameter) {
  pinMode(encoderPin1CLK, INPUT);  // Set pin CLK untuk encoder 1 sebagai input
  pinMode(encoderPin1DT, INPUT);   // Set pin DT untuk encoder 1 sebagai input
  pinMode(encoderPin1SW, INPUT_PULLUP);  // Set pin SW untuk encoder 1 dengan pull-up internal

  int lastEncoded1 = 0;  // Variabel untuk menyimpan status sebelumnya dari CLK

  while (true) {
    int MSB = digitalRead(encoderPin1CLK);  // Baca status CLK
    int LSB = digitalRead(encoderPin1DT);   // Baca status DT

    // Deteksi perubahan arah encoder
    int encoded = (MSB << 1) | LSB;  // Gabungkan MSB dan LSB
    int sum = (lastEncoded1 << 2) | encoded;  // Gabungkan dengan status sebelumnya
    if (sum == 0b1101 || sum == 0b0110) {
      encoderValue1++;  // Encoder diputar searah
    }
    if (sum == 0b1110 || sum == 0b0001) {
      encoderValue1--;  // Encoder diputar berlawanan arah
    }
    lastEncoded1 = encoded;  // Simpan status terakhir

    // Tampilkan nilai encoder 1 di OLED pertama (core 0)
    display1.clearDisplay();
    display1.setTextSize(1);
    display1.setTextColor(SSD1306_WHITE);
    display1.setCursor(0, 0);
    display1.print("Core 0 : RTOS");

    display1.setCursor(0, 10);
    display1.print("Encoder 1: ");
    display1.print(encoderValue1);
    display1.display();

    vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay pendek untuk membaca encoder
  }
}

// Fungsi untuk membaca rotary encoder 2 (core 1)
void core1Task(void *parameter) {
  pinMode(encoderPin2CLK, INPUT);  // Set pin CLK untuk encoder 2 sebagai input
  pinMode(encoderPin2DT, INPUT);   // Set pin DT untuk encoder 2 sebagai input
  pinMode(encoderPin2SW, INPUT_PULLUP);  // Set pin SW untuk encoder 2 dengan pull-up internal

  int lastEncoded2 = 0;  // Variabel untuk menyimpan status sebelumnya dari CLK

  while (true) {
    int MSB = digitalRead(encoderPin2CLK);  // Baca status CLK
    int LSB = digitalRead(encoderPin2DT);   // Baca status DT

    // Deteksi perubahan arah encoder
    int encoded = (MSB << 1) | LSB;  // Gabungkan MSB dan LSB
    int sum = (lastEncoded2 << 2) | encoded;  // Gabungkan dengan status sebelumnya
    if (sum == 0b1101 || sum == 0b0110) {
      encoderValue2++;  // Encoder diputar searah
    }
    if (sum == 0b1110 || sum == 0b0001) {
      encoderValue2--;  // Encoder diputar berlawanan arah
    }
    lastEncoded2 = encoded;  // Simpan status terakhir

    // Tampilkan nilai encoder 2 di OLED kedua (core 1)
    display2.clearDisplay();
    display2.setTextSize(1);
    display2.setTextColor(SSD1306_WHITE);
    display2.setCursor(0, 0);
    display2.print("Core 1 : Regular");

    display2.setCursor(0, 10);
    display2.print("Encoder 2: ");
    display2.print(encoderValue2);
    display2.display();

    vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay pendek untuk membaca encoder
  }
}

void setup() {
  // Inisialisasi komunikasi I2C untuk OLED pertama (core 0) menggunakan pin 14 (SDA) dan 13 (SCL)
  Wire.begin(14, 13);  // SDA, SCL untuk OLED pertama di core 0

  // Inisialisasi komunikasi I2C untuk OLED kedua (core 1) menggunakan pin 20 (SDA) dan 19 (SCL)
  Wire1.begin(20, 19);  // SDA, SCL untuk OLED kedua di core 1

  // Inisialisasi OLED pertama (di core 0)
  if (!display1.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("Gagal menginisialisasi OLED pertama!"));
    while (true);  // Jika OLED pertama gagal inisialisasi, hentikan program
  }

  // Inisialisasi OLED kedua (di core 1)
  if (!display2.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("Gagal menginisialisasi OLED kedua!"));
    while (true);  // Jika OLED kedua gagal inisialisasi, hentikan program
  }

  display1.display();
  display2.display();
  delay(2000);  // Tampilkan logo awal OLED selama 2 detik

  // Buat task untuk core 0 dan core 1
  xTaskCreatePinnedToCore(core0Task, "Core0Task", 10000, NULL, 1, NULL, 0);  // Jalankan di core 0
  xTaskCreatePinnedToCore(core1Task, "Core1Task", 10000, NULL, 1, NULL, 1);  // Jalankan di core 1
}

void loop() {
  // Tidak ada kode di loop() karena tugas dijalankan di core 0 dan core 1
}
