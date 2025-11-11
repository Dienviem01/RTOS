#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definisikan ukuran layar OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Alamat I2C untuk kedua OLED
#define OLED_ADDRESS 0x3C

// Pin LED
const int ledPin1 = 16;  // LED merah di core 0
const int ledPin2 = 39;  // LED biru di core 1

// Inisialisasi OLED pertama (I2C di core 0, menggunakan pin 14 (SDA) dan 13 (SCL))
Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Inisialisasi OLED kedua (I2C di core 1, menggunakan pin 20 (SDA) dan 19 (SCL))
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, -1);

// Variabel untuk status LED
volatile bool ledState1 = false;  // Status LED merah (core 0)
volatile bool ledState2 = false;  // Status LED biru (core 1)

// Task yang dijalankan pada core 0
void core0Task(void *parameter) {
  pinMode(ledPin1, OUTPUT);  // Set pin LED merah sebagai output
  
  while (true) {
    // Toggle LED merah
    ledState1 = !ledState1;
    digitalWrite(ledPin1, ledState1 ? HIGH : LOW);  // Menyalakan atau mematikan LED merah

    // Tampilkan status LED di OLED pertama (core 0)
    display1.clearDisplay();
    display1.setTextSize(1);
    display1.setTextColor(SSD1306_WHITE);
    display1.setCursor(0, 0);
    display1.print("Core 0 : RTOS");

    // Tampilkan status LED
    display1.setCursor(0, 10);
    if (ledState1) {
      display1.print("LED Merah: Nyala");
    } else {
      display1.print("LED Merah: Mati");
    }
    display1.display();
    
    vTaskDelay(2000 / portTICK_PERIOD_MS);  // Delay 2 detik untuk blinking LED merah
  }
}

// Task yang dijalankan pada core 1
void core1Task(void *parameter) {
  pinMode(ledPin2, OUTPUT);  // Set pin LED biru sebagai output

  while (true) {
    // Toggle LED biru
    ledState2 = !ledState2;
    digitalWrite(ledPin2, ledState2 ? HIGH : LOW);  // Menyalakan atau mematikan LED biru

    // Tampilkan status LED di OLED kedua (core 1)
    display2.clearDisplay();
    display2.setTextSize(1);
    display2.setTextColor(SSD1306_WHITE);
    display2.setCursor(0, 0);
    display2.print("Core 1 : Regular");

    // Tampilkan status LED
    display2.setCursor(0, 10);
    if (ledState2) {
      display2.print("LED Biru: Nyala");
    } else {
      display2.print("LED Biru: Mati");
    }
    display2.display();
    
    vTaskDelay(2000 / portTICK_PERIOD_MS);  // Delay 2 detik untuk blinking LED biru
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
