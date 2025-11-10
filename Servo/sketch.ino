#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

// --------- PIN SERVO (sesuai diagram) ---------
#define SERVO1_PIN 17   // servo1:PWM -> esp:17 (core 0)
#define SERVO2_PIN 18   // servo2:PWM -> esp:18 (core 1)

Servo servo1;
Servo servo2;

// --------- OLED & I2C ---------
TwoWire I2C_oled1 = TwoWire(0);  // OLED1: SCL 13, SDA 14
TwoWire I2C_oled2 = TwoWire(1);  // OLED2: SCL 19, SDA 20

Adafruit_SSD1306 display1(128, 64, &I2C_oled1, -1);
Adafruit_SSD1306 display2(128, 64, &I2C_oled2, -1);

// --------- TAMPILAN OLED ---------
void initDisplays() {
  char line1[32];
  char line2[32];

  display1.clearDisplay();
  display1.setTextSize(1);
  display1.setTextColor(SSD1306_WHITE);
  display1.setCursor(0, 0);
  snprintf(line1, sizeof(line1), "Servo pin %d", SERVO1_PIN);
  display1.println(line1);
  display1.println("running core 0");
  display1.display();

  display2.clearDisplay();
  display2.setTextSize(1);
  display2.setTextColor(SSD1306_WHITE);
  display2.setCursor(0, 0);
  snprintf(line2, sizeof(line2), "Servo pin %d", SERVO2_PIN);
  display2.println(line2);
  display2.println("running di core 1");
  display2.display();
}

// --------- TASK CORE 0 (servo1 di pin 18) ---------
void core0Task(void *pvParameters) {
  int angle = 0;
  bool naik = true;

  for (;;) {
    servo1.write(angle);

    if (naik) {
      angle += 5;
      if (angle >= 180) {
        angle = 180;
        naik = false;
      }
    } else {
      angle -= 5;
      if (angle <= 0) {
        angle = 0;
        naik = true;
      }
    }

    vTaskDelay(20 / portTICK_PERIOD_MS);  // kecepatan sweep
  }
}

// --------- TASK CORE 1 (servo2 di pin 46) ---------
void core1Task(void *pvParameters) {
  int angle = 180;
  bool turun = true;

  for (;;) {
    servo2.write(angle);

    if (turun) {
      angle -= 5;
      if (angle <= 0) {
        angle = 0;
        turun = false;
      }
    } else {
      angle += 5;
      if (angle >= 180) {
        angle = 180;
        turun = true;
      }
    }

    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

// --------- SETUP & LOOP ---------
void setup() {
  Serial.begin(115200);

  // Attach servo ke pin, pakai range 500–2400us seperti contoh kamu
  servo1.attach(SERVO1_PIN, 500, 2400);
  servo2.attach(SERVO2_PIN, 500, 2400);

  // I2C OLED1: SDA 14, SCL 13
  I2C_oled1.begin(14, 13);
  // I2C OLED2: SDA 20, SCL 19
  I2C_oled2.begin(20, 19);

  // Init OLED
  if (!display1.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED1 gagal init");
  }
  if (!display2.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED2 gagal init");
  }

  initDisplays();

  // RTOS task per core (1 baris seperti maumu)
  xTaskCreatePinnedToCore(core0Task, "Core0Task", 4096, NULL, 1, NULL, 0);  // core 0
  xTaskCreatePinnedToCore(core1Task, "Core1Task", 4096, NULL, 1, NULL, 1);  // core 1
}

void loop() {
  // Kosong, semua kerja di task RTOS
}
