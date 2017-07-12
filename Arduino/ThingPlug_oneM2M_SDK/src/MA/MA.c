/** * @file MA.c
 *
 * @brief MangementAgent
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */
#include <Arduino.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "MA.h"
#include "../SRA/SRA.h"
#include "../SRA/TTVDatatype.h"
#include "../SMA/SMA.h"

#include "Configuration.h"
#include "StreamWrapper.h"
#include "include/oneM2M_V1_14.h"
#include "include/oneM2M.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MQTT_CLIENT_ID                      "%s_%s"

#define TOPIC_PUBLISH                       "/oneM2M/req/%s/%s"
#define TOPIC_SUBSCRIBE_REQ                 "/oneM2M/req_msg/%s/%s"
#define TOPIC_SUBSCRIBE_RES                 "/oneM2M/resp/%s/%s"
#define TOPIC_SUBSCRIBE_SIZE                2

#define TO_AE                               "%s/ae-%s"
#define TO_CONTAINER                        "%s/ae-%s/cnt-%s"
#define TO_MGMTCMD                          "%s/mgc-%s"
#define TO_SUBSCRIPTION                     "%s/ae-%s/cnt-%s/sub-%s"
#define TO_CONTENTINSTANCE                  "%s/ae-%s/cnt-%s/cin-%s"
#define TO_MGMTCMDRESULT                    "%s/%s/exin-%s"
#define TO_MGC                              "mgc-%s"

#define SIZE_RESPONSE_CODE                  10
#define SIZE_RESPONSE_MESSAGE               128
#define SIZE_TOPIC                          128
#define SIZE_PAYLOAD                        2048

static enum PROCESS_STEP
{
    PROCESS_NODE_CREATE = 0,
    PROCESS_REMOTECSE_CREATE,
    PROCESS_AE_CREATE,
    PROCESS_CONTAINER_CREATE,
    PROCESS_MGMTCMD_CREATE,
    PROCESS_CONTENTINSTANCE_CREATE,

    PROCESS_END
} mStep;

static char mToStart[128] = "";
static char mAEID[128] = "";
static char mNodeLink[23] = "";
static char mClientID[24] = "";

int CreateAE() {
    mStep = PROCESS_AE_CREATE;
    int rc = -1;
    snprintf(mToStart, sizeof(mToStart), ONEM2M_TO, ONEM2M_SERVICE_ID);
    rc = tp_v1_14_RegisterDevice("S", mToStart, ONEM2M_AE_NAME, ONEM2M_AE_NAME, ACCOUNT_CREDENTIAL_ID, ONEM2M_SERVICE_ID, mClientID);
    return rc;
}

int CreateContainer() {
    mStep = PROCESS_CONTAINER_CREATE;
    int rc = -1;
    char to[512] = "";
    snprintf(to, sizeof(to), TO_AE, mToStart, ONEM2M_AE_NAME);
    rc = tp_v1_14_RegisterContainer(mAEID, to, NAME_CONTAINER);
    return rc;
}

int CreateMgmtCmd(char* cmt, char* name) {
    mStep = PROCESS_MGMTCMD_CREATE;
    int rc = -1;
    rc = tp_v1_14_RegisterMgmtCmd(mAEID, mToStart, name, cmt, mNodeLink);
    return rc;
}

int CreateContentInstance() {
    mStep = PROCESS_CONTENTINSTANCE_CREATE;
    int rc = -1;
    char to[512] = "";
    char* cnf = "Lora/Sensor";

    char *ttv;

    char* sensorDescription = "temperature";
    char *output = NULL;
    Stream_print_str(NULL, "PROCESS_CONTENTINSTANCE_CREATE");
    SMAGetData(sensorDescription, &output);
    SRAGetTTV( &ttv, 0x11, DATATYPE_FLOAT, output );
    tp_v1_14_AddData(ttv, strlen(ttv));
    free(ttv);
    free(output);

    sensorDescription = "humidity";
    SMAGetData(sensorDescription, &output);
    SRAGetTTV( &ttv, 0x12, DATATYPE_FLOAT, output );
    tp_v1_14_AddData(ttv, strlen(ttv));
    free(ttv);
    free(output);

    sensorDescription = "light";
    SMAGetData(sensorDescription, &output);
    SRAGetTTV( &ttv, 0x25, DATATYPE_USHORT, output );
    tp_v1_14_AddData(ttv, strlen(ttv));
    free(ttv);
    free(output);

    sensorDescription = "proximity";
    SMAGetData(sensorDescription, &output);
    SRAGetTTV( &ttv, 0x31, DATATYPE_USHORT, output );
    tp_v1_14_AddData(ttv, strlen(ttv));
	Stream_print_str(NULL, output);
    free(ttv);
    free(output);

    snprintf(to, sizeof(to), TO_CONTAINER, mToStart, ONEM2M_AE_NAME, NAME_CONTAINER);
    rc = tp_v1_14_Report(mAEID, to, cnf, NULL, 1);
    return rc;
}

static void UpdateExecInstance(char* nm, char* ri) {
    Stream_print_str(NULL, "UpdateExecInstance");
    char to[512] = "";
    snprintf(to, sizeof(to), TO_MGMTCMDRESULT, mToStart, nm, ri);
    tp_v1_14_Result(mAEID, to, "0", "3");
}

static int SimpleXmlParser(char* payload, char* name, char* value, int isPC) {
    int rc = 0;
    char start[10];
    char end[10];
    char* pl = payload;
    memset(start, 0, sizeof(start));
    memset(end, 0, sizeof(end));

    if(isPC) {
        pl = strstr(payload, "<pc>");
        if(!pl) return -1;
    }

    snprintf(start, sizeof(start), "<%s>", name);
    snprintf(end, sizeof(end), "</%s>", name);
    char* s = strstr(pl, start) + strlen(start);
    char* e = strstr(pl, end);

    if(s && e) {
        memcpy(value, s, e-s);
        Stream_print_str(NULL,  name);
        Stream_print_str(NULL,  value);
    }
    return rc;
}

static int IsCMD(char* payload) {
    char* request = strstr(payload, "<m2m:rqp");
    char* exin = strstr(payload, "<exin");
    return request && exin;

}
int led = 0;
static void ProcessCMD(char* payload, int payloadLen) {
    Stream_print_str(NULL, "ProcessCMD");
    Stream_print_str(NULL, "payload->");
    char *tmp = calloc(1,payloadLen+1);
    memcpy(tmp,payload,payloadLen);
    Stream_print_str(NULL, tmp);
    free(tmp);
    char nm[128] = "";
    char exra[1024] = "";
    char resourceId[23] = "";
    SimpleXmlParser(payload, ATTR_NM, nm, 1);
    SimpleXmlParser(payload, ATTR_EXRA, exra, 1);
    SimpleXmlParser(payload, ATTR_RI, resourceId, 1);

    if(exra[0] == '8' && exra[1] == '8'){   
        SMASetLED(LED_PIN,led);
        led = !led;
    }
    if(exra[0] == '8' && exra[1] == '9'){   
        char str[13] = {0};
        int i;
        for( i = 0; i < 12; i++){
            char tmp[3]={0};
            memcpy(tmp, &exra[4+i*2],2);
            str[i] = strtol( tmp, 0, 16);
        }
        SMASetLCD(0, 1, str, 1);
    }

    if(!nm[0]){
        snprintf(nm, sizeof(nm), TO_MGC, NAME_MGMTCMD_FIRMWARE);
    }
    UpdateExecInstance(nm, resourceId);
}

void MQTTConnected(int result) {
    Stream_print_str(NULL, "MQTTConnected");
}

void MQTTSubscribed(int result) {
    Stream_print_str(NULL, "MQTTSubscribed");
    CreateAE();
}

void MQTTDisconnected(int result) {
    Stream_print_str(NULL, "MQTTDisconnected");
}

void MQTTConnectionLost(char* cause) {
    Stream_print_str(NULL, "MQTTConnectionLost");
}

void MQTTMessageDelivered(int token) {
    Stream_print_str(NULL, "MQTTMessageDelivered token");
}

void MQTTMessageArrived(char* topic, char* msg, int msgLen) {
    Stream_print_str(NULL, "MQTTMessageArrived topic");

    if(msg == NULL || msgLen < 1) {
        return;
    }

    if(IsCMD(msg)) {
        ProcessCMD(msg, msgLen);
        return;
    }

    char payload[SIZE_PAYLOAD] = "";
    memcpy(payload, msg, msgLen);
    Stream_print_str(NULL, "payload->");
    Stream_print_str(NULL, payload);
    char rsc[SIZE_RESPONSE_CODE] = "";
    SimpleXmlParser(payload, ATTR_RSC, rsc, 0);
    char rsm[SIZE_RESPONSE_MESSAGE] = "";
    SimpleXmlParser(payload, ATTR_RSM, rsm, 0);

    switch(mStep) {
       case PROCESS_AE_CREATE:
           Stream_print_str(NULL, "PROCESS_AE_CREATE");
            SimpleXmlParser(payload, ATTR_AEI, mAEID, 1);
            SimpleXmlParser(payload, ATTR_NL, mNodeLink, 1);
            if(strlen(mAEID) > 0 && strlen(mNodeLink) > 0) {
                CreateContainer();
            }
            break;
        case PROCESS_CONTAINER_CREATE:
            Stream_print_str(NULL, "PROCESS_CONTAINER_CREATE");
            CreateMgmtCmd(CMT_MGMTCMD, NAME_MGMTCMD);
            Stream_print_str(NULL, "CMT_MGMTCMD_FIRMWARE, NAME_MGMTCMD_FIRMWARE");
            CreateMgmtCmd(CMT_MGMTCMD_FIRMWARE, NAME_MGMTCMD_FIRMWARE);
            break;
        case PROCESS_MGMTCMD_CREATE:
            Stream_print_str(NULL, "PROCESS_MGMTCMD_CREATE");
            // CreateContentInstance();
            mStep = PROCESS_CONTENTINSTANCE_CREATE;
            break;
        default:
            break;
    }
}

int MARun() {
    Stream_print_str(NULL, "ThingPlug_oneM2M_SDK(oneM2M v1.14)");
    int rc;
    // set callbacks
    rc = tpMQTTSetCallbacks(MQTTConnected, MQTTSubscribed, MQTTDisconnected, MQTTConnectionLost, MQTTMessageDelivered, MQTTMessageArrived);
    if(rc != 0) {
            Stream_print_str(NULL, "#### tpMQTTSetCallbacks failed");
            goto result_error;
    }
    // create
    char subscribeTopic[TOPIC_SUBSCRIBE_SIZE][SIZE_TOPIC];
    char publishTopic[SIZE_TOPIC] = "";
    memset(subscribeTopic, 0, sizeof(subscribeTopic));
    snprintf(mClientID, sizeof(mClientID), MQTT_CLIENT_ID, ACCOUNT_USER_ID, ONEM2M_AE_NAME);
    Stream_print_str(NULL, "client id->");
    Stream_print_str(NULL, mClientID);
    Stream_print_str(NULL, "<-client id->");

    snprintf(subscribeTopic[0], sizeof(subscribeTopic[0]), TOPIC_SUBSCRIBE_REQ, ONEM2M_SERVICE_ID, mClientID);
    snprintf(subscribeTopic[1], sizeof(subscribeTopic[1]), TOPIC_SUBSCRIBE_RES, mClientID, ONEM2M_SERVICE_ID);
    snprintf(publishTopic, sizeof(publishTopic), TOPIC_PUBLISH, mClientID, ONEM2M_SERVICE_ID);

    char* st[] = {subscribeTopic[0], subscribeTopic[1]};

    int port = (!MQTT_ENABLE_SERVER_CERT_AUTH ? MQTT_PORT : MQTT_SECURE_PORT);
    rc = tpMQTTCreate(MQTT_HOST, port, MQTT_KEEP_ALIVE, ACCOUNT_USER_ID, ACCOUNT_CREDENTIAL_ID, MQTT_ENABLE_SERVER_CERT_AUTH, st, TOPIC_SUBSCRIBE_SIZE, publishTopic, mClientID);
    {
        char str[256];
        sprintf(str,"tpMQTTCreate result : %d", rc);
        Stream_print_str(NULL, str);
    }
	SMAInitLCD(16, 2);
	SMASetLCD(0, 1, "Hello World!", 1);
	SMASetLCDRGB(255,255,255);
    if(rc == 0) {
        while (mStep < PROCESS_END) {  
			{
				char str[256];
				sprintf(str,"LOOP mStep = %d", mStep);
				Stream_print_str(NULL, str);
			}
            if(tpMQTTIsConnected() && mStep == PROCESS_CONTENTINSTANCE_CREATE) {
				Stream_print_str(NULL, "mStep = PROCESS_CONTENTINSTANCE_CREATE");				
                CreateContentInstance();
            }
			tpMQTTYield(10000);
        }
        Stream_print_str(NULL,"disconn");
        tpMQTTDisconnect();
    }
result_error:
    tpMQTTDestory();
    return 0;
}

#ifdef __cplusplus
}
#endif

