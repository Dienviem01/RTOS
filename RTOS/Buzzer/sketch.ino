#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definisikan ukuran layar OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Alamat I2C untuk kedua OLED
#define OLED_ADDRESS 0x3C

// Pin untuk buzzer
const int buzzerPin1 = 16;  // Buzzer 1 di core 0
const int buzzerPin2 = 39;  // Buzzer 2 di core 1

// Status buzzer
volatile bool buzzerState1 = false;  // Status buzzer 1
volatile bool buzzerState2 = false;  // Status buzzer 2

// Inisialisasi OLED pertama (I2C di core 0, menggunakan pin 14 (SDA) dan 13 (SCL))
Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Inisialisasi OLED kedua (I2C di core 1, menggunakan pin 20 (SDA) dan 19 (SCL))
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, -1);

// Task yang dijalankan pada core 0
void core0Task(void *parameter) {
  pinMode(buzzerPin1, OUTPUT);  // Set pin buzzer 1 sebagai output

  while (true) {
    // Toggle buzzer 1
    buzzerState1 = !buzzerState1;
    digitalWrite(buzzerPin1, buzzerState1 ? HIGH : LOW);  // Menyalakan atau mematikan buzzer 1

    // Tampilkan status buzzer 1 di OLED pertama (core 0)
    display1.clearDisplay();
    display1.setTextSize(1);
    display1.setTextColor(SSD1306_WHITE);
    display1.setCursor(0, 0);
    display1.print("Core 0 : RTOS");

    // Tampilkan status buzzer 1
    display1.setCursor(0, 10);
    if (buzzerState1) {
      display1.print("Buzzer 1: Bunyi");
    } else {
      display1.print("Buzzer 1: Diam");
    }
    display1.display();
    
    // Tampilkan status buzzer 2 di OLED kedua (core 1)
    display2.clearDisplay();
    display2.setTextSize(1);
    display2.setTextColor(SSD1306_WHITE);
    display2.setCursor(0, 0);
    display2.print("Core 1 : Regular");

    if (buzzerState2) {
      display2.print("Buzzer 2: Bunyi");
    } else {
      display2.print("Buzzer 2: Diam");
    }
    display2.display();

    vTaskDelay(2000 / portTICK_PERIOD_MS);  // Delay 2 detik untuk update status
  }
}

// Task yang dijalankan pada core 1
void core1Task(void *parameter) {
  pinMode(buzzerPin2, OUTPUT);  // Set pin buzzer 2 sebagai output

  while (true) {
    // Toggle buzzer 2
    buzzerState2 = !buzzerState2;
    digitalWrite(buzzerPin2, buzzerState2 ? HIGH : LOW);  // Menyalakan atau mematikan buzzer 2

    // Tampilkan status buzzer 1 di OLED pertama (core 0)
    display1.clearDisplay();
    display1.setTextSize(1);
    display1.setTextColor(SSD1306_WHITE);
    display1.setCursor(0, 0);
    display1.print("Core 0 : RTOS");

    if (buzzerState1) {
      display1.print("Buzzer 1: Bunyi");
    } else {
      display1.print("Buzzer 1: Diam");
    }
    display1.display();

    // Tampilkan status buzzer 2 di OLED kedua (core 1)
    display2.clearDisplay();
    display2.setTextSize(1);
    display2.setTextColor(SSD1306_WHITE);
    display2.setCursor(0, 0);
    display2.print("Core 1 : Regular");

    if (buzzerState2) {
      display2.print("Buzzer 2: Bunyi");
    } else {
      display2.print("Buzzer 2: Diam");
    }
    display2.display();

    vTaskDelay(2000 / portTICK_PERIOD_MS);  // Delay 2 detik untuk update status
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
