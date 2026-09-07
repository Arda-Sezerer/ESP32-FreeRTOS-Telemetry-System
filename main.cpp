#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#define LED_PIN 2
#define ACCEL_THRESHOLD 12.0 // m/s^2 cinsinden kritik ivme eşiği

QueueHandle_t xQueue;
SemaphoreHandle_t xAlarmSemaphore;
Adafruit_MPU6050 mpu;


typedef struct{// Sensör verilerini tutacak veri yapımız (Struct)
  float accelX;
  float accelY;
  float accelZ;
  float totalaccel;
}SensorData_t;

void vTask1(void *pvParameters){//1. görev veri üreten bir sensörün verisini kuyruğa ilet
  sensors_event_t a,g,temp;

  for(;;){
    //MPU6050 verilerini okur
    mpu.getEvent(&a, &g, &temp);

    SensorData_t data;
    data.accelX = a.acceleration.x;
    data.accelY = a.acceleration.y;
    data.accelZ = a.acceleration.z;

    //Total vektörel ivme hesabı
    data.totalaccel = sqrt(pow(data.accelX, 2) + pow(data.accelY, 2) + pow(data.accelZ, 2));

    //veri yapısını kuyruğa gönder
    xQueueSend(xQueue, &data, pdMS_TO_TICKS(100)); //100ms bekleme 
    
    // 200 ms periyotla veri oku (5 Hz)
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void vTask2(void *pvParameters){//1. görevden gelen verileri kuyruktan al ve işle
  SensorData_t gelenData;

  for (;;) {
    if (xQueueReceive(xQueue, &gelenData, portMAX_DELAY) == pdPASS) {
      Serial.printf("Task 2 [İşlemci]: İvme = %.2f m/s^2 (X:%.1f, Y:%.1f, Z:%.1f)\n",
                    gelenData.totalaccel, gelenData.accelX, gelenData.accelY, gelenData.accelZ);

      // Eşik değer aşıldıysa alarm bayrağını kaldır
      if (gelenData.totalaccel > ACCEL_THRESHOLD) {
        Serial.println("  ! [UYARI] Yüksek darbe/hareket tespit edildi!");
        xSemaphoreGive(xAlarmSemaphore);
      }
    }
  }
}

void vTask3(void *pvParameters){
  for (;;) {
    // Semaphore bekleyip uykuda kal
    if (xSemaphoreTake(xAlarmSemaphore, portMAX_DELAY) == pdTRUE) {
      Serial.println("\n>>> TASK 3 [ALARM]: LED YANACAK (DARBE TESPİTİ) <<<\n");
      
      digitalWrite(LED_PIN, HIGH);
      vTaskDelay(pdMS_TO_TICKS(1000)); // LED 1 saniye yansın
      digitalWrite(LED_PIN, LOW);
    }
  }
}


void setup(){
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  xQueue = xQueueCreate(5, sizeof(SensorData_t));//1. kuyruğu oluşturma 5 int değişkeni kapasitesi
  xAlarmSemaphore = xSemaphoreCreateBinary();//1. acil durum olay tanımı 

  // MPU6050 Başlatma
  if (!mpu.begin()) {
    Serial.println("MPU6050 sensörüne ulaşılamadı! Bağlantıları kontrol edin.");
    while (1) vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  Serial.println("MPU6050 Başarıyla Başlatıldı.");

  if(xQueue != NULL && xAlarmSemaphore != NULL){
    //1. görevi işletim sistemine kayıt etme
    xTaskCreate(
      vTask1,    //çağırılacak fonksiyon 
      "SensörOkuma", //görevin adı
      3072,      //göreve ayrılan byte cinsinden bellek boyutu
      NULL,      //fonksiyona gönderilcek olan parametrenin adresi(yoksa null)
      2,         //öncelik seviyesi
      NULL       //ileride durdulup başlatılmayacaksa null
    );

    //2. görevi işletim sistemine kayıt etme
    xTaskCreate(
      vTask2,
      "Veriİşleme",
      2048,
      NULL,
      1,         //öncelik sırası 
      NULL        
    );

    //3. görev işletim sistemine kayıt ediliyor
    xTaskCreate(
      vTask3,
      "AlarmSürücü",
      2048,
      NULL,
      3,        // Önceliği 3 yaptık (Daha acil)         
      NULL        
    );
  }
  else{
    Serial.println("Kuyruk Oluşturulamadı");
  }
}

void loop(){

}

