#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ESP_INTR_FLAG_DEFAULT 0
#define CONFIG_BUTTON_PIN GPIO_NUM_27

TaskHandle_t TASK_2 = NULL;
TaskHandle_t TASK_1 = NULL;

const TickType_t X_DELAY = 1000 / portTICK_PERIOD_MS;

void IRAM_ATTR button_isr_handler(void* arg)
{
    // vTaskPrioritySet(TASK_2, 1);
    vTaskResume(TASK_2);
}

void button_task(void* arg)
{
    while (true)
    {
        vTaskSuspend(NULL);
        printf("ESP32\n");
        // vTaskPrioritySet(TASK_2, 1);
    }
}

void task_1()
{
    while(true) {
        printf("1911681\n");
        vTaskDelay(X_DELAY);
    }
}

void app_main()
{
    gpio_pad_select_gpio(CONFIG_BUTTON_PIN);

    gpio_set_direction(CONFIG_BUTTON_PIN, GPIO_MODE_INPUT);

    gpio_set_intr_type(CONFIG_BUTTON_PIN, GPIO_INTR_POSEDGE);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    gpio_isr_handler_add(CONFIG_BUTTON_PIN, button_isr_handler, NULL);

    xTaskCreate(task_1, "print MSSV", 4096, NULL, 2, &TASK_1);
    xTaskCreate(button_task, "button_task", 4096, NULL, 1, &TASK_2);
}