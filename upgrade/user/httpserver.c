#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "httpserver.h"
#include "upgradefirmware.h"

#define SERVERADDR "192.168.31.158"
#define SERVERPORT 80



const char *DefaultPage=
"<html>"
"<head><title>Default</title></head>"
"<body>I am Http server!</body>"
"</html>"
;


// 发送200 ok报头
int file_ok(int cfd, long flen)
{
    char *send_buf = zalloc(sizeof(char)*100);
    sprintf(send_buf, "HTTP/1.1 200 OK\r\n");
    send(cfd, send_buf, strlen(send_buf), 0);
    sprintf(send_buf, "Connection: keep-alive\r\n");
    send(cfd, send_buf, strlen(send_buf), 0);
    sprintf(send_buf, "Content-Length: %ld\r\n", flen);
    send(cfd, send_buf, strlen(send_buf), 0);
    sprintf(send_buf, "Content-Type: text/html\r\n");
    send(cfd, send_buf, strlen(send_buf), 0);
    sprintf(send_buf, "\r\n");
    send(cfd, send_buf, strlen(send_buf), 0);
    free(send_buf);
    return 0;
}




/******************************************************************************
 * FunctionName : ATaskHttpServer
 * Description  : ATaskHttpServer 任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ATaskHttpServer( void *pvParameters )
{
    int iVariableExample = 0;
    int fd = -1;
    int cfd = -1;
    int NetTimeOnt = 2000;
    int ret;

    struct sockaddr_in ServerAddr;
    struct sockaddr ClientAddr;
    socklen_t ClientAddrlen = sizeof(struct sockaddr);
    char *Httpmsg;
    char *Sendmsg;

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

    memset(&ServerAddr,0,sizeof(ServerAddr));
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = INADDR_ANY;
    ServerAddr.sin_port = htons(SERVERPORT);
    ServerAddr.sin_len = sizeof(ServerAddr);


    if(bind(fd,(struct sockaddr*)&ServerAddr,ServerAddr.sin_len) != 0)
    {

        
        printf("bind socket fail!\n");
        vTaskDelete(NULL);
        return;


    }

    if(listen(fd,5) != 0)
    {

        printf("listen socket fail!\n");
        vTaskDelete(NULL);
        return;


    }
    Httpmsg = (char*)zalloc(sizeof(char)*1000);
    for(;;)
    {

        
        cfd = accept(fd,&ClientAddr,&ClientAddrlen);
        if(cfd != -1)
        {
            
            printf("HttpClient accept\n");
            ret = recv(cfd,Httpmsg,1000,0);
            if(ret > 0)
            {
                if( strncmp( Httpmsg , "UpgradeFirmware:" , 16 ) == 0 ) 
                {
                if( ExcuteUpgrade( (char *)&Httpmsg[16] ) == true )
                 {
                  send( cfd , "Upgrade is Excuted!\r\n" , strlen( "Upgrade is Excuted!\r\n" ) , 0 );
                   }
                     else
                   {
                     send( cfd , "Upgrade fail!\r\n" , strlen( "Upgrade fail!\r\n" ) , 0 );
                 }
                }



            }
            else
            {
                printf("HttpClient data is no!\n");



            }






        }
        close(cfd);


    }
    free(Httpmsg);
    vTaskDelete( NULL );





}


/******************************************************************************
 * FunctionName : HttpServer_init
 * Description  : HttpServer_init 初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void HttpServer_init(void)
{

    xTaskCreate(ATaskHttpServer, "HttpServer", 256, NULL, 4, NULL);

}





