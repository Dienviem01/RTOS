#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Stepper.h>

// ----------------- KONFIGURASI STEPPER -----------------

const int stepsPerRevolution = 200;

// Stepper 1 di core 0, pin 15-18 (sesuai diagram JSON)
#define STEPPER1_IN1 15  // B-
#define STEPPER1_IN2 16  // B+
#define STEPPER1_IN3 17  // A+
#define STEPPER1_IN4 18  // A-

// Stepper 2 di core 1, pin 37-40 (sesuai diagram JSON)
#define STEPPER2_IN1 37  // B-
#define STEPPER2_IN2 38  // B+
#define STEPPER2_IN3 39  // A+
#define STEPPER2_IN4 40  // A-

Stepper stepper1(stepsPerRevolution, STEPPER1_IN1, STEPPER1_IN2, STEPPER1_IN3, STEPPER1_IN4);
Stepper stepper2(stepsPerRevolution, STEPPER2_IN1, STEPPER2_IN2, STEPPER2_IN3, STEPPER2_IN4);

// ----------------- KONFIGURASI OLED -----------------

TwoWire I2C_oled1 = TwoWire(0);  // OLED1: SCL 13, SDA 14
TwoWire I2C_oled2 = TwoWire(1);  // OLED2: SCL 19, SDA 20

Adafruit_SSD1306 display1(128, 64, &I2C_oled1, -1);
Adafruit_SSD1306 display2(128, 64, &I2C_oled2, -1);

// ----------------- TAMPILAN OLED -----------------

void initDisplays() {
  // Hitung otomatis x-y untuk masing-masing motor
  int s1MinPin = min(min(STEPPER1_IN1, STEPPER1_IN2),
                     min(STEPPER1_IN3, STEPPER1_IN4));
  int s1MaxPin = max(max(STEPPER1_IN1, STEPPER1_IN2),
                     max(STEPPER1_IN3, STEPPER1_IN4));

  int s2MinPin = min(min(STEPPER2_IN1, STEPPER2_IN2),
                     min(STEPPER2_IN3, STEPPER2_IN4));
  int s2MaxPin = max(max(STEPPER2_IN1, STEPPER2_IN2),
                     max(STEPPER2_IN3, STEPPER2_IN4));

  char line1[32];
  char line2[32];

  // OLED 1: "motor pin x-y running core 0"
  display1.clearDisplay();
  display1.setTextSize(1);
  display1.setTextColor(SSD1306_WHITE);
  display1.setCursor(0, 0);
  snprintf(line1, sizeof(line1), "Motor pin %d-%d", s1MinPin, s1MaxPin);
  display1.println(line1);
  display1.println("running core 0");
  display1.display();

  // OLED 2: "motor pin x-y running di core 1"
  display2.clearDisplay();
  display2.setTextSize(1);
  display2.setTextColor(SSD1306_WHITE);
  display2.setCursor(0, 0);
  snprintf(line2, sizeof(line2), "Motor pin %d-%d", s2MinPin, s2MaxPin);
  display2.println(line2);
  display2.println("running di core 1");
  display2.display();
}

// ----------------- TASK RTOS -----------------

// Core 0: motor stepper1, arah "positif" (tanpa nilai step minus)
void core0Task(void *pvParameters) {
  for (;;) {
    Serial.println("Core 0: clockwise");
    stepper1.step(stepsPerRevolution);   // putar searah jarum jam
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// Core 1: motor stepper2, arah berlawanan
void core1Task(void *pvParameters) {
  for (;;) {
    Serial.println("Core 1: counterclockwise");
    stepper2.step(-stepsPerRevolution);  // putar berlawanan (nilai minus di sini)
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// ----------------- SETUP & LOOP -----------------

void setup() {
  Serial.begin(115200);

  // Set kecepatan motor
  stepper1.setSpeed(60);  // rpm
  stepper2.setSpeed(60);  // rpm

  // Inisialisasi dua bus I2C
  // OLED1: SDA 14, SCL 13
  I2C_oled1.begin(14, 13);
  // OLED2: SDA 20, SCL 19
  I2C_oled2.begin(20, 19);

  // Inisialisasi dua OLED (alamat sama 0x3C di bus berbeda)
  if (!display1.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED1 gagal init");
  }
  if (!display2.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED2 gagal init");
  }

  initDisplays();

  // Buat dua task RTOS, dipin ke core yang berbeda
  xTaskCreatePinnedToCore(core1Task, "Core1Task", 4096, NULL, 1, NULL, 0);

  xTaskCreatePinnedToCore(core1Task, "Core1Task", 4096, NULL, 1, NULL, 1);

void loop() {
  // Kosong, semua kerja di task RTOS
}
