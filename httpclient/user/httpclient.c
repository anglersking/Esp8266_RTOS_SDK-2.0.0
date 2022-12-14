#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "httpclient.h"

#define SERVERADDR "192.168.31.158"
#define SERVERPORT 8000
const char GetStr[] = "GET /index.html HTTP/1.1\r\n"
"Host: www.baidu.com\r\n"
"\r\n";

#define HTTP_RCV_LEN 50000
/******************************************************************************
 * FunctionName : ATaskHttpClient
 * Description  : ATaskHttpClient 任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ATaskHttpClient( void *pvParameters )
{
    int iVariableExample = 0;
    int fd = -1;

    int NetTimeOnt = 20000;
    int ret;
    int i;
    struct hostent *phostent;
    struct sockaddr_in ServerAddr;
    char *Httpmsg;
    char *ipaddr = NULL;

    STATION_STATUS StaStatus;
    do
    {
        StaStatus = wifi_station_get_connect_status();
        vTaskDelay(100);



    }while(StaStatus != STATION_GOT_IP);


    
    fd = socket(PF_INET,SOCK_STREAM,0);
    if(fd == -1)
    {
        printf("get socket fail!\n");
        vTaskDelete(NULL);
        return;

    }

    setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&NetTimeOnt,sizeof(int));
    phostent = gethostbyname("www.baidu.com");
    if(phostent == NULL)
    {


        printf("get host ip fail!\n");
        vTaskDelete(NULL);
        return;



    }
    else
    {

        for(i = 0;phostent->h_addr_list[i];i++)
        {

              ipaddr = inet_ntoa(*(struct in_addr*)(phostent->h_addr_list[i]));
               if(ipaddr != NULL)
               {

                    printf("host addr is:%s\n",  ipaddr );

                    break;

               }




        }
        if(ipaddr == NULL)
        {

            printf("error:get ip fail!\r\n");

            vTaskDelete(NULL);
            return;

        }





    }
    
    memset(&ServerAddr,0,sizeof(ServerAddr));
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = inet_addr(ipaddr);
    ServerAddr.sin_port = htons(80);
    ServerAddr.sin_len = sizeof(ServerAddr);

    do
        {
            ret = connect(fd,(struct sockaddr*)&ServerAddr,ServerAddr.sin_len);
            if(ret != 0)
            {

                printf("connect is fail!\n");
                vTaskDelay(100);

            }
        }
    while (ret != 0);
    

    Httpmsg = zalloc(HTTP_RCV_LEN);
	send(fd,GetStr,strlen(GetStr),0);
    for(;;)
    {


        do
         {
            ret = recv(fd,Httpmsg,50000,0);
            if(ret > 0)
            {
				//printf("ret=%d\n",ret);
                printf("%s",Httpmsg);



            }
            else
            {
                printf("HttpServer data is no!\n");



            }

         }while(ret == -1);
        


    }
	free(Httpmsg);
    vTaskDelete( NULL );





}


/******************************************************************************
 * FunctionName : HttpClient_init
 * Description  : HttpClient_init 初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void HttpClient_init(void)
{

    xTaskCreate(ATaskHttpClient, "HttpClient", 256, NULL, 4, NULL);

}




