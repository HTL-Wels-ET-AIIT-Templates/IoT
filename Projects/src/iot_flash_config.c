/**
  ******************************************************************************
  * @file    iot_flash_config.c
  * @author  MCD Application Team
 * @version	3.0.0
 * @date	20-July-2018
  * @brief   configuration in flash memory.
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
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "main.h"
#ifdef USE_FIREWALL
#include "firewall_wrapper.h"
#endif
#include "flash.h"
#include "iot_flash_config.h"
#include "msg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define PEM_READ_LINE_SIZE    120
#define PEM_READ_BUFFER_SIZE  8192  /**< Max size which can be got from the terminal in a single getInputString(). */

/* Private macros ------------------------------------------------------------*/
#ifdef ENABLE_TRACE_FLASH
#define TRACE_FLASH msg_info
#else
#define TRACE_FLASH(...)
#endif

/* Private variables ---------------------------------------------------------*/
/** Do not zero-initialize the static user configuration.
 *  Otherwise, it must be entered manually each time the device FW is updated by STLink.
 */

#ifdef __ICCARM__  /* IAR */
__no_init const user_config_t lUserConfig @ "UNINIT_FIXED_LOC";
#elif defined ( __CC_ARM   )/* Keil / armcc */
user_config_t lUserConfig __attribute__((section("UNINIT_FIXED_LOC"), zero_init));
#elif defined ( __GNUC__ )      /*GNU Compiler */
user_config_t lUserConfig __attribute__((section("UNINIT_FIXED_LOC")));
#endif

/* Private function prototypes -----------------------------------------------*/
int CaptureAndFlashPem(char *pem_name, char const *flash_addr, bool restricted_area);
char pemString[] = "-----BEGIN CERTIFICATE-----\r\n"\
		"MIIF2DCCA8CgAwIBAgIQTKr5yttjb+Af907YWwOGnTANBgkqhkiG9w0BAQwFADCB\r\n"\
		"hTELMAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4G\r\n"\
		"A1UEBxMHU2FsZm9yZDEaMBgGA1UEChMRQ09NT0RPIENBIExpbWl0ZWQxKzApBgNV\r\n"\
		"BAMTIkNPTU9ETyBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAwMTE5\r\n"\
		"MDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBhTELMAkGA1UEBhMCR0IxGzAZBgNVBAgT\r\n"\
		"EkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UEBxMHU2FsZm9yZDEaMBgGA1UEChMR\r\n"\
		"Q09NT0RPIENBIExpbWl0ZWQxKzApBgNVBAMTIkNPTU9ETyBSU0EgQ2VydGlmaWNh\r\n"\
		"dGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQCR\r\n"\
		"6FSS0gpWsawNJN3Fz0RndJkrN6N9I3AAcbxT38T6KhKPS38QVr2fcHK3YX/JSw8X\r\n"\
		"pz3jsARh7v8Rl8f0hj4K+j5c+ZPmNHrZFGvnnLOFoIJ6dq9xkNfs/Q36nGz637CC\r\n"\
		"9BR++b7Epi9Pf5l/tfxnQ3K9DADWietrLNPtj5gcFKt+5eNu/Nio5JIk2kNrYrhV\r\n"\
		"/erBvGy2i/MOjZrkm2xpmfh4SDBF1a3hDTxFYPwyllEnvGfDyi62a+pGx8cgoLEf\r\n"\
		"Zd5ICLqkTqnyg0Y3hOvozIFIQ2dOciqbXL1MGyiKXCJ7tKuY2e7gUYPDCUZObT6Z\r\n"\
		"+pUX2nwzV0E8jVHtC7ZcryxjGt9XyD+86V3Em69FmeKjWiS0uqlWPc9vqv9JWL7w\r\n"\
		"qP/0uK3pN/u6uPQLOvnoQ0IeidiEyxPx2bvhiWC4jChWrBQdnArncevPDt09qZah\r\n"\
		"SL0896+1DSJMwBGB7FY79tOi4lu3sgQiUpWAk2nojkxl8ZEDLXB0AuqLZxUpaVIC\r\n"\
		"u9ffUGpVRr+goyhhf3DQw6KqLCGqR84onAZFdr+CGCe01a60y1Dma/RMhnEw6abf\r\n"\
		"Fobg2P9A3fvQQoh/ozM6LlweQRGBY84YcWsr7KaKtzFcOmpH4MN5WdYgGq/yapiq\r\n"\
		"crxXStJLnbsQ/LBMQeXtHT1eKJ2czL+zUdqnR+WEUwIDAQABo0IwQDAdBgNVHQ4E\r\n"\
		"FgQUu69+Aj36pvE8hI6t7jiY7NkyMtQwDgYDVR0PAQH/BAQDAgEGMA8GA1UdEwEB\r\n"\
		"/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAArx1UaEt65Ru2yyTUEUAJNMnMvl\r\n"\
		"wFTPoCWOAvn9sKIN9SCYPBMtrFaisNZ+EZLpLrqeLppysb0ZRGxhNaKatBYSaVqM\r\n"\
		"4dc+pBroLwP0rmEdEBsqpIt6xf4FpuHA1sj+nq6PK7o9mfjYcwlYRm6mnPTXJ9OV\r\n"\
		"2jeDchzTc+CiR5kDOF3VSXkAKRzH7JsgHAckaVd4sjn8OoSgtZx8jb8uk2Intzna\r\n"\
		"FxiuvTwJaP+EmzzV1gsD41eeFPfR60/IvYcjt7ZJQ3mFXLrrkguhxuhoqEwWsRqZ\r\n"\
		"CuhTLJK7oQkYdQxlqHvLI7cawiiFwxv/0Cti76R7CZGYZ4wUAc1oBmpjIXUDgIiK\r\n"\
		"boHGhfKppC3n9KUkEEeDys30jXlYsQab5xoq2Z0B15R97QNKyvDb6KkBPvVWmcke\r\n"\
		"jkk9u+UJueBPSZI9FoJAzMxZxuY67RIuaTxslbH9qh17f4a+Hg4yRvv7E491f0yL\r\n"\
		"S0Zj/gA0QHDBw7mh3aZw4gSzQbzpgJHqZJx64SIDqZxubw5lT2yHh17zbqD5daWb\r\n"\
		"QOhTsiedSrnAdyGN/4fy3ryM7xfft0kL0fJuMAsaDk527RH89elWsn2/x20Kk4yl\r\n"\
		"0MC2Hb46TpSi125sC8KKfPog88Tk5c0NqMuRkrF8hey1FGlmDoLnzc7ILaZRfyHB\r\n"\
		"NVOFBkpdn627G190\r\n"\
		"-----END CERTIFICATE-----\r\n"\
		"-----BEGIN CERTIFICATE-----\r\n"\
		"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\r\n"\
		"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\r\n"\
		"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\r\n"\
		"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\r\n"\
		"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\r\n"\
		"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\r\n"\
		"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\r\n"\
		"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\r\n"\
		"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\r\n"\
		"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\r\n"\
		"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\r\n"\
		"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\r\n"\
		"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\r\n"\
		"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\r\n"\
		"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\r\n"\
		"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\r\n"\
		"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\r\n"\
		"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\r\n"\
		"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\r\n"\
		"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\r\n"\
		"-----END CERTIFICATE-----\r\n"
;



/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Get a line from the console (user input).
  * @param  Out:  inputString   Pointer to buffer for input line.
  * @param  In:   len           Max length for line.
  * @retval Number of bytes read from the terminal.
  */
int getInputString(char *inputString, size_t len)
{
  size_t currLen = 0;
  int c = 0;
  
  c = getchar();
  
  while ((c != EOF) && ((currLen + 1) < len) && (c != '\r') && (c != '\n') )
  {
    if (c == '\b')
    {
      if (currLen != 0)
      {
        --currLen;
        inputString[currLen] = 0;
        printf(" \b");
      }
    }
    else
    {
      if (currLen < (len-1))
      {
        inputString[currLen] = c;
      }
      
      ++currLen;
    }
    c = getchar();
  }
  if (currLen != 0)
  { /* Close the string in the input buffer... only if a string was written to it. */
    inputString[currLen] = '\0';
  }
  if (c == '\r')
  {
    c = getchar(); /* assume there is '\n' after '\r'. Just discard it. */
  }
  
  return currLen;
}

/**
  * @brief  Get one PEM string (ASCII format of TLS certificates and keys) from the console (user input).
  * @param  Out: key_read_buffer    Destimatation buffer.
  * @param  In:  max_len            Maximum length to be written to the destination buffer.
  * @retval Number of characters read into the output buffer.
  */
int enterPemString(char * read_buffer, size_t max_len)
{
  int i = 0;
  int read_len = 0;
  bool eof = false;
  read_len = getInputString(&read_buffer[i], max_len);
  
  while ( (read_len >= 0) && (i < max_len) && !eof )
  {
    i += read_len;
    read_buffer[i++] = '\n';
    read_len = getInputString(&read_buffer[i], max_len);
    eof = (strncmp(&read_buffer[i], "-----END",8) == 0);   
    if (eof)
    {
        i += read_len;
        read_buffer[i++] = '\n';
        read_len = getInputString(&read_buffer[i], max_len);
        if (read_len != 0) eof =false;
    }
  }
  
  if (i >= max_len)
  {
   msg_error("Certificate is too long , allocated size is %d\n\r",max_len);
   return 0;
  }
  read_buffer[++i] = '\0';
  return i;
}

  
/**
  * @brief  Ask user and write the TLS certificates and key to the FLASH memory.
  * @param  none
  * @retval Error code
  *             0    Success
  *             <0   Unrecoverable error
  */
  
 int CaptureAndFlashPem(char *pem_name, char const *flash_addr, bool restricted_area)
 {
  char * key_read_buffer = NULL;
  int    ret = 0;
  key_read_buffer = malloc(PEM_READ_BUFFER_SIZE);
  if (key_read_buffer == NULL)
  {
    msg_error("Could not allocate %d bytes for the console readbuffer.\n\r", PEM_READ_BUFFER_SIZE);
    return -1;
  }

  memset(key_read_buffer, 0, PEM_READ_BUFFER_SIZE);
//  printf("\nEnter your %s: \n\r",pem_name);
//  enterPemString(key_read_buffer, PEM_READ_BUFFER_SIZE);
  strcpy(key_read_buffer, pemString);

  //msg_info("read: --->\n%s\n<---\n\r", key_read_buffer);
  
  /* Write to FLASH. */
  TRACE_FLASH("writing to %lx\n\r", flash_addr);
  
#ifdef FIREWALL_MBEDLIB
  if (restricted_area)
  {
    ret = FLASH_firewall_update((uint32_t)flash_addr, key_read_buffer, strlen(key_read_buffer) + 1);  /* Append the closing \0*/
  }
  else
#endif
  {
    //printf("pem name=%s, length to update in flash=%d\n\r", pem_name, strlen(key_read_buffer) + 1);
    ret = FLASH_update((uint32_t)flash_addr, key_read_buffer, strlen(key_read_buffer) + 1);  /* Append the closing \0*/
  }
  
  free(key_read_buffer);
  
  return ret;
 }
   

int updateTLSCredentials(void)
{
  int ret = 0;
  
  printf("\nUpdating TLS security credentials.\n\r");
//  printf("\nEnter the x509 certificates or keys as per the following format:\n\r");
//  printf("-----BEGIN CERTIFICATE-----\n\r");
//  printf("YMPGn8u67GB9t+aEMr5P+1gmIgNb1LTV+/Xjli5wwOQuvfwu7uJBVcA0Ln0kcmnL\n\r");
//  printf("R7EUQIN9Z/SG9jGr8XmksrUuEvmEF/Bibyc+E1ixVA0hmnM3oTDPb5Lc9un8rNsu\n\r");
//    printf(".......\n\r");
//  printf("-----END CERTIFICATE-----\n\r");
//  printf("-----BEGIN CERTIFICATE-----\n\r");
//  printf("YMPGn8u67GB9t+aEMr5P+1gmIgNb1LTV+/Xjli5wwOQuvfwu7uJBVcA0Ln0kcmnL\n\r");
//  printf(".......\n\r");
//  printf("-----END CERTIFICATE-----\n\r");
//  printf("\\n.......\n\r");
    
//  if ( (checkTLSRootCA() == -1)
//      || check("Do you want to update the root CA certificate(s)? [y/n]\n\r") )
  {
    ret = CaptureAndFlashPem("root CA",lUserConfig.tls_root_ca_cert, false);
    if (ret == 0)
    {
      uint64_t magic = USER_CONF_MAGIC;
      ret = FLASH_update((uint32_t)&lUserConfig.ca_tls_magic, &magic, sizeof(uint64_t));
    }
  } 
  
#if defined(AWS)
  if (ret == 0)
  {
    if (checkTLSDevice() == -1)
    {
      ret |= CaptureAndFlashPem("device certificate",lUserConfig.tls_device_cert, false);
      ret |= CaptureAndFlashPem("device key",lUserConfig.tls_device_key, true);
    }
    else
    {
      if (check("Do you want to update the device certificate? [y/n]\n\r"))
      {
        ret |= CaptureAndFlashPem("device certificate",lUserConfig.tls_device_cert, false);
      }
      
      if (check("Do you want to update the device key? [y/n]\n\r"))
      {
        ret |= CaptureAndFlashPem("device key",lUserConfig.tls_device_key, false);
      }
    }
    
    if (ret == 0)
    {
      uint64_t magic = USER_CONF_MAGIC;
      ret = FLASH_update((uint32_t)&lUserConfig.device_tls_magic, &magic, sizeof(uint64_t));
    }
  }
#endif  /* AWS */
  
  if (ret < 0)
  {
    msg_error("Failed updating the TLS configuration in FLASH.\n\r");
  }
  
  return ret;
}


/**
  * @brief  Check if TLS root CA certificates are present in FLASH memory.
  * @param  void
  * @retval 0 Configured,
           -1 Not configured.
  */
int checkTLSRootCA()
{
  return (lUserConfig.ca_tls_magic == USER_CONF_MAGIC) ? 0 : -1;
}

/**
  * @brief  Check if the device TLS credentials (device certificate and device private key)
            are present in FLASH memory.
  * @param  void
  * @retval 0 Configured,
           -1 Not configured.
  */
int checkTLSDevice()
{
  return (lUserConfig.device_tls_magic == USER_CONF_MAGIC) ? 0 : -1;
}

/**
  * @brief  Get the TLS cerificates and private key addresses in the FLASH memory.
  * @param  Out: ca_cert        CA certificate / trust chain (PEM format: string)
  * @param  Out: device_cert    Device certificate (PEM format: string)
  * @param  Out: private_key    Device private key (PEM format: string)
  * @retval 0:    TLS credentials found, and passed back to the caller.
  *        -1:    TLS credentials not found.
  */
int getTLSKeys(const char ** const root_ca_cert, const char ** const device_cert, const char ** const private_key)
{
  int rc = -1;
  if ( (lUserConfig.ca_tls_magic == USER_CONF_MAGIC)
#ifdef AWS
        && (lUserConfig.device_tls_magic == USER_CONF_MAGIC)
#endif
     )
  {
    if (root_ca_cert != NULL)    *root_ca_cert = lUserConfig.tls_root_ca_cert;
    if (device_cert !=NULL)      *device_cert = lUserConfig.tls_device_cert;
    if (private_key!= NULL)      *private_key = lUserConfig.tls_device_key;
    rc = 0;
  }
  else
  {
    if (root_ca_cert != NULL)    *root_ca_cert = NULL;
    if (device_cert !=NULL)      *device_cert = NULL;
    if (private_key!= NULL)      *private_key = NULL;
  }
  return rc;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
