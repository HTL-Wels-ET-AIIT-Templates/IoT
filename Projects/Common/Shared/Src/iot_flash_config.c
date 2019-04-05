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
#include "rfu.h"
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
bool check(char *s);
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
static char MyDeviceName[] = "DeviceType=doesnotmatter;DeviceId=";
extern char gMyDeviceId[];


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
  * @brief  Check whether the C2C parameters are present in FLASH memory.
  *         Returns the parameters if present.
  * @param  Out:  oper_ap_code       oper_ap_code  SIM operator Acces Point code 
  * @param  Out:  username           username
  * @param  Out:  password           password
  * @retval   0 if the parameters are present in FLASH.
  *          -1 if the parameters are not present in FLASH.
  */
int checkC2cCredentials(const char ** const oper_ap_code, const char ** const username, const char ** const password)
{
  bool is_soapc_present = 0;
  
  if (lUserConfig.c2c_config.magic == USER_CONF_MAGIC)
  {
    is_soapc_present = true;
    if (oper_ap_code == NULL)
    {
      return -2;
    }
    *oper_ap_code = lUserConfig.c2c_config.oper_ap_code;
    *username = lUserConfig.c2c_config.username;
    *password = lUserConfig.c2c_config.password;
  }
 
  return (is_soapc_present) ? 0 : -1;
}


/**
  * @brief  Write the C2C parameters to the FLASH memory.
  * @param  In:   oper_ap_code  SIM operator Acces Point code e.g "EM" for Emnify, "ESEYE1" for Eseye, etc.
  * @param  In:   username        
  * @param  In:   password      
  * @retval Error code
  *             0    Success
  *             <0   Unrecoverable error
  */
int updateC2cCredentials(const char ** const oper_ap_code, const char ** const username, const char ** const password)
{
  c2c_config_t c2c_config;
  int ret = 0;

  if ((oper_ap_code == NULL) ||(username == NULL) || (password == NULL))
  {
    return -1;
  }
  
  memset(&c2c_config, 0, sizeof(c2c_config_t));
    
  msg_info("\nEnter Sim Operator Access Point Code (e.g. EM or ESEYE1 etc): ");
  getInputString(c2c_config.oper_ap_code, USER_CONF_C2C_SOAPC_MAX_LENGTH);
  msg_info("You have entered <%s> as the Sim Operator Access Point Code.\n\r", c2c_config.oper_ap_code);

  msg_info("\nEnter the username (it can be NULL) (max 16 char):  ");
  getInputString(c2c_config.username, USER_CONF_C2C_USERID_MAX_LENGTH);
  msg_info("You have entered <%s> as the username.\n\r", c2c_config.username);
  
  msg_info("\nEnter the password (it can be NULL) (max 16 char):  ");
  getInputString(c2c_config.password, USER_CONF_C2C_PSW_MAX_LENGTH);
  msg_info("You have entered <%s> as the password.\n\r", c2c_config.password);
  
  c2c_config.magic = USER_CONF_MAGIC;

  ret = FLASH_update((uint32_t)&lUserConfig.c2c_config, &c2c_config, sizeof(c2c_config_t));

  if (ret < 0)
  {
    msg_error("Failed updating the C2C configuration in FLASH.\n\r");
  }

  msg_info("\n\r");
  return ret;
}


/**
  * @brief  Check whether the Wifi parameters are present in FLASH memory.
  *         Returns the parameters if present.
  * @param  Out:  ssid              Wifi SSID.
  * @param  Out:  psk               Wifi security password.
  * @param  Out:  security_mode     See @ref wifi_network_security_t definition.
  * @retval   0 if the parameters are present in FLASH.
  *          -1 if the parameters are not present in FLASH.
  */
int checkWiFiCredentials(const char ** const ssid, const char ** const psk, uint8_t * const security_mode)
{
  bool is_ssid_present = 0;
  
  if (lUserConfig.wifi_config.magic == USER_CONF_MAGIC)
  {
    is_ssid_present = true;
    if ((ssid == NULL) ||(psk == NULL) || (security_mode == NULL))
    {
      return -2;
    }
    *ssid = lUserConfig.wifi_config.ssid;
    *psk = lUserConfig.wifi_config.psk;
    *security_mode = lUserConfig.wifi_config.security_mode;
  }
 
  return (is_ssid_present) ? 0 : -1;
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
  * @brief  Ask user and write in FLASH memory the MQTT broker address (AWS endpoint),
  *          and the device name.
  * @retval Error code
  *             0    Success
  *             <0   Unrecoverable error
  */
int AWSServerDeviceConfig(void)
{
  iot_config_t iot_config;
  int ret = 0;

  memset(&iot_config, 0, sizeof(iot_config_t));
    
  printf("\nEnter server address: (example: xxx.iot.region.amazonaws.com) \n\r");
  getInputString(iot_config.server_name, USER_CONF_SERVER_NAME_LENGTH);
  msg_info("read: --->\n%s\n<---\n\r", iot_config.server_name);
  
  printf("\nEnter device name: (example: mything1) \n\r");
  getInputString(iot_config.device_name, USER_CONF_DEVICE_NAME_LENGTH);
  msg_info("read: --->\n%s\n<---\n\r", iot_config.device_name);

  iot_config.magic = USER_CONF_MAGIC;
   
  ret = FLASH_update((uint32_t)&lUserConfig.iot_config, &iot_config, sizeof(iot_config_t));
  
  if (ret < 0)
  {
    msg_error("Failed programming the IOT server / device config into Flash.\n\r");
  }
  return ret;
}


/**
  * @brief  Ask user and write in FLASH memory the Azure connection string.
  * @retval    Error code
  *             0    Success
  *             <0   Unrecoverable error
  */
int AzureDeviceConfig(void)
{
    iot_config_t iot_config;
    int ret = 0;

    memset(&iot_config, 0, sizeof(iot_config_t));
      
    printf("\nEnter the Azure connection string of your device: (template: HostName=xxx;DeviceId=xxx;SharedAccessKey=xxxx=) \n\r");
    getInputString(iot_config.device_name, USER_CONF_DEVICE_NAME_LENGTH);
    msg_info("read: --->\n%s\n<---\n\r", iot_config.device_name);

    iot_config.magic = USER_CONF_MAGIC;
     
    ret = FLASH_update((uint32_t)&lUserConfig.iot_config, &iot_config, sizeof(iot_config_t));
    
    if (ret != 0)
    {
      msg_error("Error: Failed programming the IOT server / device config into FLASH.\n\r");
      return -1;
    }
   
  return 0;
}


/**
  * @brief  Ask user and write in FLASH memory the Bluemix registration mode and the connection string
  * @retval    Error code
  *             0    Success
  *             <0   Unrecoverable error
  */
int BluemixDeviceConfig(void)
{        
  iot_config_t iot_config;
  int ret = 0;
  char c;
  int n=0;
  
  memset(&iot_config, 0, sizeof(iot_config_t));
    
  msg_info("\n\r");
    
//    do
//    {
//      printf("\rEnter Registration Mode (1 - Quickstart, 2 - Simple): \n\r");
//      c = getchar();
//    }
//    while ( (c < '1')  || (c > '2'));

    // Hardcode setting to Quickstart
  	c = '1';
    
    switch (c)
    {
      case '1':
        strcpy(iot_config.device_name, QUICK_START_REG_NAME);
        strcat(iot_config.device_name, ";");
        msg_info("\nYou have selected the Quickstart registration mode.\n\r");
//        msg_info("\nEnter the Bluemix connection string of your device: (template: %s=xxx;%s=xxx) \n\r",DEVICE_TYPE_KEY,DEVICE_ID_KEY);
        break;
          
      case '2':
        strcpy(iot_config.device_name, SIMPLE_REG_NAME);
        strcat(iot_config.device_name, ";");
        msg_info("\nYou have selected the Simple registration mode.\n\r");
        msg_info("\nEnter the Bluemix connection string of your device: (template: %s=xxx;%s=xxx;%s=xxx;%s=xxx) \n\r",ORG_ID_KEY,DEVICE_TYPE_KEY,DEVICE_ID_KEY,TOKEN_KEY);
        break;
          
      default:
        ret = -1;
        return ret;
    }
        
    n=strlen(iot_config.device_name);
    // Prepare Device Identification from user strings
//    getInputString(iot_config.device_name+n, USER_CONF_DEVICE_NAME_LENGTH-n);
    strcpy(iot_config.device_name + n, MyDeviceName);
    n=strlen(iot_config.device_name);
    strcpy(iot_config.device_name + n, gMyDeviceId);
//#if (!defined STM32L475xx)
//  }
//#endif // STM32L475xx
  msg_info("connection string: --->%s<---\n\r", iot_config.device_name);
  
  iot_config.magic = USER_CONF_MAGIC;
     
  ret = FLASH_update((uint32_t)&lUserConfig.iot_config, &iot_config, sizeof(iot_config_t));
    
  if (ret != 0)
  {
    msg_error("Error: Failed programming the IOT server / device config into FLASH.\n\r");
    return -1;
  }
  
  return ret;
}
  
/**
  * @brief  Ask user and write in FLASH memory the Verizon device name.
  * @retval Error code
  *         0  Success
  *         <0 Unrecoverable error
  */
int VerizonDeviceConfig(void)
{
  iot_config_t iot_config;
  int ret = 0;

  memset(&iot_config, 0, sizeof(iot_config_t));

  printf("\nEnter device name: (example: mything1) \n\r");
  getInputString(iot_config.device_name, USER_CONF_DEVICE_NAME_LENGTH);
  msg_info("read: --->\n%s\n<---\n\r", iot_config.device_name);

  iot_config.magic = USER_CONF_MAGIC;

  ret = FLASH_update((uint32_t)&lUserConfig.iot_config, &iot_config, sizeof(iot_config_t));
  if (ret != 0)
  {
    msg_error("Error: Failed programming the IOT server / device config into FLASH.\n\r");
    return -1;
  }

  return 0;
}


/**
  * @brief  Get the MQTT broker address (AWS endpoint) from FLASH memory.
  * @param  Out:  address   Pointer to location of the server address.
  * @retval  0  Success:  The server address is configured and returned to the caller.
  *         -1  Error:    No server address is configured.
  */
int getServerAddress(const char ** const address)
{
  int ret = -1;
  
  if (address != NULL)
  {
    if (lUserConfig.iot_config.magic == USER_CONF_MAGIC)
    {
      *address = lUserConfig.iot_config.server_name;
      ret = 0;
    } else {
      *address = NULL;
    }
  }
  return ret;
}


/**
  * @brief  Get the device name from FLASH memory.
  * @param  Out:  name    Pointer to location of the device name.
  * @retval   0   Success:  The device name is configured and returned to the caller.
  *          -1   Error:    No device name is configured.
  
  */
int getDeviceName(const char ** const name)
{
  int ret = -1;

  if (name != NULL)
  {
    if (lUserConfig.iot_config.magic == USER_CONF_MAGIC)
    {
      *name = lUserConfig.iot_config.device_name;
      ret = 0;
    } else {
      *name = NULL;
    }
  }
  return ret;
}

/**
  * @brief  Check if the MQTT broker address and the device name are present in FLASH memory.
  * @retval 0:  The server name and the device name are configured.
  *        -1:  The server name and the device name are not configured.
  */
int checkServerDevice()
{
  return (lUserConfig.iot_config.magic == USER_CONF_MAGIC) ? 0 : -1;
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

#ifdef RFU
/**
  * @brief  Firmware version management dialog.
  *         Allows:
  *             - Selecting a different FW version for the next boot, if already programmed in the other FLASH bank.
  *             - Download a FW file from an HTTP URL and program it to the other FLASH bank.
  */
int updateFirmwareVersion()
{
  char console_yn = 'n';
  const firmware_version_t  * fw_version;
  uint32_t cur_bank = FLASH_get_bank(FLASH_BASE);
  uint32_t alt_bank = (cur_bank == FLASH_BANK_1) ? FLASH_BANK_2 : FLASH_BANK_1;
  
  rfu_getVersion(&fw_version, false);
  printf("\n*** Firmware version management ***\n\r");
  printf("\nPress the BLUE user button within the next 5 seconds\nto change the firmware version from %s %d.%d.%d %s %s, running from bank #%lu.\n\r",
         fw_version->name, fw_version->major, fw_version->minor, fw_version->patch, fw_version->build_date, fw_version->build_time, cur_bank);
  printf("\n\r");

  if (Button_WaitForPush(5000))
  {
    printf("Current FW Version: %s %d.%d.%d %s %s, running from bank #%lu\n\r", fw_version->name, fw_version->major, fw_version->minor, fw_version->patch, fw_version->build_date, fw_version->build_time, cur_bank);

    /* Check whether an alternative firmware version is available in FLASH memory. */
    uint32_t *sp_candidate = (uint32_t *) (FLASH_BASE + FLASH_BANK_SIZE);
    if ((*sp_candidate < SRAM1_BASE) || (*sp_candidate > (SRAM1_BASE + SRAM1_SIZE_MAX)))
    { /* No alternative. */
      printf("Warning: The FLASH bank #%lu bank does not contain a valid boot image. Bank %lu will keep being used at next reset.\n\r", alt_bank, cur_bank);
    }
    else
    { /* An alternative exists. */
      firmware_version_t alt_fw_version = { "", 0, 0, 0, "", "" };
      const firmware_version_t *p_alt_fw_version;
      rfu_getVersion(&p_alt_fw_version, true);
      /* Guard the version strings against a read overflow in case the alt FLASH bank is inconsistent. */
      memcpy(&alt_fw_version, p_alt_fw_version, sizeof(firmware_version_t));
      alt_fw_version.build_date[FWVERSION_DATE_SIZE-1] = '\0';
      alt_fw_version.build_time[FWVERSION_TIME_SIZE-1] = '\0';
      
      printf("Altern. FW Version: %s %d.%d.%d %s %s, available in bank #%lu\n\r",
             alt_fw_version.name,
             alt_fw_version.major,
             alt_fw_version.minor,
             alt_fw_version.patch,
             alt_fw_version.build_date,
             alt_fw_version.build_time, alt_bank);
   
     do
     {
       printf("\rDo you want to switch to the alternative firmware version? (You will have to reset the board.) (y/n)  \b");
       console_yn = getchar();
     }
     while((console_yn != 'y') && (console_yn != 'n') && (console_yn != '\n'));
     if (console_yn != '\n') printf("\n\r");

     if (console_yn == 'y')
      {
        if (0 == FLASH_set_boot_bank(FLASH_BANK_BOTH))
        {
          printf("Befault boot bank switched successfully.\n\r");
          return 0;
        }
        else
        {
          printf("Error: failed changing the boot configuration\n\r");
          return -1;
        }
      }
    }
    
    do
    {
      printf("\rDo you want to download and program a new firmare version into FLASH bank #%lu? (y/n) \b", alt_bank);
      console_yn = getchar();
    }
    while((console_yn != 'y') && (console_yn != 'n') && (console_yn != '\n'));
    if (console_yn != '\n') printf("\n\r");
   
    if (console_yn != 'y')
    {
      return 0;
    }

#define DEFAULT_FW_URL      "http://192.168.3.100:1080/B-L475E-IOT01_Cloud_AWS.sim"
#define MAX_FW_URL_LENGTH   100
    char fw_url[MAX_FW_URL_LENGTH];
    strncpy(fw_url, DEFAULT_FW_URL, sizeof(fw_url));

    printf("\nEnter the URL of the new firmware file:(By default: %s) :", fw_url);
    getInputString(fw_url, sizeof(fw_url));
    msg_info("read: --->\n%s\n<---\n\r", fw_url);
    
    printf("Downloading and programming the new firmware into the alternate FLASH bank.\n\r");
    
    int ret = rfu_update(fw_url);
    switch (ret)
    {
      case RFU_OK:
        printf("\nProgramming done. Now you can reset the board.\n\n\r");
        break;
      case RFU_ERR_HTTP:
        printf("\nError: Programming failed. Reason: HTTP error - check your network connection, "
               "and that the HTTP server supports HTTP/1.1 and the progressive download.\n\n\r");
        break;
      case RFU_ERR_FF:
        printf("\nError: Programming failed. Reason: Invalid firmware fileformat - check that the IAR simple-code format is used.\n\n\r");
        break;
      case RFU_ERR_FLASH:
        printf("\nError: Programming failed. Reason: FLASH memory erase/write - check that the firmware file matches the SoC FLASH memory mapping"
               "and write protection settings. Double check that there is no illegal write to the FLASH address range.\n\n\r");
        break;
      default:
        printf("\nError: Programming failed. Unknown reason.\n\n\r");
    }
  }
  
  return 0;
}
#endif /* RFU support */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
