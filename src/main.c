#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "lcd/lcd.h"
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/queue.h"

#define mainQUEUE_RECIEVE_TASK_PRIORITY (tskIDLE_PRIORITY + 2)
#define mainQUEUE_SEND_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define mainQUEUE_LENGTH (1)

#define mainQUEUE_SEND_FREQUENCY_MS			pdMS_TO_TICKS(1000)

// Task declarations
static void prvQueueRecieveTask(void *pvParameters);
static void prvQueueSendTask(void *pvParameters);

static QueueHandle_t xQueue = NULL;
//static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];

int main( void )
{

  Lcd_Init();
  LCD_Clear(WHITE);
  BACK_COLOR = WHITE;

  xQueue = xQueueCreate( mainQUEUE_LENGTH , sizeof(uint32_t));

  if(xQueue != NULL)
  {
    xTaskCreate(prvQueueRecieveTask,
      "rx",
      configMINIMAL_STACK_SIZE * 2U,
      NULL,
      mainQUEUE_RECIEVE_TASK_PRIORITY,
      NULL);

    xTaskCreate(prvQueueSendTask,
      "tx",
      configMINIMAL_STACK_SIZE * 2U,
      NULL,
      mainQUEUE_SEND_TASK_PRIORITY,
      NULL);

    vTaskStartScheduler();
  }

  for(;;)
  {

  }
}

// Task definitions
static void prvQueueRecieveTask( void *pvParameters )
{
  TickType_t xNextWakeTime;
  const unsigned long ulValueToSend = 123UL;
  BaseType_t xReturned;

  // Use the parameter so the compiler doesn't complain
  (void) pvParameters;

  xNextWakeTime = xTaskGetTickCount();

  for(;;)
  {
    vTaskDelayUntil(&xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS);

    xReturned = xQueueSend(xQueue, &ulValueToSend, 0U);
    configASSERT(xReturned == pdPASS);
  }

  // Should NOT be reached, but included for safety
  vTaskDelete(NULL);
}

static void prvQueueSendTask( void *pvParameters )
{
  unsigned long ulRecievedValue;
  const unsigned long ulExpectedValue = 123UL;
  /*const char * const pcPassMessage = "Blink!";
  const char * const pcFailMessage = "Error!";*/

  (void) pvParameters;

  for(;;)
  {
    xQueueReceive(xQueue, &ulRecievedValue, portMAX_DELAY);

    //LCD_Clear(BLACK);
    if(ulRecievedValue == ulExpectedValue)
    {
      taskENTER_CRITICAL();
      LCD_ShowString(20, 20, (u8*) "Blink!", GREEN);
      taskEXIT_CRITICAL();
      ulRecievedValue = 0U;
    }
    else
    {
      taskENTER_CRITICAL();
      LCD_ShowString(20, 40, (u8*) "Error!", RED);
      taskEXIT_CRITICAL();
    }
    
  }

  // Should NOT be reached, but included for safety
  vTaskDelete(NULL);
}

void vAssertCalled( void )
{
  return;
}