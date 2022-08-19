#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/apps/sntp.h"

#include "user_sntp.h"
char* SntpServerNames[3] ={

"ntp1.aliyun.com",
"ntp2.aliyun.com",
"ntp3.aliyun.com"
};



/******************************************************************************
 * FunctionName : ATaskSntp
 * Description  : ATaskSntp 任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ATaskSntp( void *pvParameters )
{
    STATION_STATUS Status;
    uint32 time;

    do{

        Status = wifi_station_get_connect_status();
        vTaskDelay(100);
    
    }while(Status != STATION_GOT_IP);
    printf("task is SNTP\n");
    printf("STATION_GOT_IP!\n");
    sntp_setservername(0,SntpServerNames[0]);
    sntp_setservername(1,SntpServerNames[1]);
    sntp_setservername(2,SntpServerNames[2]);
    sntp_init();
    for(;;)
    {

        time = sntp_get_current_timestamp();
        if(time)
        {
            printf("current date:%s\n",sntp_get_real_time(time));
            




        }
        vTaskDelay(500);




    }
    vTaskDelete(NULL);
}




/******************************************************************************
 * FunctionName : Sntp_init
 * Description  : Sntp_init 初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void Sntp_init(void)
{

    xTaskCreate(ATaskSntp, "Sntp", 512, NULL, 4, NULL);

}



