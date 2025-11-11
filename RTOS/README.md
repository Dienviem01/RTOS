# Praktikum Real Time Operating System (RTOS) dengan ESP32-S3

Repository ini berisi beberapa contoh praktikum RTOS pada **ESP32-S3**, seperti **Button**, **LED**, **Buzzer**, **OLED**, **Potentiometer**, **Servo**, dan **Stepper**.  
Fokus utama adalah bagaimana cara membagi pekerjaan ke dua core (**core 0** dan **core 1**) menggunakan **FreeRTOS** di Arduino ESP32.

---

## ⚙️ Perbedaan Core 0 dan Core 1 di ESP32

ESP32 memiliki dua inti prosesor (**dual-core**) yang dapat bekerja secara paralel.

- **Core 0**  
  Digunakan oleh sistem dan proses latar belakang seperti WiFi, Bluetooth, dan task internal ESP-IDF.

- **Core 1**  
  Menjalankan kode utama pengguna (fungsi `setup()` dan `loop()`) saat program diunggah melalui Arduino IDE.

Dalam proyek ini, kedua core digunakan untuk menjalankan task yang berbeda agar sistem bekerja secara paralel. Contohnya:

- **Core 0**: Membaca input dari potensiometer atau tombol dan menampilkan hasil ke OLED pertama.  
- **Core 1**: Mengontrol perangkat lain seperti servo, stepper, atau OLED kedua.

---

## 🧠 Ciri Khas Program FreeRTOS di Repository Ini

Program dalam repository ini menggunakan **FreeRTOS**, sistem operasi real-time bawaan dari ESP32.  
Berikut beberapa ciri khasnya:

---

### 1. Struktur Fungsi Task

Setiap task ditulis dalam bentuk fungsi seperti ini:

```cpp
void core0Task(void *pvParameters) {
  for (;;) {
    // Kode yang berjalan terus di core 0
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
```

**Ciri khasnya:**
- Menggunakan `void *pvParameters` sebagai parameter.  
- Loop tanpa akhir `for(;;)` atau `while(true)` menandakan ini adalah **task RTOS**, bukan fungsi biasa.  
- Menggunakan `vTaskDelay()` sebagai pengganti `delay()` agar scheduler bisa mengatur task lain.

---

### 2. Mengatur Task di Core Tertentu

FreeRTOS pada ESP32 memungkinkan menjalankan task di core tertentu menggunakan fungsi berikut:

```cpp
xTaskCreatePinnedToCore(
  taskFunction,    // Fungsi task yang akan dijalankan
  "TaskName",      // Nama task
  stackSize,       // Ukuran stack (biasanya 4096 atau 10000)
  parameter,       // Parameter (biasanya NULL)
  priority,        // Prioritas task (1 = normal)
  handle,          // Handle task (NULL jika tidak diperlukan)
  coreID           // Nomor core: 0 atau 1
);
```

**Contoh penggunaan di repository ini:**

```cpp
xTaskCreatePinnedToCore(core0Task, "Core0Task", 10000, NULL, 1, NULL, 0);
xTaskCreatePinnedToCore(core1Task, "Core1Task", 10000, NULL, 1, NULL, 1);
```

**Artinya:**
- Task `core0Task` dijalankan di **core 0**.  
- Task `core1Task` dijalankan di **core 1**.

---

### 3. Fungsi vTaskDelay()

Berbeda dengan `delay()`, fungsi ini menunda task **tanpa menghentikan seluruh prosesor**:

```cpp
vTaskDelay(100 / portTICK_PERIOD_MS);
```

Artinya task akan “tidur” selama 100 ms dan memberikan waktu bagi scheduler untuk menjalankan task lain di core yang sama atau berbeda.

---

### 4. Loop() Kosong

Karena semua proses utama sudah dijalankan di task RTOS, bagian `loop()` biasanya dibiarkan kosong:

```cpp
void loop() {
  // Kosong, semua kerja dilakukan di task RTOS
}
```

---

## 🧩 Ringkasan

| Konsep                        | Keterangan                                                |
| ----------------------------- | --------------------------------------------------------- |
| **Core 0**                    | Digunakan untuk task sistem dan background                |
| **Core 1**                    | Default core tempat `loop()` dan kode pengguna dijalankan |
| **FreeRTOS**                  | Sistem operasi real-time bawaan ESP32                     |
| **Task RTOS**                 | Fungsi terpisah yang berjalan paralel                     |
| **vTaskDelay()**              | Pengganti `delay()` agar multitasking tetap berjalan       |
| **xTaskCreatePinnedToCore()** | Fungsi untuk menentukan task dijalankan di core mana      |

---

## 🔗 Link Wokwi dan Video

| No | Judul Praktikum            | Link Wokwi                                                         | Link Video (Drive Shortcut)                            |
|----|----------------------------|--------------------------------------------------------------------|--------------------------------------------------------|
| 1  | Button                     | [Wokwi Project](https://wokwi.com/projects/447244574109811713)     | [Drive](https://drive.google.com/xxxxxxxx)             |
| 2  | LED                        | [Wokwi Project](https://wokwi.com/projects/447245467456890881)     | [Drive](https://drive.google.com/eeeeeeee)             |
| 3  | Potentiometer              | [Wokwi Project](https://wokwi.com/projects/447251511213579265)     | [Drive](https://drive.google.com/yyyyyyyy)             |
| 4  | Buzzer                     | [Wokwi Project](https://wokwi.com/projects/447247199660893185)     | [Drive](https://drive.google.com/zzzzzzzz)             |
| 5  | Servo Motor                | [Wokwi Project](https://wokwi.com/projects/447252879958924289)     | [Drive](https://drive.google.com/aaaaaaaa)             |
| 6  | Stepper Motor              | [Wokwi Project](https://wokwi.com/projects/447251511213579265)     | [Drive](https://drive.google.com/bbbbbbbb)             |
| 7  | OLED Display               | [Wokwi Project](https://wokwi.com/projects/447239534357789697)     | [Drive](https://drive.google.com/cccccccc)             |
| 8  | Rotary Encoder             | [Wokwi Project](https://wokwi.com/projects/447247992356260865)     | [Drive](https://drive.google.com/dddddddd)             |

---

## 🧪 Langkah Percobaan

1. Buka **link Wokwi** sesuai praktikum yang ingin diuji.  
2. Jalankan simulasi langsung di Wokwi.  
3. Amati aktivitas task di **core 0** dan **core 1** melalui Serial Monitor Wokwi.  
4. Ubah parameter seperti **delay**, **prioritas**, atau **stack size** untuk melihat efek pada eksekusi task.  

---


## 👨‍💻 Kontributor

**Nama:** Husain Anshoruddin & Muhammad Bintang Saputra

**NIM:** 3223600020 & 3223600023

**Topik:** Praktikum FreeRTOS dengan ESP32-S3

**Kampus:** Politeknik Elektronika Negeri Surabaya
