//#include <wirish/wirish.h>
//#include "libraries/FreeRTOS/MapleFreeRTOS.h"
#include <MapleFreeRTOS900.h>
#define BOARD_LED_PIN PC13
#define SENSOR_PIN PB15
#define TEST_PIN PB5
xSemaphoreHandle sensorMutex = 0; 

struct freq {
  int LED;
  int longDelay;
  int shortDelay;
} LEDfreq[2] = {
  {PA8, 100, 10},
  {PA9,   74,  7}
};


static void vLEDFlashTask(void *pvParameters) {
  for (;;) {
    struct freq *Lfreq = (struct freq *)pvParameters;
    int ix;
      vTaskDelay(Lfreq->longDelay);
      for (ix = 0; ix < 256; ix++) {
        if(xSemaphoreTake(sensorMutex,100)){   
          analogWrite(Lfreq->LED, ix);
          xSemaphoreGive(sensorMutex);
        }   
       vTaskDelay(Lfreq->shortDelay);
       }
       
      vTaskDelay(Lfreq->longDelay);
      for (ix = 255; ix >= 0; ix--) {
        if(xSemaphoreTake(sensorMutex,100)){   
          analogWrite(Lfreq->LED, ix);
          xSemaphoreGive(sensorMutex); 
        } 
        vTaskDelay(Lfreq->shortDelay);
    }
  }
}

//Sensor task, ir-sensor returns 0 when triggerd
static void vSensorInTask(void *pvParameters){
  for(;;){
    if(digitalRead(SENSOR_PIN) == 0){
       xSemaphoreTake(sensorMutex, 100);
       digitalWrite(TEST_PIN, HIGH);
       vTaskDelay(1);
       xSemaphoreGive(sensorMutex);   
    }
    else{     
       digitalWrite(TEST_PIN, LOW);   
       vTaskDelay(1);    
    }       
  }
}

void setup() {
  // initialize the digital pin as an output:
  pinMode(TEST_PIN, OUTPUT);
  
  //Create Semaphore
  sensorMutex = xSemaphoreCreateMutex();
  
  //Setup Tasks
  xTaskCreate(vLEDFlashTask,
              "LEDblink1",
              configMINIMAL_STACK_SIZE,
              (void *)&LEDfreq[0],
              tskIDLE_PRIORITY + 2,
              NULL);
  xTaskCreate(vLEDFlashTask,
              "LEDblink2",
              configMINIMAL_STACK_SIZE,
              (void *)&LEDfreq[1],
              tskIDLE_PRIORITY + 2,
              NULL);
   xTaskCreate(vSensorInTask,
              "Sensor1",
              configMINIMAL_STACK_SIZE,
              (void *)NULL,
              tskIDLE_PRIORITY + 4,
              NULL);
           
  vTaskStartScheduler();
}


void loop() {
  // Insert background code here
}
