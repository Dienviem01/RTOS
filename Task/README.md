# 🧠 Praktikum FreeRTOS Multi-Task dengan ESP32-S3

Repository ini berisi **program multitasking menggunakan FreeRTOS** pada **ESP32-S3**.
Setiap komponen seperti **OLED**, **Servo**, **Stepper**, **Potensiometer**, **Rotary Encoder**, **Button**, **LED**, dan **Buzzer** dijalankan sebagai *task terpisah* di core 0 dan core 1.

Tujuannya adalah memahami **cara kerja multitasking dan pembagian tugas antar core** pada ESP32 menggunakan **FreeRTOS**.

---

## ⚙️ Konsep Dasar

ESP32-S3 memiliki **dua core prosesor** yang dapat bekerja secara paralel:

| Core       | Fungsi Utama                                                      | Contoh Penggunaan                        |
| ---------- | ----------------------------------------------------------------- | ---------------------------------------- |
| **Core 0** | Menangani sistem dan proses latar belakang seperti WiFi/Bluetooth | OLED, Potensiometer, Button, LED, Buzzer |
| **Core 1** | Menjalankan program utama pengguna                                | Servo, Encoder, Stepper                  |

Dengan **FreeRTOS**, setiap komponen dapat dijalankan di task terpisah menggunakan fungsi `xTaskCreatePinnedToCore()` agar tidak saling mengganggu.

---

## 🧩 Struktur Program

### 🔸 Pembuatan Task

Setiap *task* memiliki struktur dasar seperti berikut:

```cpp
void TaskName(void *pvParameters) {
  for (;;) {
    // Kode yang dijalankan terus menerus
    vTaskDelay(pdMS_TO_TICKS(100));  // jeda tanpa menghentikan core
  }
}
```

Task dibuat dengan fungsi:

```cpp
xTaskCreatePinnedToCore(
  TaskName,     // Fungsi task
  "TaskLabel",  // Nama task (untuk debugging)
  2048,         // Ukuran stack
  NULL,         // Parameter
  1,            // Prioritas task
  &taskHandle,  // Handle task
  coreID        // Core tujuan: 0 atau 1
);
```

---

## 🧠 Daftar Task dalam Program

| No | Nama Task       | Fungsi                                        | Komponen                    | Core |
| -- | --------------- | --------------------------------------------- | --------------------------- | ---- |
| 1  | **OledTask**    | Menampilkan teks di OLED                      | Adafruit SSD1306            | 0    |
| 2  | **ServoTask**   | Menggerakkan servo maju–mundur 0°–180°        | Servo (GPIO 17)             | 1    |
| 3  | **PotTask**     | Membaca nilai potensiometer dan menampilkan % | Potensiometer (GPIO 16)     | 0    |
| 4  | **EncoderTask** | Membaca pergerakan rotary encoder             | Rotary Encoder (GPIO 4,5,6) | 1    |
| 5  | **ButtonTask**  | Mendeteksi klik tombol                        | Push Button (GPIO 21)       | 0    |
| 6  | **LedTask**     | Menyalakan dan mematikan LED bergantian       | LED (GPIO 2)                | 0    |
| 7  | **BuzzerTask**  | Mengaktifkan buzzer secara periodik           | Buzzer (GPIO 1)             | 0    |
| 8  | **StepperTask** | Menggerakkan motor stepper maju–mundur        | Stepper (GPIO 37–40)        | 1    |

---

## 🖼️ Komponen yang Digunakan

| Komponen       | GPIO                    | Keterangan            |
| -------------- | ----------------------- | --------------------- |
| OLED SSD1306   | SDA = 14, SCL = 13      | Tampilan teks         |
| Servo          | 17                      | Gerak 0–180° otomatis |
| Potensiometer  | 16                      | Input analog          |
| Rotary Encoder | CLK = 4, DT = 5, SW = 6 | Input putaran & klik  |
| Button         | 21                      | Tombol tekan digital  |
| LED            | 2                       | Indikator nyala/mati  |
| Buzzer         | 1                       | Bunyi periodik        |
| Stepper Motor  | IN1–IN4 = 37–40         | Putaran dua arah      |

---

## 📦 Fitur FreeRTOS yang Digunakan

| Fitur                         | Deskripsi                                   |
| ----------------------------- | ------------------------------------------- |
| **xTaskCreatePinnedToCore()** | Menjalankan task di core tertentu           |
| **vTaskDelay()**              | Delay tanpa menghentikan core lain          |
| **vTaskDelete()**             | Menghapus task (digunakan di OLED)          |
| **TaskHandle_t**              | Variabel untuk menyimpan handle setiap task |
| **pdMS_TO_TICKS()**           | Konversi dari milidetik ke tick RTOS        |

---

## 🔍 Contoh Output di Serial Monitor

```
=== FreeRTOS 8 Task Started ===
Servo: Task berjalan
Stepper: Task berjalan
Task LED: Nyala
Task Buzzer: Nyala
Task Potensiometer value: 52%
Task Encoder value: 3
Task Button: Diklik
Task LED: Mati
Task Buzzer: Mati
```

---

## 🚀 Cara Menjalankan

1. Buka project di **Arduino IDE** atau **PlatformIO**.
2. Pilih board **ESP32-S3 DevKitC-1**.
3. Pastikan library berikut terpasang:

   * **Adafruit GFX Library**
   * **Adafruit SSD1306**
   * **ESP32Servo**
   * **AccelStepper**
4. Upload program ke board ESP32-S3.
5. Buka **Serial Monitor (115200 baud)** untuk melihat aktivitas task.

---

## 🧪 Eksperimen yang Disarankan

* Ubah **prioritas task** untuk melihat pengaruhnya terhadap performa.
* Tambahkan **vTaskSuspend() / vTaskResume()** untuk menghentikan atau melanjutkan task tertentu.
* Coba pindahkan task antar core untuk mengamati perbedaan respons.

---

## 👨‍💻 Kontributor

| Nama                         | NIM        | Peran                                  |
| ---------------------------- | ---------- | -------------------------------------- |
| **Husain Anshoruddin**       | 3223600020 | Pengembang RTOS Core & Task Management |
| **Muhammad Bintang Saputra** | 3223600023 | Integrasi Komponen & Dokumentasi       |

📍 **Politeknik Elektronika Negeri Surabaya (PENS)**
**Topik:** Praktikum FreeRTOS Multitasking pada ESP32-S3

---

ingin saya bantu tambahkan juga **tabel alokasi core & prioritas** biar makin rapi?
