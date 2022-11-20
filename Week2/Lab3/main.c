Skip to content
Product
Solutions
Open Source
Pricing
Search
Sign in
Sign up
DuyThinh1415
/
HCMUT_HTN_Lab
Public
Code
Issues
Pull requests
Actions
Projects
Security
Insights
HCMUT_HTN_Lab/Lab 3/main.c
@DuyThinh1415
DuyThinh1415 Add lab3
Latest commit e30cad7 8 hours ago
 History
 1 contributor
166 lines (135 sloc)  4.18 KB

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <inttypes.h>
#include <stdio.h>
#include <sys/time.h>

// choosing one option and define that, undef the other
// don't forget changing option in FreeRTOSConfig.h
#undef POLICY_PREEMPTIVE
#define POLICY_COOPERATIVE

#define GPIO_BUTTON GPIO_NUM_0
#define SHORT_TASK_LOOPS_COUNT 2000000u
#define YIELD_TASK_LOOPS_COUNT 2000000u

static uint32_t uxLastTaskID = -1u;
static uint32_t uxLastTaskID_old = -1u;

void printCurrentState(void)
{
    if (uxLastTaskID != uxLastTaskID_old)
    {
        uxLastTaskID_old = uxLastTaskID;
        struct timeval tv_now;
        gettimeofday(&tv_now, NULL);
        printf("%ld.%06ld\tTick: %04u, ID: %u\n",
               tv_now.tv_sec, tv_now.tv_usec, xTaskGetTickCount(), uxLastTaskID);
    }
}



#ifdef POLICY_PREEMPTIVE
static TaskHandle_t contTask01_handle = NULL;
static TaskHandle_t contTask02_handle = NULL;
static TaskHandle_t eventTask03_handle = NULL;

void IRAM_ATTR button_press_handler(void* arg)
{
    xTaskResumeFromISR(eventTask03_handle);
}

void initializeGPIO(void)
{
    gpio_reset_pin(GPIO_BUTTON);
    gpio_set_direction(GPIO_BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_BUTTON, GPIO_PULLUP_ONLY);
    gpio_pullup_en(GPIO_BUTTON);
    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
    gpio_set_intr_type(GPIO_BUTTON, GPIO_INTR_NEGEDGE);
    gpio_isr_handler_add(GPIO_BUTTON, button_press_handler, NULL);
    gpio_intr_enable(GPIO_BUTTON);
}

void vContinuousTask(void *param)
{
    while (1)
    {
        uxLastTaskID = (uint32_t) param;
        printCurrentState();
    }
    vTaskDelete(NULL);
}

void vEventTask(void *param)
{
    vTaskSuspend(NULL);

    while (1)
    {
        printf("Event task %u start running\n", (uint32_t) param);

        for (UBaseType_t i = 0; i < SHORT_TASK_LOOPS_COUNT; i++)
        {
            uxLastTaskID = (uint32_t) param;
            printCurrentState();
        }
        printf("Event task %u go into suspend\n", (uint32_t) param);
        vTaskSuspend(NULL);
    }
    vTaskDelete(NULL);
}
#endif



#ifdef POLICY_COOPERATIVE
static TaskHandle_t yieldTask01_handle = NULL;
static TaskHandle_t yieldTask02_handle = NULL;
static TaskHandle_t yieldTask03_handle = NULL;
static TaskHandle_t yieldTask04_handle = NULL;

void IRAM_ATTR button_press_handler(void* arg)
{
    xTaskResumeFromISR(yieldTask01_handle);
    xTaskResumeFromISR(yieldTask02_handle);
    xTaskResumeFromISR(yieldTask03_handle);
    xTaskResumeFromISR(yieldTask04_handle);
}

void initializeGPIO(void)
{
    gpio_reset_pin(GPIO_BUTTON);
    gpio_set_direction(GPIO_BUTTON, GPIO_MODE_INPUT);

    gpio_set_pull_mode(GPIO_BUTTON, GPIO_PULLUP_ONLY);
    gpio_pullup_en(GPIO_BUTTON);

    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
    gpio_set_intr_type(GPIO_BUTTON, GPIO_INTR_NEGEDGE);
    gpio_isr_handler_add(GPIO_BUTTON, button_press_handler, NULL);
    gpio_intr_enable(GPIO_BUTTON);
}

void vTaskWithYield(void *param)
{
    uint32_t taskID = (uint32_t) param;
    vTaskDelay(10 * taskID);

    while (1)
    {
        for (UBaseType_t i = 0; i < YIELD_TASK_LOOPS_COUNT; i++)
        {
            uxLastTaskID = taskID;
            printCurrentState();
        }

        if (taskID > 1)
        {
            vTaskSuspend(NULL);
        }
        else
        {
            taskYIELD();
        }
    }

    vTaskDelete(NULL);
}
#endif



void app_main(void)
{
    vTaskPrioritySet(NULL, 10);
    initializeGPIO();

#ifdef POLICY_PREEMPTIVE
    xTaskCreate(vContinuousTask, "Continous task #1", 2048, (void*) 1u, 1, &contTask01_handle);
    xTaskCreate(vContinuousTask, "Continous task #2", 2048, (void*) 2u, 1, &contTask02_handle);

    xTaskCreate(vEventTask, "Event task #3", 2048, (void*) 3, 2, &eventTask03_handle);
#endif

#ifdef POLICY_COOPERATIVE
    xTaskCreate(vTaskWithYield, "Yielding task #1", 2048, (void*) 1u, 1, &yieldTask01_handle);
    xTaskCreate(vTaskWithYield, "Yielding task #2", 2048, (void*) 2u, 2, &yieldTask02_handle);
    xTaskCreate(vTaskWithYield, "Yielding task #3", 2048, (void*) 3u, 3, &yieldTask03_handle);
    xTaskCreate(vTaskWithYield, "Yielding task #4", 2048, (void*) 4u, 4, &yieldTask04_handle);
#endif

    vTaskDelete(NULL);
}
Footer
© 2022 GitHub, Inc.
Footer navigation
Terms
Privacy
Security
Status
Docs
Contact GitHub
Pricing
API
Training
Blog
About
HCMUT_HTN_Lab/main.c at main · DuyThinh1415/HCMUT_HTN_Lab · GitHub