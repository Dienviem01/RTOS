#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Ukuran OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Alamat I2C OLED
#define OLED_ADDRESS 0x3C

// OLED 1 pakai Wire (I2C default, SDA 14, SCL 13)
Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// OLED 2 pakai Wire1 (I2C kedua, SDA 20, SCL 19)
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, -1);

// Pin potentiometer (sesuai JSON)
const int potPin1 = 16;  // pot1:SIG -> esp:16 (core 0)
const int potPin2 = 2;  // pot2:SIG -> esp:39 (core 1)

// Nilai baca potentiometer
volatile int potValue1 = 0;
volatile int potValue2 = 0;

// Task core 0: baca pot1 dan tampil di OLED1
void core0Task(void *parameter) {
  for (;;) {
    potValue1 = analogRead(potPin1);        // 0..4095
    int persen = map(potValue1, 0, 4095, 0, 100);

    display1.clearDisplay();
    display1.setTextSize(1);
    display1.setTextColor(SSD1306_WHITE);
    display1.setCursor(0, 0);
    display1.print("Core 0 : RTOS");

    display1.setCursor(0, 12);
    display1.print("Pot1 raw : ");
    display1.println(potValue1);

    display1.setCursor(0, 24);
    display1.print("Pot1 ");
    display1.print(persen);
    display1.println("%");

    // Bar sederhana
    int barWidth = map(persen, 0, 100, 0, SCREEN_WIDTH - 10);
    display1.drawRect(0, 40, SCREEN_WIDTH - 10, 10, SSD1306_WHITE);
    display1.fillRect(0, 40, barWidth, 10, SSD1306_WHITE);

    display1.display();

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// Task core 1: baca pot2 dan tampil di OLED2
void core1Task(void *parameter) {
  for (;;) {
    potValue2 = analogRead(potPin2);        // 0..4095
    int persen = map(potValue2, 0, 4095, 0, 100);

    display2.clearDisplay();
    display2.setTextSize(1);
    display2.setTextColor(SSD1306_WHITE);
    display2.setCursor(0, 0);
    display2.print("Core 1 : Regular");

    display2.setCursor(0, 12);
    display2.print("Pot2 raw : ");
    display2.println(potValue2);

    display2.setCursor(0, 24);
    display2.print("Pot2 ");
    display2.print(persen);
    display2.println("%");

    int barWidth = map(persen, 0, 100, 0, SCREEN_WIDTH - 10);
    display2.drawRect(0, 40, SCREEN_WIDTH - 10, 10, SSD1306_WHITE);
    display2.fillRect(0, 40, barWidth, 10, SSD1306_WHITE);

    display2.display();

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);

  // I2C OLED 1: SDA 14, SCL 13
  Wire.begin(14, 13);
  // I2C OLED 2: SDA 20, SCL 19
  Wire1.begin(20, 19);

  // Potentiometer (ADC), tidak wajib set pinMode, tapi aman pakai INPUT
  pinMode(potPin1, INPUT);
  pinMode(potPin2, INPUT);

  // Inisialisasi OLED 1
  if (!display1.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("Gagal init OLED1"));
    while (true) { vTaskDelay(1000 / portTICK_PERIOD_MS); }
  }

  // Inisialisasi OLED 2
  if (!display2.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("Gagal init OLED2"));
    while (true) { vTaskDelay(1000 / portTICK_PERIOD_MS); }
  }

  display1.clearDisplay();
  display1.display();
  display2.clearDisplay();
  display2.display();

  // Task untuk masing-masing core
  xTaskCreatePinnedToCore(core0Task, "Core0Task", 10000, NULL, 1, NULL, 0);  // core 0
  xTaskCreatePinnedToCore(core1Task, "Core1Task", 10000, NULL, 1, NULL, 1);  // core 1
}

void loop() {
  // Kosong, semua kerja di task
}
