/**
  @page Vibration analysis application on IBM Watson Cloud

  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @author  MCD Application Team
 * @version	3.0.0
 * @date	20-July-2018
  * @brief   Description of BlueMix Cloud application.
  ******************************************************************************
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. All rights reserved.
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
  @endverbatim

@par Application Description

The sample application implements vibration analysis demonstration on IBM Bluemix Cloud IoT client for following hardware configuration:

NUCLEO-F429ZI + X-NUCLEO-IKS01A2 + X-NUCLEO-NFC04A1 (OPTIONAL TO USE)

2 ways of connecting to the IBM IoT platform are proposed:
* Quickstart registration: no account nor token are required, the connection is unsecured.
* Simple registration: an IBM IoT account is required, the device is authenticated by a token,
  the connection is secured using mbedTLS
  
This application is ported to the NUCLEO-F429ZI and connect with IBM 
Watson IoT Cloud via ethernet port present on NUCLEO-F429ZI.


@par Hardware and Software environment

  - MCU board: Either NUCLEO-F429ZI

  - Ethernet Port access for Internet connectivity

  - Bluemix IoT account
      * An IBM Bluemix account is required to use an IBM Watson IoT platform application
        or to use the simple way of registering the device.
        An IBM Bluemix account is not required to use the Quickstart way of registering the device.
        To register, go to this url: https://console.bluemix.net/registration/
        You will receive an email asking to confirm the account creation

  - An IBM Watson IoT platform application
      * This provides a way to interact from the cloud to the device and is not required for the
        device to publish data

  - A development PC for building the application, programming through ST-Link, and running the virtual console.

@par How to use it ?

In order to make the program work, you must follow these steps:

Device creation (for the simple registering)
  - Follow the steps as indicated at: https://developer.ibm.com/recipes/tutorials/how-to-register-devices-in-ibm-iot-foundation/


Application build and flash
  
  - WARNING: before opening the project with any toolchain be sure your folder
    installation path is not too in-depth since the toolchain may report errors
    after building.

  - Open and build the project with one of the supported development toolchains (see the release note
    for detailed information about the version requirements).

  - Program the firmware on the STM32 board: you can copy (or drag and drop) the
    binary file under Projects\STM32F429ZI-Nucleo\Applications\Cloud\Bluemix\Binary
    to the USB mass storage location created when you plug the STM32
    board to your PC. If the host is a Linux PC, the STM32 device can be found in
    the /media folder with the name "DIS_F429". For example, if the created mass
    storage location is "/media/DIS_F429", then the command to program the board
    with a binary file named "my_firmware.bin" is simply: cp my_firmware.bin
    /media/DIS_F429.

  Alternatively, you can program the STM32 board directly through one of the supported development toolchains.


Application first launch
  - The board must be connected to a PC through USB (ST-LINK USB port).
    Open the console through serial terminal emulator (e.g. TeraTerm), select the ST-LINK COM port of your
    board and configure it with:
    - 8N1, 115200 bauds, no HW flow control;
    - line endings set to LF.

    - Set the TLS root CA certificates:
      Copy-paste the content of Projects\Common\Bluemix\comodo_bluemix.pem.
      The device uses it to authenticate the remote hosts through TLS.

      Note: The sample application requires that a concatenation of 2 CA certificates is provided
          1) for the HTTPS server which is used to retrieve the current time and date at boot time.
             (the "Comodo" certificate).
          2) for the IBM IoT Platform. This one is used with the simple way of registering not with the quickstart way.
  
  - Select the way of registering between "quickstart" and "simple" and enter the connection string as prompted.
  
  - After the parameters are configured, it is possible to change them by restarting the board
    and pushing the User button (blue button) when prompted.


Application runtime
  - Makes an HTTPS request to retrieve the current time and date, and configures the RTC.

      Note: HTTPS has the advantage over NTP that the server can be authentified by the board, preventing
            a possible man-in-the-middle attack.
            However, the first time the board is switched on (and each time it is powered down and up, if the RTC
            is not backed up), the verification of the server certificate will fail as its validity period will
            not match the RTC value.
            The following log will be printed to the console: "x509_verify_cert() returned -9984 (-0x2700)"

            If the CLOUD_TIMEDATE_TLS_VERIFICATION_IGNORE switch is defined in cloud.c (it is the case by default),
              this error is ignored and the RTC is updated from the "Date:" field of the HTTP response header.
            Else, a few more error log lines are printed, and the application retries to connect without verifying
              the server certificate.
              If/once ok, the RTC is updated.
            
  - Device to cloud connection: the device connects to the IBM Bluemix platform
      The application publishes the sensor values, FFT max frequency, FFT max frequency amplitude, and a timestamp:
       - once if the user button is pushed shortly
       - around every second or paused if the user button is double pushed
      
      The user can see the reported value at the url as indicated in the serial terminal emulator
      At https://quickstart.internetofthings.ibmcloud.com with quickstart or
      at https://<Organisation Id>.internetofthings.ibmcloud.com with the simple registration mode
      The device Id to use is also given in the serial terminal emulator

  - Cloud to device connection: in simple register mode only, our device is a managed device and accepts the "Reboot" command.
    
@par Caveats

  - The mbedTLS configuration parameter MBEDTLS_SSL_MAX_CONTENT_LEN is tailored down to 8 kbytes.
    It is sufficient for connecting to the IBM Bluemix IoT cloud, and to the HTTPS server used for retrieveing
    the time and date at boot time.
    But the TLS standard may require up to 16kbytes, depending on the server configuration.
    For instance, if the server certificate is 10 kbytes large, it will not fit in the device buffer,
    the TLS handshake will fail, and the TLS connection will not be possible.

  - If you face a connection issue in simple registered mode, take care that your security policy which is accessible
    from the IBM Watson IoT Platform dashboard is set to either "TLS Optional" or "TLS with Token Authentification"

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
