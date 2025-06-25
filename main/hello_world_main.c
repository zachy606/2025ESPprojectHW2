// /*
//  * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
//  *
//  * SPDX-License-Identifier: CC0-1.0
//  */


#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#define N 4


static int M1[N][N] = {
    {1, 2, 2, 4}, 
    {2, 2, 2, 2},
    {2, 2, 2, 2},
    {2, 2, 2, 2},
};

static int M2[N][N] = {
    {3, 3, 3, 3}, 
    {3, 3, 3, 3},
    {3, 4, 3, 3},
    {3, 3, 3, 3},
};

static int M3[N][N] = {
    {0, 0, 0, 0}, 
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
};

static int sum = 0;
static SemaphoreHandle_t pos_mutex;

static portMUX_TYPE s_spinlock = portMUX_INITIALIZER_UNLOCKED;
static int cnt = 0;
static int i=0,j=0;
static int sumi=0,sumj=0;

void task(void *arg){
    int core_id = esp_cpu_get_core_id();
    printf("Task %s create\n", pcTaskGetName(NULL));
    
    while(1){
        int locali=0,localj=0;
        // if(xSemaphoreTake(pos_mutex, portMAX_DELAY)==pdTRUE){
            portENTER_CRITICAL(&s_spinlock);
            if(i==4)break;
            locali = i;
            localj = j;
            

            j++;
            if(j==4){
                j=0;
                i++;
            }
            
        // }
        portEXIT_CRITICAL(&s_spinlock);
        core_id = esp_cpu_get_core_id();
        printf("Task %s is multiplying on Core%d\n", pcTaskGetName(NULL), core_id);
        xSemaphoreGive(pos_mutex);
        for(int k=0;k<N;k++){
                M3[locali][localj] += M1[locali][k] * M2[k][localj];
        }
        

        // vTaskDelay(pdMS_TO_TICKS(1));
        // if(i==4)break;
    }

    // xSemaphoreGive(pos_mutex);
    portEXIT_CRITICAL(&s_spinlock);

    while(1){
        int locali=0,localj=0;
        portENTER_CRITICAL(&s_spinlock);
        // if(xSemaphoreTake(pos_mutex, portMAX_DELAY)==pdTRUE){
            if(sumi==4)break;
            locali = sumi;
            localj = sumj;
            sumj++;
            if(sumj==4){
                sumj=0;
                sumi++;
            }
            

        // }
        // xSemaphoreGive(pos_mutex);
        portEXIT_CRITICAL(&s_spinlock);
        printf("Task %s is summing on Core%d\n", pcTaskGetName(NULL), core_id);
        sum += M3[locali][localj];
        // vTaskDelay(pdMS_TO_TICKS(1));

    }
    // xSemaphoreGive(pos_mutex);
    portEXIT_CRITICAL(&s_spinlock);
    cnt++;
    vTaskDelete(NULL);
    
}


void app_main(void) {
    pos_mutex = xSemaphoreCreateMutex();    
    
    // // create Multiplication tasks
    // xTaskCreate(Multiplication, "multiplicationA", 2048, NULL, 1, NULL);
    // xTaskCreate(Multiplication, "multiplicationB", 2048, NULL, 1, NULL);
    

    // // create Summation tasks
    // xTaskCreate(Summation, "summationA", 2048, NULL, 1, NULL);
    // xTaskCreate(Summation, "summationB", 2048, NULL, 1, NULL);


    xTaskCreate(task, "taskA", 2048, NULL, 1, NULL);
    xTaskCreate(task, "taskB", 2048, NULL, 1, NULL);

    // print M3 and sum
    while(cnt<2){
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    printf("M3: \n");
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            printf("%2d ", M3[i][j]);
        }
        printf("\n");
    }
    printf("sum of elements of M3: %d\n", sum);
}
LLL






// #include <stdio.h>
// #include <inttypes.h>
// #include "sdkconfig.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_chip_info.h"
// #include "esp_flash.h"
// #include "esp_system.h"

// void app_main(void)
// {
//     printf("Hello world!\n");

//     /* Print chip information */
//     esp_chip_info_t chip_info;
//     uint32_t flash_size;
//     esp_chip_info(&chip_info);
//     printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
//            CONFIG_IDF_TARGET,
//            chip_info.cores,
//            (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
//            (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
//            (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
//            (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

//     unsigned major_rev = chip_info.revision / 100;
//     unsigned minor_rev = chip_info.revision % 100;
//     printf("silicon revision v%d.%d, ", major_rev, minor_rev);
//     if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
//         printf("Get flash size failed");
//         return;
//     }

//     printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
//            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

//     printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

//     for (int i = 10; i >= 0; i--) {
//         printf("Restarting in %d seconds...\n", i);
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//     }
//     printf("Restarting now.\n");
//     fflush(stdout);
//     esp_restart();
// }

