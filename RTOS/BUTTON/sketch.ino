#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definisikan ukuran layar OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Alamat I2C untuk kedua OLED
#define OLED_ADDRESS 0x3C

// Inisialisasi OLED pertama (I2C di core 0, menggunakan pin 14 (SDA) dan 13 (SCL))
Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Inisialisasi OLED kedua (I2C di core 1, menggunakan pin 20 (SDA) dan 19 (SCL))
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, -1);

// Definisikan pin tombol
const int buttonPin1 = 16;  // Tombol untuk core 0
const int buttonPin2 = 39;  // Tombol untuk core 1

// Variabel untuk membaca status tombol
volatile bool buttonState1 = false;  // Status tombol 1
volatile bool buttonState2 = false;  // Status tombol 2

// Fungsi untuk membaca status tombol di core 0
void core0Task(void *parameter) {
  while (true) {
    // Baca status tombol yang terhubung di pin 16 (untuk core 0)
    buttonState1 = digitalRead(buttonPin1);

    // Tampilkan pesan di OLED pertama (core 0)
    display1.clearDisplay();
    display1.setTextSize(1);
    display1.setTextColor(SSD1306_WHITE);
    display1.setCursor(0, 0);
    display1.print("Core 0 : RTOS");

    // Tampilkan status tombol
    display1.setCursor(0, 10);
    if (buttonState1) {
      display1.print("Tombol 1: Tidak diklik");
    } else {
      display1.print("Tombol 1: Diklik");
    }
    display1.display();
    
    vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay pendek untuk memberi kesempatan kepada core 1
  }
}

// Fungsi untuk membaca status tombol di core 1
void core1Task(void *parameter) {
  while (true) {
    // Baca status tombol yang terhubung di pin 39 (untuk core 1)
    buttonState2 = digitalRead(buttonPin2);

    // Tampilkan pesan di OLED kedua (core 1)
    display2.clearDisplay();
    display2.setTextSize(1);
    display2.setTextColor(SSD1306_WHITE);
    display2.setCursor(0, 0);
    display2.print("Core 1 : Regular");

    // Tampilkan status tombol
    display2.setCursor(0, 10);
    if (buttonState2) {
      display2.print("Tombol 2: Tidak diklik");
    } else {
      display2.print("Tombol 2: diklik");
    }
    display2.display();
    
    vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay pendek untuk memberi kesempatan kepada core 0
  }
}

void setup() {
  // Inisialisasi komunikasi I2C untuk OLED pertama (core 0) menggunakan pin 14 (SDA) dan 13 (SCL)
  Wire.begin(14, 13);  // SDA, SCL untuk OLED pertama di core 0

  // Inisialisasi komunikasi I2C untuk OLED kedua (core 1) menggunakan pin 20 (SDA) dan 19 (SCL)
  Wire1.begin(20, 19);  // SDA, SCL untuk OLED kedua di core 1

  // Inisialisasi tombol
  pinMode(buttonPin1, INPUT_PULLUP);  // Tombol 1 untuk core 0, aktifkan pull-up internal
  pinMode(buttonPin2, INPUT_PULLUP);  // Tombol 2 untuk core 1, aktifkan pull-up internal

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
