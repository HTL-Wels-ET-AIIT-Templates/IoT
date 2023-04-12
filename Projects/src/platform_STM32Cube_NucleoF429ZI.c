/**
 ******************************************************************************
 * @file    platform_STM32Cube_NucleoF429ZI.c
 * @author  Central LAB
 * @version	3.0.0
 * @date	20-July-2018
 * @brief   Platform specific adapter
 ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
 */

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "main.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "cmsis_os.h"
#include "ethernetif.h"
#include "lwip/dhcp.h"

#include "STM32CubeInterface.h"
#include "user.h"

/* NTP end point */
#define NTP_ENDPOINT_IPADDRESS_DEFAULT         "time.nist.gov"
#define NTP_ENDPOINT_TCPPORT           37

/** @addtogroup FFT_DEMO_NUCLEO_F429ZI
  * @{
  */
   


/* Exported variables ---------------------------------------------------------*/
TargetFeatures_t TargetBoardFeatures;

TIM_HandleTypeDef    TimCCHandle;

struct netif gnetif; /* network interface structure */

/* Imported variables --------------------------------------------------------*/
extern const char certificates[];

void Error_Handler(void);
time_t SynchronizationAgentConvertNTPTime2EpochTime(uint8_t* pBufferTimingAnswer,size_t sizeBuffer);
int TimingSystemSetSystemTime(time_t epochTimeNow);
void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate);
void Button_ISR(void);


/* Inside of Sector 23 */
#define MDM_FLASH_ADD ((uint32_t)(0x081FF000))
#define MDM_FLASH_SECTOR FLASH_SECTOR_23

void DHCP_init(void const * argument);
#define MAX_DHCP_TRIES  4
__IO uint8_t DHCP_state;
uint8_t macAddress[6];

//2kHz/0.5 For Sensors Data data @ 0.5Hz
#define DEFAULT_TIM_CC1_PULSE  4000

/* Defines related to Clock configuration */    
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

/* DHCP process states */
#define DHCP_OFF                   (uint8_t) 0
#define DHCP_START                 (uint8_t) 1
#define DHCP_WAIT_ADDRESS          (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED      (uint8_t) 3
#define DHCP_TIMEOUT               (uint8_t) 4
#define DHCP_LINK_DOWN             (uint8_t) 5

void User_notification(struct netif *netif,BluemixEthConfiguration_t EthConfiguration);

volatile uint32_t Int_Current_Time1 = 0; /*!< Int_Current_Time1 Value */
volatile uint32_t Int_Current_Time2 = 0; /*!< Int_Current_Time2 Value */
volatile uint8_t AXL_INT_received = 0;
volatile uint8_t useTerm = 0;
volatile uint8_t funcChanged = 1;

/**
 * @brief  Function for initializing timers for sending the Telemetry data to IoT hub
 * @param  None
 * @retval None
 */
void InitTimers(void)
{
  uint32_t uwPrescalerValue;

  /* Timer Output Compare Configuration Structure declaration */
  TIM_OC_InitTypeDef sConfig;

  /* Compute the prescaler value to have TIM3 counter clock equal to 2 KHz */
  uwPrescalerValue = (uint32_t) ((SystemCoreClock / 2000) - 1); 

  /* Set TIM1 instance (Motion)*/
  /* Set TIM1 instance */
  TimCCHandle.Instance = TIM1;
  TimCCHandle.Init.Period        = 65535;
  TimCCHandle.Init.Prescaler     = uwPrescalerValue;
  TimCCHandle.Init.ClockDivision = 0;
  TimCCHandle.Init.CounterMode   = TIM_COUNTERMODE_UP;
  if(HAL_TIM_OC_Init(&TimCCHandle) != HAL_OK) {
    /* Initialization Error */
    Error_Handler();
  }

 /* Configure the Output Compare channels */
 /* Common configuration for all channels */
  sConfig.OCMode     = TIM_OCMODE_TOGGLE;
  sConfig.OCPolarity = TIM_OCPOLARITY_LOW;

  /* Output Compare Toggle Mode configuration: Channel1 */
  if(HAL_TIM_OC_ConfigChannel(&TimCCHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK) {
    /* Configuration Error */
    Error_Handler();
  }
}


/**
 * @brief  Function for initializing the Real Time Clock
 * @param  None
 * @retval None
 */
void InitRTC(void)
{
  RTC_HandleTypeDef *RtcHandle = &TargetBoardFeatures.RtcHandle;

  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follow:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain
  */

  RtcHandle->Instance = RTC;
  RtcHandle->Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle->Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle->Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle->Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle->Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle->Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

  if(HAL_RTC_Init(RtcHandle) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief  Function for setting the Real Time Clock
 * @param  None
 * @retval None
 */
void SetRTC(void)
{
  const char msg[] = "\n";
  char buf[8] = "";
  struct sockaddr_in address;
  struct hostent *server;
  
  char ipAddress[] = NTP_ENDPOINT_IPADDRESS_DEFAULT;
  uint8_t tcpPort  =  NTP_ENDPOINT_TCPPORT;
  int8_t socketHandle;
  
  time_t epochTimeToSetForSystem;

  /* Check if Data stored in BackUp register0: No Need to reconfigure RTC */
  while(HAL_RTCEx_BKUPRead(&TargetBoardFeatures.RtcHandle, RTC_BKP_DR0) != 0x32F2) {
    /* Configure RTC Calendar */
    printf("Contact NTP server %s \r\n", ipAddress);

    if ((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      printf("Can not create socket\r\n");
      return;
    }
    
    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(ipAddress);
    printf("ERROR, ipaddress for time server= %s\r\n", ipAddress);
    if (server == NULL) {
      printf("ERROR, no such host as %s\r\n", ipAddress);
      goto RETRY;
    }
    
    /* build the server's Internet address */
    memset((char *) &address, 0, sizeof(address));
    memcpy((char *)&address.sin_addr.s_addr,
           (char *)server->h_addr, server->h_length);
    address.sin_family = AF_INET;
    address.sin_port = htons(tcpPort);
    
    /* connect: create a connection with the server */
    if (connect(socketHandle, (struct sockaddr*)&address, sizeof(address)) < 0) {
      printf("ERROR connecting\r\n");
      goto RETRY;
    }

    /* write message to socket */
    if (write(socketHandle, msg, strlen(msg)) < 0) {
      printf("ERROR writing to socket");
      goto RETRY;
    }
    
    /* read reply from server */
    if ((read(socketHandle, buf, 8))< 0) {
      printf("ERROR reading from socket");
      goto RETRY;
    }
    
    buf[4]='\0';
    
    epochTimeToSetForSystem = SynchronizationAgentConvertNTPTime2EpochTime((uint8_t*)buf,4);
    if (TimingSystemSetSystemTime(epochTimeToSetForSystem)== 0){
      printf("Error Failed to set system time. \r\n");
      goto RETRY;
    } else {
      //printf("Set UTC Time: %s\r\n",(get_ctime(&epochTimeToSetForSystem)));
    }
    
  RETRY:
    /* close socket */
    close(socketHandle);

    if(HAL_RTCEx_BKUPRead(&TargetBoardFeatures.RtcHandle, RTC_BKP_DR0) != 0x32F2) {

      printf("I'll try again to connect to NTP server in 4 seconds\r\n");
      HAL_Delay(4000);
    }
  }
  /* RTC is ok */
  {
    uint8_t aShowTime[50] = {0};
    uint8_t aShowDate[50] = {0};
    RTC_CalendarShow(aShowTime,aShowDate);
    printf("Init Real Time Clock %s %s\r\n",aShowDate,aShowTime);
  }
}


/**
  * @brief  EXTI line detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == KEY_BUTTON_PIN)
  {
    Button_ISR();
  }

#ifdef FFT_DEMO
  uint8_t instance;

  BSP_ACCELERO_Get_Instance(ACCELERO_handle, &instance);

  if(GPIO_Pin == KEY_BUTTON_PIN)
  {

    /* Manage software debouncing*/
    int doOperation = 0;

    if(Int_Current_Time1 == 0 && Int_Current_Time2 == 0)
    {
      Int_Current_Time1 = user_currentTimeGetTick();
      doOperation = 1;
    }
    else
    {
      int i2;
      Int_Current_Time2 = user_currentTimeGetTick();
      i2 = Int_Current_Time2;

      /* If I receive a button interrupt after more than 300 ms from the first one I get it, otherwise I discard it */
      if((i2 - Int_Current_Time1)  > 300)
      {
        Int_Current_Time1 = Int_Current_Time2;
        doOperation = 1;
      }
    }

    if(doOperation)
    {
      if ( DataLoggerActive )
      {
        useTerm = 0;                       /* always off */
      }
      else
      {
        useTerm = ( useTerm ) ? 0 : 1;    /* toggle on each button pressed */
        funcChanged = 1;
      }
    }
  }

  if( GPIO_Pin == M_INT1_O_PIN )
  {
    if( instance == LIS2DH12_0 || instance == LSM6DSL_X_1 )
    {
      AXL_INT_received = 1;
      //printf("9. AXL_INT_received: %d\n\r", AXL_INT_received);
    }
  }

  if( GPIO_Pin == LSM303AGR_INT_O_PIN )
  {
    if( instance == LSM303AGR_X_0 )
    {
      AXL_INT_received = 1;
      //printf("1. AXL_INT_received: %d\n\r", AXL_INT_received);
    }
  }

  if( GPIO_Pin == LSM6DSL_INT1_O_PIN )
  {
    if( instance == LSM6DSL_X_0 )
    {
      AXL_INT_received = 1;
      //printf("2. AXL_INT_received: %d\n\r", AXL_INT_received);
    }
  }
#endif // FFT_DEMO
}

/**
  * @brief  Definition of ThreadAPI_Sleep
  * @param milliseconds: delay in millesconds
  */
void ThreadAPI_Sleep(unsigned int milliseconds)
{
  HAL_Delay(milliseconds);
}




/**
  * @brief  Initializes the lwIP stack
  * @param  BluemixEthConfiguration_t EthConfiguration Ethernet configuration
  * @retval None
  */
void Netif_Config(BluemixEthConfiguration_t EthConfiguration)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;	
  
  /* IP address setting */
  IP4_ADDR(&ipaddr, EthConfiguration.ipaddress[0], EthConfiguration.ipaddress[1], EthConfiguration.ipaddress[2], EthConfiguration.ipaddress[3]);
  IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
  IP4_ADDR(&gw, EthConfiguration.gwaddress[0], EthConfiguration.gwaddress[1], EthConfiguration.gwaddress[2], EthConfiguration.gwaddress[3]);
  
  /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
  struct ip_addr *netmask, struct ip_addr *gw,
  void *state, err_t (* init)(struct netif *netif),
  err_t (* input)(struct pbuf *p, struct netif *netif))
  
  Adds your network interface to the netif_list. Allocate a struct
  netif and pass a pointer to this structure as the first argument.
  Give pointers to cleared ip_addr structures when using DHCP,
  or fill them with sane numbers otherwise. The state pointer may be NULL.
  
  The init function pointer must point to a initialization function for
  your ethernet netif interface. The following code illustrates it's use.*/
  
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
  
  /*  Registers the default network interface. */
  netif_set_default(&gnetif);
  
  if (netif_is_link_up(&gnetif))
  {
    /* When the netif is fully configured this function must be called.*/
    netif_set_up(&gnetif);
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
  }
}

/**
  * @brief  Notify the User about the network interface config status
  * @param  netif: the network interface
  * @param  EthConfiguration: the Ethernet configuration
  * @retval None
  */
void User_notification(struct netif *netif,BluemixEthConfiguration_t EthConfiguration) 
{
  if (netif_is_up(netif))
  {
    /* Update DHCP state machine */
    if ( EthConfiguration.use_dhcp )
      DHCP_state = DHCP_START;
  }
  else
  {  
    /* Update DHCP state machine */
    if ( EthConfiguration.use_dhcp )
      DHCP_state = DHCP_LINK_DOWN;
  } 
}

/**
* @brief  DHCP Process
* @param  argument: network interface
* @retval None
*/
void DHCP_init(void const * argument)
{
  struct netif *netif = (struct netif *) argument;
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;
  struct dhcp *dhcp;
  uint32_t IPaddress;
  //int count = 0;
  
  for (;;)
  {
    switch (DHCP_state)
    {
    case DHCP_START:
      {
        ip_addr_set_zero_ip4(&netif->ip_addr);
        ip_addr_set_zero_ip4(&netif->netmask);
        ip_addr_set_zero_ip4(&netif->gw);       
        dhcp_start(netif);
        DHCP_state = DHCP_WAIT_ADDRESS;
      }
      break;
      
    case DHCP_WAIT_ADDRESS:
      {                
        if (dhcp_supplied_address(netif)) 
        {
          DHCP_state = DHCP_ADDRESS_ASSIGNED;	
          /* Read the new IP address */
          IPaddress = netif->ip_addr.addr;
          printf("IPaddress = %ld.%ld.%ld.%ld\r\n",
                 IPaddress&0xFF,
                 (IPaddress>> 8)&0xFF,
                 (IPaddress>>16)&0xFF,
                 (IPaddress>>24)&0xFF                 
                   );
          
          return;
        }
        else
        {
          dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);

          /* DHCP timeout */
          if (dhcp->tries > MAX_DHCP_TRIES)
          {
            DHCP_state = DHCP_TIMEOUT;
            
            /* Stop DHCP */
            dhcp_stop(netif);
            
            /* Static address used */
            IP_ADDR4(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
            IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
            IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
            netif_set_addr(netif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));
            
          }
        }
      }
      break;
  case DHCP_LINK_DOWN:
    {
      /* Stop DHCP */
      dhcp_stop(netif);
      DHCP_state = DHCP_OFF; 
    }
    break;
    default: break;
    }
    
    /* wait 250 ms */
    //printf("wait again: %d\n\r", count++);
    osDelay(250);
  }
}


/**
 * @brief User function for Erasing the MDM on Flash
 * @param None
 * @retval uint32_t Success/NotSuccess [1/0]
 */
uint32_t UserFunctionForErasingFlash(void) {
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t SectorError = 0;
  uint32_t Success=1;

  EraseInitStruct.TypeErase = TYPEERASE_SECTORS;
  EraseInitStruct.VoltageRange = VOLTAGE_RANGE_3;
  EraseInitStruct.Sector = FLASH_SECTOR_23;
  EraseInitStruct.NbSectors = 1;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK){
    /* Error occurred while sector erase. 
      User can add here some code to deal with this error. 
      SectorError will contain the faulty sector and then to know the code error on this sector,
      user can call function 'HAL_FLASH_GetError()'
      FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
    Success=0;
    Error_Handler();
  }

  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  return Success;
}

/**
 * @brief User function for Saving the MDM  on the Flash
 * @param void *InitMetaDataVector Pointer to the MDM beginning
 * @param void *EndMetaDataVector Pointer to the MDM end
 * @retval uint32_t Success/NotSuccess [1/0]
 */
uint32_t UserFunctionForSavingFlash(void *InitMetaDataVector,void *EndMetaDataVector)
{
  uint32_t Success=1;

  /* Store in Flash Memory */
  uint32_t Address = MDM_FLASH_ADD;
  uint32_t *WriteIndex;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  for(WriteIndex =((uint32_t *) InitMetaDataVector); WriteIndex<((uint32_t *) EndMetaDataVector); WriteIndex++) {
    if (HAL_FLASH_Program(TYPEPROGRAM_WORD, Address,*WriteIndex) == HAL_OK){
      Address = Address + 4;
    } else {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error
         FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
      Error_Handler();
      Success=0;
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
   to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();
 
  return Success;
}


/**
 * @}
 */

/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
