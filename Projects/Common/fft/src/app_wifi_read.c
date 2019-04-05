/**
******************************************************************************
* @file    app_wifi_read.c
* @author  Central LAB
 * @version	3.0.0
 * @date	20-July-2018
* @brief   This file contains all routines related to 
*          application level socket read from WIFI device.
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
*
* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
*
*        http://www.st.com/software_license_agreement_liberty_v2
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


#ifdef USE_WIFI_SPWF04

#include "stdint.h"
#include "stdio.h"
#include "wifi_interface.h"

#if (defined USE_STM32F4XX_NUCLEO)
#include "stm32f4xx_hal.h"
#elif (defined USE_STM32L4XX_NUCLEO)
#include "stm32l4xx_hal.h"
#endif // (defined USE_STM32F4XX_NUCLEO)

#define  RX_DATA_SIZE	6144
volatile uint8_t  bufferRead[RX_DATA_SIZE]; 
volatile uint32_t bufferReadOut   = 0;
volatile uint32_t bufferReadIn    = 0;
volatile uint8_t  bufferReadError = 0;


/**
* @brief Enque socket read data to an Application buffer
*
* Socket data read is asychronous to STM32 code. Top of that, Socket Read is called from a FreeRTOS thread 
* and Real Socket read callback (when data arrives asychronously) happens in ISR out of the FreeRTOS context.
* So need a Storage location to bridge between two stack, one in ISR context and another in FreeRTOS context.
* "bufferRead" is that bridge buffer/storage location. This function provides interface to add data to this buffer.
* @param unsigned char - data to be added to the buffer.
* @return unsigned char - number of bytes added to buffer (1 or 0)
*/

uint8_t EnqueueRx( uint8_t Data)
{
    __disable_irq();

    if(bufferReadOut == (bufferReadIn + 1)%RX_DATA_SIZE) 
    {
        __enable_irq();
        return 0x0;

    }
    else
    {

        bufferRead[bufferReadIn] = Data;
        bufferReadIn= (bufferReadIn + 1)% RX_DATA_SIZE;
        __enable_irq();
        return 0x1;
    }
}


/**
* @brief Deque socket read data from an Application buffer
*
* For detail refer to EnqueueRx(). This function provides interface to remove data from the bufferRead[] buffer.
* @param pointer to a Byte - data to be received from the buffer.
* @return Byte - number of bytes received from buffer (1 or 0)
*/
uint8_t DequeueRx( uint8_t *Data)
{

    __disable_irq();

   
    if(bufferReadOut == bufferReadIn)
    {
        __enable_irq();
        
        return 0;
    }
    else
    {

        *Data = bufferRead[bufferReadOut];
        bufferReadOut= (bufferReadOut+1)% RX_DATA_SIZE;
        __enable_irq();
        
        return 0x1;
    }
}

/**
* @brief Called asychronously by wifi driver when data arrives on AWS IoT socket
*
* Socket read callback (when data arrives asychronously). Calls the EnqueueRx()
* @param unsigned pointer to char - pointer to data buffer to be read.
* @param signed int - total message data size to be received
* @param signed int - partial message data size received of total message size
* @return no return
* @warning: further this function needs modification
*/
void ind_wifi_socket_data_received(int8_t server_id, int8_t socket_id, uint8_t * data_ptr, uint32_t message_size, uint32_t chunk_size, WiFi_Socket_t socket_type)
{
    volatile uint32_t i = 0;

    while(i < chunk_size )
    {
        if(EnqueueRx(data_ptr[i]))
        {
            i++;
        }
    }
}

/**
* @brief Called asychronously by wifi driver when client socket is closed by AWS IoT server
*
* 
* @param unsigned pointer to char - pointer to socket id
*/
void ind_wifi_socket_client_remote_server_closed(uint8_t *socketID, WiFi_Socket_t socket_type)
{
  uint8_t id = *socketID;
  printf("\r\n>>User Callback>>remote server socket closed\r\n");
  printf("Socket ID closed: %d",id);//this will actually print the character/string, not the number
  fflush(stdout);
}

#endif /* USE_WIFI_SPWF04 */



#ifdef USE_WIFI_SPWF01

#include "stdint.h"
#include "stdio.h"
#include "wifi_interface.h"

#if (defined USE_STM32F4XX_NUCLEO)
#include "stm32f4xx_hal.h"
#elif (defined USE_STM32L4XX_NUCLEO)
#include "stm32l4xx_hal.h"
#endif // (defined USE_STM32F4XX_NUCLEO)

#define  RX_DATA_SIZE	6144 //1024
volatile uint8_t  bufferRead[RX_DATA_SIZE]; 
volatile uint32_t bufferReadOut   = 0;
volatile uint32_t bufferReadIn    = 0;
volatile uint8_t  bufferReadError = 0;


/**
* @brief Enque socket read data to an Application buffer
*
* Socket data read is asychronous to STM32 code. Top of that, Socket Read is called from a FreeRTOS thread 
* and Real Socket read callback (when data arrives asychronously) happens in ISR out of the FreeRTOS context.
* So need a Storage location to bridge between two stack, one in ISR context and another in FreeRTOS context.
* "bufferRead" is that bridge buffer/storage location. This function provides interface to add data to this buffer.
* @param unsigned char - data to be added to the buffer.
* @return unsigned char - number of bytes added to buffer (1 or 0)
*/

uint8_t EnqueueRx( uint8_t Data)
{
    __disable_irq();

    if(bufferReadOut == (bufferReadIn + 1)%RX_DATA_SIZE) 
    {
        __enable_irq();
        return 0x0;

    }
    else
    {

        bufferRead[bufferReadIn] = Data;
        bufferReadIn= (bufferReadIn + 1)% RX_DATA_SIZE;
        __enable_irq();
        return 0x1;
    }
}


/**
* @brief Deque socket read data from an Application buffer
*
* For detail refer to EnqueueRx(). This function provides interface to remove data from the bufferRead[] buffer.
* @param pointer to a Byte - data to be received from the buffer.
* @return Byte - number of bytes received from buffer (1 or 0)
*/
uint8_t DequeueRx( uint8_t *Data)
{

    __disable_irq();

   
    if(bufferReadOut == bufferReadIn)
    {
        __enable_irq();
        
        return 0;
    }
    else
    {

        *Data = bufferRead[bufferReadOut];
        bufferReadOut= (bufferReadOut+1)% RX_DATA_SIZE;
        __enable_irq();
        
        return 0x1;
    }
}

/**
* @brief Called asychronously by wifi driver when data arrives on AWS IoT socket
*
* Socket read callback (when data arrives asychronously). Calls the EnqueueRx()
* @param unsigned pointer to char - pointer to data buffer to be read.
* @param signed int - total message data size to be received
* @param signed int - partial message data size received of total message size
* @return no return
* @warning: further this function needs modification
*/
void ind_wifi_socket_data_received(int8_t server_id, int8_t socket_id, uint8_t * data_ptr, uint32_t message_size, uint32_t chunk_size, WiFi_Socket_t socket_type)

{
    volatile uint32_t i = 0;

    while(i < chunk_size )
    {
        if(EnqueueRx(data_ptr[i]))
        {
            i++;
        }
    }
}

/**
* @brief Called asychronously by wifi driver when client socket is closed by AWS IoT server
*
* 
* @param unsigned pointer to char - pointer to socket id
*/
void ind_wifi_socket_client_remote_server_closed(uint8_t *socketID, WiFi_Socket_t socket_type)
{
  uint8_t id = *socketID;
  printf("\r\n>>User Callback>>remote server socket closed\r\n");
  printf("Socket ID closed: %d",id);//this will actually print the character/string, not the number
  fflush(stdout);
}

#endif /* USE_WIFI_SPWF01 */
