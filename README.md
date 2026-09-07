# Gerçek Zamanlı Telemetri ve Eşik Tabanlı Alarm Sistemi (ESP32 + FreeRTOS)

ESP32 platformu üzerinde **FreeRTOS yapı taşları** (Görevler, Kuyruklar, İkili Semaphorlar) ve I2C arayüzü ile bağlanan **MPU6050 IMU sensörü** kullanılarak geliştirilmiş gerçek zamanlı bir gömülü sistem projesidir.

---

## 📌 Mimari ve Çalışma Mantığı

Bu proje, sensör verilerinin toplanması, işlenmesi ve kritik durumların tespiti süreçlerini deterministik (zaman öncelikli) olarak yönetmek amacıyla çok görevli (multi-tasking) bir mimari üzerine kurulmuştur:

1. **Sensör Okuma Görevi (`vTaskSensorRead`):** 
   - MPU6050 sensöründen I2C protokolü üzerinden ivmeölçer ($a_x, a_y, a_z$) ve jiroskop ($g_x, g_y, g_z$) verilerini periyodik olarak sorgular.
   - Elde edilen ham verileri bir yapı (`struct SensorData`) haline getirir.

2. **Görevler Arası Veri Transferi (FreeRTOS Queue):**
   - Okunan veriler, yarış durumlarını (race condition) önlemek ve görevler arasındaki bağımlılığı kesmek için bir FreeRTOS Kuyruğuna (`xQueue`) gönderilir.

3. **Telemetri İşleme Görevi (`vTaskTelemetryProcess`):**
   - Kuyruktan gelen verileri okur ve toplam ivme Vektör Büyüklüğünü veya belirlenen sınır değerleri hesaplar.
   - Eğer okunan değer kritik eşik sınırını aşarsa (örneğin ani darbe veya aşırı eğim), sistem durumu uyarısına geçer ve bir İkili Semaphor (`xSemaphoreGive`) yayınlar.

4. **Kritik Olay ve Alarm Görevi (`vTaskAlarmTrigger`):**
   - Normal şartlarda semaphor bekleyerek bloklanmış (Blocked) durumda kalır, işlemci kaynağı tüketmez.
   - Semaphor sinyali geldiği anda uyanır ve GPIO çıkışına bağlı Alarm LED'ini / Buzzer'ı tetikleyerek donanımsal uyarı verir.

---

## 🛠️ Donanım ve Pin Bağlantı Tablosu

| Bileşen | ESP32 DevKit V1 Pini | Açıklama / Protokol |
| :--- | :--- | :--- |
| **MPU6050 VCC** | 3.3V / 5V | Güç Beslemesi |
| **MPU6050 GND** | GND | Ortak Toprak |
| **MPU6050 SDA** | GPIO 21 | I2C Veri Hattı |
| **MPU6050 SCL** | GPIO 22 | I2C Saat Hattı |
| **Uyarı LED'i / Buzzer** | GPIO 2 | Eşik Aşımı Alarm Çıkışı |

---

## 💻 Kurulum ve Çalıştırma

1. **Gereksinimler:**
   - ESP32 Kart Yöneticisi tanımlanmış Arduino IDE veya VS Code (PlatformIO).
   - `Adafruit MPU6050` ve `Adafruit Sensor` kütüphaneleri.

2. **Yükleme:**
   - Devre bağlantılarını yukarıdaki pin tablosuna göre yapın.
   - `main.cpp` dosyasındaki kodları projenize ekleyin ve ESP32 kartınıza yükleyin.
   - Seri Port Ekranını (Serial Monitor) **115200 baud** hızında açarak telemetri çıktılarını ve FreeRTOS görev durumlarını takip edin.

---

## 📂 Proje Yapısı

```text
.
├── main.cpp                 # FreeRTOS görevleri ve sensör sürücü kodları
└── README.md                # Detaylı proje dokümantasyonu
