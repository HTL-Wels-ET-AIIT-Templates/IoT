/*******************************************************************************
 * Copyright (c) 2015, 2016 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Jeffrey Dare            - initial implementation and API implementation
 *    Sathiskumar Palaniappan - Added support to create multiple Iotfclient
 *                              instances within a single process
 *    Lokesh Haralakatta      - Added SSL/TLS support
 *    Lokesh Haralakatta      - Added Client Side Certificates support
 *    Lokesh Haralakatta      - Separated out device client and gateway client specific code.
 *                            - Retained back common code here.
 *    Lokesh Haralakatta      - Added Logging Feature
 *******************************************************************************/

#include <iotclient.h>
#include "ctype.h"
#include <msg.h>

#define CFG_STRING_MAX 30

unsigned short keepAliveInterval = 360;
int sleep(int);

IotDevice_t iotDevice;

/**
 * Function used to initialize the Watson IoT client

 * @param domain - Your domain Name
 * @param port - TCP port
 * @param devId - device ID
 *
 * @return int return code
 */
int initialize(char* domainName, uint16_t port, char *devId)
{
	int rc = 0;

	if(domainName == NULL || port == 0) {
		rc = MISSING_INPUT_PARAM;
		goto exit;
	}

	strcpy(iotDevice.deviceClient.cfg.domain, domainName);
	strcpy(iotDevice.deviceClient.cfg.id, devId);
	iotDevice.deviceClient.cfg.port = port;

	strcpy(iotDevice.deviceInfo.serialNumber, "XXX" );
	strcpy(iotDevice.deviceInfo.manufacturer , "ST microelectronics");
	strcpy(iotDevice.deviceInfo.model , "7865");
	strcpy(iotDevice.deviceInfo.deviceClass , "DMA");
	strcpy(iotDevice.deviceInfo.description , "Sensor board");
	strcpy(iotDevice.deviceInfo.fwVersion , "1.0.0");
	strcpy(iotDevice.deviceInfo.hwVersion , "1.0");
	strcpy(iotDevice.deviceInfo.descriptiveLocation , "EGL C");

	exit:
	return rc;
}

/**
 * Function used to connect to the IoT client
 * @param client - Reference to the Iotfclient
 *
 * @return int return code
 */
int connectiotf(void)
{
	int rc = isConnected();
	if(rc){ //if connected return
		printf("Client is connected\n");
		return rc;
	}

	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

	char hostname[HOSTNAME_STRING_SIZE];
	sprintf(hostname, iotDevice.deviceClient.cfg.domain);

	NewNetwork(&iotDevice.deviceClient.n);

	if((rc = ConnectNetwork(&(iotDevice.deviceClient.n),hostname,iotDevice.deviceClient.cfg.port)) != 0){
		msg_error("ConnectNetwork Failed: %d\n", rc);
		goto exit;
	}

	MQTTClient(&iotDevice.deviceClient.c, &iotDevice.deviceClient.n, 1000, iotDevice.deviceClient.buf,
			BUFFER_SIZE, iotDevice.deviceClient.readbuf, BUFFER_SIZE);

	data.willFlag = 0;
	data.MQTTVersion = 3;
	data.clientID.cstring = iotDevice.deviceClient.cfg.id;

	data.keepAliveInterval = keepAliveInterval;
	data.cleansession = 1;

	rc = MQTTConnect(&iotDevice.deviceClient.c, &data);
	if(rc != 0){
		msg_error("MqttConnect Failed: %d\n", rc);
	}

	exit:
	if(rc != 0){
		freeTLSConnectData(&(iotDevice.deviceClient.n.TLSConnectData));
	}

	return rc;
}

/**
 * Function used to publish the given data to the topic with the given QoS
 * @Param client - Address of MQTT Client
 * @Param topic - Topic to publish
 * @Param payload - Message payload
 *
 * @return int - Return code from MQTT Publish
 **/
int publishData(char *topic, char *payload){
	int rc = -1;
	MQTTMessage pub;

	pub.qos=QOS0;
	//       if (qos==0) pub.qos=QOS0;
	//       if (qos==1) pub.qos=QOS1;
	//       if (qos==2) pub.qos=QOS2;

	pub.retained = '0';
	pub.payload = payload;
	pub.payloadlen = strlen(payload);

	rc = MQTTPublish(&iotDevice.deviceClient.c, topic , &pub);
	if(rc != SUCCESS) {
		msg_error("\nConnection lost, retry the connection \n");
		retry_connection();
		rc = MQTTPublish(&iotDevice.deviceClient.c, topic , &pub);
	}

	return(rc);
}

/**
 * Function used to Yield for commands.
 * @param time_ms - Time in milliseconds
 * @return int return code
 */
int yield(int time_ms)
{
	int rc = 0;
	rc = MQTTYield(&iotDevice.deviceClient.c, time_ms);

	return rc;
}

/**
 * Function used to check if the client is connected
 *
 * @return int return code
 */
int isConnected(void)
{
	int result = iotDevice.deviceClient.c.isconnected;
	return result;
}

/**
 * Function used to disconnect from the IoT service
 *
 * @return int return code
 */

int disconnect(void)
{
	int rc = 0;
	if(isConnected())
		rc = MQTTDisconnect(&iotDevice.deviceClient.c);
	iotDevice.deviceClient.n.disconnect(&(iotDevice.deviceClient.n));

	return rc;

}

/**
 * Function used to set the time to keep the connection alive with IoT service
 * @param keepAlive - time in secs
 *
 */
void setKeepAliveInterval(unsigned int keepAlive)
{
	keepAliveInterval = keepAlive;

}

//Staggered retry
int retry_connection(void)
{
	int retry = 1;
	int rc = -1;
	int delay = 3;

	while((rc = connectiotf()) != SUCCESS)
	{
		printf("Retry Attempt #%d ", retry);
		printf(" next attempt in %d seconds\n", delay);
		sleep(delay);
	}
	return rc;
}

