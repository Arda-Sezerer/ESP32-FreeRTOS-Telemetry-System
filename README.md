# Gerçek Zamanlı Telemetri ve Eşik Tabanlı Alarm Sistemi (ESP32 + FreeRTOS)

ESP32 platformu üzerinde **FreeRTOS yapı taşları** (Görevler, Kuyruklar, İkili Semaphorlar) ve I2C arayüzü ile bağlanan **MPU6050 IMU sensörü** kullanılarak geliştirilmiş gerçek zamanlı bir gömülü sistem projesidir.

---

## 📌 Mimari ve Özellikler

- **Eşzamanlı Görev Mimarisi:** ESP32'nin çift çekirdekli mimarisi üzerinde çalışan çok görevli (multi-task) donanım yazılımı tasarımı.
- **Sensör Veri Toplama Görevi (`vTaskSensorRead`):** MPU6050 sensöründen I2C hattı üzerinden ivmeölçer ve jiroskop verilerini periyodik olarak okur.
- **Görevler Arası İletişim (FreeRTOS Kuyruğu - Queue):** Sensör telemetri verilerini veri toplama ve işleme görevleri arasında güvenli bir şekilde aktarır.
- **Telemetri İşleme Görevi (`vTaskTelemetryProcess`):** Kuyruktaki verileri tüketir, belirlenen eşik değerlerini analiz eder ve olay sinyallerini tetikler.
- **Kritik Olay Yönetimi (İkili Semaphor - Binary Semaphore):** Sensör sınır değerleri aşıldığında alarm ve uyarı mekanizmasını (`vTaskAlarmTrigger`) anında senkronize eder.
- **Donanım Entegrasyonu:**
  - **ESP32 DevKit V1**
  - **MPU6050 IMU Sensörü** (SDA/SCL I2C hatları)
  - Eşik uyarıları için GPIO pinlerinden sürülen durum LED'leri ve sesli uyarıcılar (Buzzer)

---

## 🛠️ Donanım ve Pin Yapılandırması

| Bileşen | ESP32 Pini | Protokol / İşlev |
| :--- | :--- | :--- |
| **MPU6050 SDA** | GPIO 21 | I2C Veri Hattı |
| **MPU6050 SCL** | GPIO 22 | I2C Saat Hattı |
| **Alarm LED / Çıkış** | GPIO 2 | Uyarı Çıkış Göstergesi |

---

## 📂 Proje Yapısı

```text
.
├── main.cpp                 # FreeRTOS görev uygulamalarını içeren ana kaynak kod
└── README.md                # Proje dokümantasyonu
