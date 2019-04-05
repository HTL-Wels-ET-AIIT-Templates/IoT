/**
  ******************************************************************************
  * @file    net_tcp_c2c.c
  * @author  MCD Application Team
 * @version	3.0.0
 * @date	20-July-2018
  * @brief   Network abstraction at transport layer level. TCP implementation
  *          on ST C2C connectivity API.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "net_internal.h"

#ifdef USE_C2C

/* Private defines -----------------------------------------------------------*/
/* The socket timeout of the non-blocking sockets is supposed to be 0.
 * But the underlying component does not necessarily supports a non-blocking
 * socket interface.
 * The NOBLOCKING timeouts are intended to be set to the lowest possible value
 * supported by the underlying component. E.g. UART at 115200 needs at least 
 * 105 ms to send a package of 1500 bytes. */
#define NET_DEFAULT_NOBLOCKING_WRITE_TIMEOUT  200
#define NET_DEFAULT_NOBLOCKING_READ_TIMEOUT   200

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char OperatorsString[C2C_OPERATORS_LIST + 1]; 
//char IpAddrString[C2C_IPADDR_LIST + 1]; 


/* Private function prototypes -----------------------------------------------*/
int net_sock_create_tcp_c2c(net_hnd_t nethnd, net_sockhnd_t * sockhnd, net_proto_t proto);
int net_sock_open_tcp_c2c(net_sockhnd_t sockhnd, const char * hostname, int dstport);
int net_sock_recv_tcp_c2c(net_sockhnd_t sockhnd, uint8_t * buf, size_t len);
int net_sock_send_tcp_c2c(net_sockhnd_t sockhnd, const uint8_t * buf, size_t len);
int net_sock_close_tcp_c2c(net_sockhnd_t sockhnd);
int net_sock_destroy_tcp_c2c(net_sockhnd_t sockhnd);

/* Functions Definition ------------------------------------------------------*/

int net_sock_create_tcp_c2c(net_hnd_t nethnd, net_sockhnd_t * sockhnd, net_proto_t proto)
{
  int rc = NET_ERR;
  net_ctxt_t *ctxt = (net_ctxt_t *) nethnd;
  net_sock_ctxt_t *sock = NULL;
  
  sock = net_malloc(sizeof(net_sock_ctxt_t));
  if (sock == NULL)
  {
    msg_error("net_sock_create allocation failed.\n");
    rc = NET_ERR;
  }
  else
  {
    memset(sock, 0, sizeof(net_sock_ctxt_t));
    sock->net = ctxt;
    sock->next = ctxt->sock_list;
    sock->methods.create  = (net_sock_create_tcp_c2c);
    sock->methods.open    = (net_sock_open_tcp_c2c);
    sock->methods.recv    = (net_sock_recv_tcp_c2c);
    sock->methods.send    = (net_sock_send_tcp_c2c);
    sock->methods.close   = (net_sock_close_tcp_c2c);
    sock->methods.destroy = (net_sock_destroy_tcp_c2c);
    sock->proto           = proto;
    sock->blocking        = NET_DEFAULT_BLOCKING;
    sock->read_timeout    = NET_DEFAULT_BLOCKING_READ_TIMEOUT;
    sock->write_timeout   = NET_DEFAULT_BLOCKING_WRITE_TIMEOUT;
    ctxt->sock_list       = sock; /* Insert at the head of the list */
    *sockhnd = (net_sockhnd_t) sock;

    rc = NET_OK;
  }
  
  return rc;
}


int net_sock_open_tcp_c2c(net_sockhnd_t sockhnd, const char * hostname, int dstport)
{
  int rc = NET_ERR;
  net_sock_ctxt_t *sock = (net_sock_ctxt_t * ) sockhnd;
  uint8_t ip_addr[4] = { 0, 0, 0, 0 };
  
  sock->underlying_sock_ctxt = (net_sockhnd_t) -1; /* Initialize to a non-null value which may not be confused with a valid port number. */
  
  /* Find a free underlying socket on the network interface. */
  bool underlying_socket_busy[C2C_MAX_CONNECTIONS];
  memset(underlying_socket_busy, 0, sizeof(underlying_socket_busy));
  
  net_sock_ctxt_t * cur = sock->net->sock_list;
  do 
  {
    if ((cur->proto == NET_PROTO_TCP) && ((int) cur->underlying_sock_ctxt >= 0) )
    {
      underlying_socket_busy[(int) cur->underlying_sock_ctxt] = true;
    }
    cur = cur->next;
  } while (cur != NULL);
  
  for (int i = 0; i < C2C_MAX_CONNECTIONS; i++)
  {
    if (underlying_socket_busy[i] == false)
    {
      sock->underlying_sock_ctxt = (net_sockhnd_t) i;
      break;
    }
  }
  
  /* Free socket found */
  if (sock->underlying_sock_ctxt >= 0)
  {
    if (C2C_GetHostAddress((char *)hostname, ip_addr) != C2C_RET_OK)
    {
      // NB: This blocking call may take several seconds before returning. An asynchronous interface should be added.
      msg_info("The address of %s could not be resolved.\n", hostname);
      rc = NET_ERR;
    }
    else
    {
      if( C2C_RET_OK == C2C_OpenClientConnection(
            (uint32_t) sock->underlying_sock_ctxt, C2C_TCP_PROTOCOL, NULL, ip_addr, dstport, 0) )
      {
        rc = NET_OK;
      }
      else
      {
        underlying_socket_busy[(int) sock->underlying_sock_ctxt] = false;
        msg_error("Failed opening the underlying C2C socket %d.\n", (int) sock->underlying_sock_ctxt);
        sock->underlying_sock_ctxt = (net_sockhnd_t) -1;
      }
    }
  }
  else
  {
    msg_error("Could not find a free socket on the specified network interface.\n");
    rc = NET_PARAM;
  }
  
  return rc;
}


int net_sock_recv_tcp_c2c(net_sockhnd_t sockhnd, uint8_t * buf, size_t len)
{
  int rc = 0;
  C2C_Ret_t status = C2C_RET_OK;
  net_sock_ctxt_t *sock = (net_sock_ctxt_t * ) sockhnd;
  uint16_t read = 0;
  uint16_t tmp_len = MIN(len, C2C_PAYLOAD_SIZE);
  uint8_t * tmp_buf = buf;
  uint32_t start_time = HAL_GetTick();
    
  /* Read the received payload by chuncks of C2C_PAYLOAD_SIZE bytes because of
   * a constraint of C2C_ReceiveData(). */
  do
  {
    if ( (sock->blocking == true) && (net_timeout_left_ms(start_time, HAL_GetTick(), sock->read_timeout) <= 0) )
    {
      rc = NET_TIMEOUT;
      break;
    }
    
    status = C2C_ReceiveData((uint8_t) ((uint32_t)sock->underlying_sock_ctxt & 0xFF), tmp_buf, tmp_len, &read,
                             (sock->blocking == true) ? sock->read_timeout : NET_DEFAULT_NOBLOCKING_READ_TIMEOUT);

    msg_debug("Read %d/%d.\n", read, tmp_len);
    if (status != C2C_RET_OK)
    {
      msg_error("net_sock_recv(): error %d in C2C_ReceiveData() - socket=%d requestedLen=%d received=%d\n",
             status, (int) sock->underlying_sock_ctxt, tmp_len, read);
      msg_error("The port is likely to have been closed by the server.\n")
      rc = NET_EOF;
      break;  
    }
    else
    {
      if (read > tmp_len)
      {
        msg_error("C2C_ReceiveData() returned a longer payload than requested (%d/%d).\n", read, tmp_len);
        rc = NET_ERR;
        break;
      }
      tmp_buf += read;
      tmp_len = MAX(0, MIN(len - (tmp_buf - buf), C2C_PAYLOAD_SIZE));
    }
    
  } while ( (read == 0) && (sock->blocking == true) && (rc == 0) );
    
  return (rc < 0) ? rc : tmp_buf - buf;
}


int net_sock_send_tcp_c2c( net_sockhnd_t sockhnd, const uint8_t * buf, size_t len)
{
  int rc = 0;
  C2C_SendStatus_t status = C2C_SEND_OK;
  net_sock_ctxt_t *sock = (net_sock_ctxt_t * ) sockhnd;
  uint16_t sent = 0;
  uint32_t start_time = HAL_GetTick();
  char ErrorString[C2C_ERROR_STRING];
  
  do
  {
    if ( (sock->blocking == true) && (net_timeout_left_ms(start_time, HAL_GetTick(), sock->write_timeout) <= 0) )
    {
      rc = NET_TIMEOUT;
      break;
    }
    
    status = C2C_SendData((uint8_t) ((uint32_t)sock->underlying_sock_ctxt & 0xFF), (uint8_t *)buf, len, &sent,
                          (sock->blocking == true) ? sock->write_timeout : NET_DEFAULT_NOBLOCKING_WRITE_TIMEOUT );

    if (status !=  C2C_SEND_OK)
    {
      if (status == C2C_SEND_FAIL)
      {
        msg_error("C2C_SendData(): send FAIL: sending buffer might be full");
      }
      else
      {        
        C2C_RetrieveLastErrorDetails(ErrorString);
        msg_error("C2C_SendData(): send ERROR: %s", ErrorString);
      }
      
      rc = NET_ERR;
      msg_error("Send failed.\n");
      break;
    }
  
  } while ( (sent == 0) && (sock->blocking == true) && (rc == 0) );
  
  return (rc < 0) ? rc : sent;
}


int net_sock_close_tcp_c2c(net_sockhnd_t sockhnd)
{
  int rc = NET_ERR;
  net_sock_ctxt_t *sock = (net_sock_ctxt_t * ) sockhnd;
  C2C_Ret_t status = C2C_CloseClientConnection((uint8_t) ((uint32_t)sock->underlying_sock_ctxt && 0xFF));
  if (status == C2C_RET_OK)
  {
    sock->underlying_sock_ctxt = (net_sockhnd_t) -1;
    rc = NET_OK;
  }
  return rc;
}


int net_sock_destroy_tcp_c2c(net_sockhnd_t sockhnd)
{
  int rc = NET_ERR;
  net_sock_ctxt_t *sock = (net_sock_ctxt_t * ) sockhnd;
  net_ctxt_t *ctxt = sock->net;
    
  /* Find the parent in the linked list.
   * Unlink and free. 
   */
  if (sock == ctxt->sock_list)
  {
    ctxt->sock_list = sock->next;
    rc = NET_OK;
  }
  else
  {
    net_sock_ctxt_t *cur = ctxt->sock_list;
    do
    {
      if (cur->next == sock)
      {
        cur->next = sock->next;
        rc = NET_OK;
        break;
      }
      cur = cur->next;
    } while(cur->next != NULL);
  }
  if (rc == NET_OK)
  {
    net_free(sock);
  }
  
  return rc;
}

#endif /* USE_C2C */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/