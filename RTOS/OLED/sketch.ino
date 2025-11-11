#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definisikan ukuran layar OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Alamat I2C untuk OLED
#define OLED_ADDRESS 0x3C

// Inisialisasi OLED pertama (I2C di core 0, menggunakan pin 14 (SDA) dan 13 (SCL))
Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Inisialisasi OLED kedua (I2C di core 1, menggunakan pin 20 (SDA) dan 19 (SCL))
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, -1);

// Flag untuk mengatur giliran core
volatile bool core0Turn = true;

// Task yang dijalankan pada core 0
void core0Task(void *parameter) {
  while (true) {
    if (core0Turn) {
      // Dapatkan ID core yang sedang digunakan
      int coreID = xPortGetCoreID();  // Ini akan mengembalikan 0 untuk core 0 atau 1 untuk core 1

      // Tampilkan pesan "Core X : RTOS" di OLED pertama (core 0)
      display1.clearDisplay();
      display1.setTextSize(1);
      display1.setTextColor(SSD1306_WHITE);
      display1.setCursor(0, 0);
      display1.print("Running on Core ");
      display1.print(coreID);  // Menampilkan ID core
      display1.setCursor(0, 10);  // Pindahkan ke baris berikutnya
      display1.print("Core 0 : RTOS");
      display1.display();
      
      // Beri waktu 5 detik agar pesan tetap tampil
      vTaskDelay(5000 / portTICK_PERIOD_MS);  // Tunda 5 detik

      // Ganti giliran ke core 1
      core0Turn = false;
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay pendek untuk memberi kesempatan kepada core 1
  }
}

// Task yang dijalankan pada core 1
void core1Task(void *parameter) {
  while (true) {
    if (!core0Turn) {
      // Dapatkan ID core yang sedang digunakan
      int coreID = xPortGetCoreID();  // Ini akan mengembalikan 0 untuk core 0 atau 1 untuk core 1

      // Tampilkan pesan "Core X : Regular" di OLED kedua (core 1)
      display2.clearDisplay();
      display2.setTextSize(1);
      display2.setTextColor(SSD1306_WHITE);
      display2.setCursor(0, 0);
      display2.print("Running on Core ");
      display2.print(coreID);  // Menampilkan ID core
      display2.setCursor(0, 10);  // Pindahkan ke baris berikutnya
      display2.print("Core 1 : Regular");
      display2.display();
      
      // Beri waktu 5 detik agar pesan tetap tampil
      vTaskDelay(5000 / portTICK_PERIOD_MS);  // Tunda 5 detik

      // Ganti giliran ke core 0
      core0Turn = true;
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay pendek untuk memberi kesempatan kepada core 0
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
