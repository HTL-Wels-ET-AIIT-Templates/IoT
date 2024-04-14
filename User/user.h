/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_USER_H
#define __USER_USER_H

/* Includes ------------------------------------------------------------------*/
/* Defines -------------------------------------------------------------------*/
/* MAC ADDRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
#define MAC_ADDR0   0x3E
#define MAC_ADDR1   0x1D
#define MAC_ADDR2   0x6D
#define MAC_ADDR3   0xAB
#define MAC_ADDR4   0xFC
#define MAC_ADDR5   0x0A

/*Static IP ADDRESS*/
#define IP_ADDR0   (uint8_t) 192
#define IP_ADDR1   (uint8_t) 168
#define IP_ADDR2   (uint8_t) 178
#define IP_ADDR3   (uint8_t) 200
   
/*NETMASK*/
#define NETMASK_ADDR0   (uint8_t) 255
#define NETMASK_ADDR1   (uint8_t) 255
#define NETMASK_ADDR2   (uint8_t) 255
#define NETMASK_ADDR3   (uint8_t) 0

/*Gateway Address*/
#define GW_ADDR0   (uint8_t) 192
#define GW_ADDR1   (uint8_t) 168
#define GW_ADDR2   (uint8_t) 178
#define GW_ADDR3   (uint8_t) 1

 /*DNS Server Address*/
 #define DNS_ADDR0   (uint8_t) 192
 #define DNS_ADDR1   (uint8_t) 168
 #define DNS_ADDR2   (uint8_t) 178
 #define DNS_ADDR3   (uint8_t) 1

/* DHCP Enable / Disable */
#define DHCP_ENABLE		0

/* MQTT Config */
#define MQTT_SERVER_URL		"docker.htl-wels.at"
#define MQTT_SERVER_PORT	1883
#define MQTT_CLIENT_ID		"MyDeviceName"

/* Exported types ------------------------------------------------------------*/
typedef enum {
  BP_NOT_PUSHED=0, 
  BP_SINGLE_PUSH, 
  BP_MULTIPLE_PUSH, 
  BP_TRIPLE_PUSH
} ButtonPush_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
int userInit(void);
void userTask(void);
ButtonPush_t Button_CheckForPush();
int PrepareMqttPayload(char * PayloadBuffer, int PayloadSize);



#endif /* __USER_USER_H */
