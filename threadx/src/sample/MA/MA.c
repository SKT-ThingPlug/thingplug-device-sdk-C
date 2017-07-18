/**

 * @file MA.c
 *
 * @brief MangementAgent
 *
 * Copyright (C) 2017. SK Telecom, All Rights Reserved.
 * Written 2017, by SK Telecom
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "MA.h"
#include "sample/SRA/SRA.h"
#include "sample/SRA/TTVDatatype.h"
#include "sample/SMA/SMA.h"

#include "MQTT/MQTTS7G2.h"
#include "MQTT/MQTTS7G2_TLS.h"
#include "MQTT/MQTTClient.h".
#include "oneM2M/SKTtpDebug.h"

#include "oneM2M/oneM2M.h"
#include "oneM2M/Versions/oneM2M_V1_14.h"
#include "Configuration.h"
#include "oneM2M/SKTtpDebug.h"

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
    SKTDebugPrint(LOG_LEVEL_INFO, "PROCESS_CONTENTINSTANCE_CREATE");
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

    sensorDescription = "motion";
    SMAGetData(sensorDescription, &output);
    SRAGetTTV( &ttv, 0x2F, DATATYPE_BOOLEAN, output );
    tp_v1_14_AddData(ttv, strlen(ttv));
    free(ttv);
    free(output);

    snprintf(to, sizeof(to), TO_CONTAINER, mToStart, ONEM2M_AE_NAME, NAME_CONTAINER);
    rc = tp_v1_14_Report(mAEID, to, cnf, NULL, 1);
    return rc;
}

static void UpdateExecInstance(char* nm, char* ri) {
    SKTDebugPrint(LOG_LEVEL_INFO, "UpdateExecInstance");
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
        SKTDebugPrint(LOG_LEVEL_INFO,  name);
        SKTDebugPrint(LOG_LEVEL_INFO,  value);
    }
    return rc;
}

static int IsCMD(char* payload) {
    char* request = strstr(payload, "<m2m:rqp");
    char* exin = strstr(payload, "<exin");
    return request && exin;

}

static void ProcessCMD(char* payload, int payloadLen) {
    SKTDebugPrint(LOG_LEVEL_INFO, "ProcessCMD");
    SKTDebugPrint(LOG_LEVEL_INFO, "payload->");
    char *tmp = calloc(1,payloadLen+1);
    memcpy(tmp,payload,payloadLen);
    SKTDebugPrint(LOG_LEVEL_INFO, tmp);
    free(tmp);
    char nm[128] = "";
    char exra[1024] = "";
    char resourceId[23] = "";
    SimpleXmlParser(payload, ATTR_NM, nm, 1);
    SimpleXmlParser(payload, ATTR_EXRA, exra, 1);
    SimpleXmlParser(payload, ATTR_RI, resourceId, 1);
    if(!nm[0]){
        snprintf(nm, sizeof(nm), TO_MGC, NAME_MGMTCMD_FIRMWARE);
    }
    UpdateExecInstance(nm, resourceId);
}

void MQTTConnected(int result) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTConnected");
}

void MQTTSubscribed(int result) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTSubscribed");
    CreateAE();
}

void MQTTDisconnected(int result) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTDisconnected");
}

void MQTTConnectionLost(char* cause) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTConnectionLost");
}

void MQTTMessageDelivered(int token) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTMessageDelivered token");
}

void MQTTMessageArrived(char* topic, char* msg, int msgLen) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTMessageArrived topic");

    if(msg == NULL || msgLen < 1) {
        return;
    }

    if(IsCMD(msg)) {
        ProcessCMD(msg, msgLen);
        return;
    }

    char payload[SIZE_PAYLOAD] = "";
    memcpy(payload, msg, msgLen);
    SKTDebugPrint(LOG_LEVEL_INFO, "payload->");
    SKTDebugPrint(LOG_LEVEL_INFO, payload);
    char rsc[SIZE_RESPONSE_CODE] = "";
    SimpleXmlParser(payload, ATTR_RSC, rsc, 0);
    char rsm[SIZE_RESPONSE_MESSAGE] = "";
    SimpleXmlParser(payload, ATTR_RSM, rsm, 0);

    switch(mStep) {
       case PROCESS_AE_CREATE:
           SKTDebugPrint(LOG_LEVEL_INFO, "PROCESS_AE_CREATE");
            SimpleXmlParser(payload, ATTR_AEI, mAEID, 1);
            SimpleXmlParser(payload, ATTR_NL, mNodeLink, 1);
            if(strlen(mAEID) > 0 && strlen(mNodeLink) > 0) {
                CreateContainer();
            }
            break;
        case PROCESS_CONTAINER_CREATE:
            SKTDebugPrint(LOG_LEVEL_INFO, "PROCESS_CONTAINER_CREATE");
            CreateMgmtCmd(CMT_MGMTCMD, NAME_MGMTCMD);
            SKTDebugPrint(LOG_LEVEL_INFO, "CMT_MGMTCMD_FIRMWARE, NAME_MGMTCMD_FIRMWARE");
            CreateMgmtCmd(CMT_MGMTCMD_FIRMWARE, NAME_MGMTCMD_FIRMWARE);
            break;
        case PROCESS_MGMTCMD_CREATE:
            SKTDebugPrint(LOG_LEVEL_INFO, "PROCESS_MGMTCMD_CREATE");
            // CreateContentInstance();
            mStep = PROCESS_CONTENTINSTANCE_CREATE;
            break;
        default:
            break;
    }
}

int MARun() {
    SKTDebugInit(1, LOG_LEVEL_INFO);
    SKTDebugPrint(LOG_LEVEL_INFO, "ThingPlug_oneM2M_SDK(oneM2M v1.14)");
    int rc;

    // set callbacks
    rc = tpMQTTSetCallbacks(MQTTConnected, MQTTSubscribed, MQTTDisconnected, MQTTConnectionLost, MQTTMessageDelivered, MQTTMessageArrived);
    SKTDebugPrint(LOG_LEVEL_INFO, "tpMQTTSetCallbacks result");
    if(rc != 0) {
            SKTDebugPrint(LOG_LEVEL_DEBUG, "#### tpMQTTSetCallbacks failed");
            goto result_error;
    }
    // create
    char subscribeTopic[TOPIC_SUBSCRIBE_SIZE][SIZE_TOPIC];
    char publishTopic[SIZE_TOPIC] = "";
    memset(subscribeTopic, 0, sizeof(subscribeTopic));
    snprintf(mClientID, sizeof(mClientID), MQTT_CLIENT_ID, ACCOUNT_USER_ID, ONEM2M_AE_NAME);
    SKTDebugPrint(LOG_LEVEL_INFO, "client id->");
    SKTDebugPrint(LOG_LEVEL_INFO, mClientID);
    SKTDebugPrint(LOG_LEVEL_INFO, "<-client id->");

    snprintf(subscribeTopic[0], sizeof(subscribeTopic[0]), TOPIC_SUBSCRIBE_REQ, ONEM2M_SERVICE_ID, mClientID);
    snprintf(subscribeTopic[1], sizeof(subscribeTopic[1]), TOPIC_SUBSCRIBE_RES, mClientID, ONEM2M_SERVICE_ID);
    snprintf(publishTopic, sizeof(publishTopic), TOPIC_PUBLISH, mClientID, ONEM2M_SERVICE_ID);

    char* st[] = {subscribeTopic[0], subscribeTopic[1]};
#if(MQTT_ENABLE_SERVER_CERT_AUTH)
    char host[] = MQTT_SECURE_HOST;
    int port = MQTT_SECURE_PORT;
#else
    char host[] = MQTT_HOST;
    int port = MQTT_PORT;
#endif
    rc = tpMQTTCreate(host, port, MQTT_KEEP_ALIVE, ACCOUNT_USER_ID, ACCOUNT_CREDENTIAL_ID, MQTT_ENABLE_SERVER_CERT_AUTH, st, TOPIC_SUBSCRIBE_SIZE, publishTopic, mClientID);
    {
        char str[256];
        sprintf(str,"tpMQTTCreate result : %d", rc);
        SKTDebugPrint(LOG_LEVEL_INFO, str);
    }
    if(rc == 0) {
        while (mStep < PROCESS_END) {
            if(tpMQTTYield(900) != 0)  // wait for seconds
            {
                tx_thread_sleep(100);
            }
            if(tpMQTTIsConnected() && mStep == PROCESS_CONTENTINSTANCE_CREATE) {
                CreateContentInstance();
            }
        }
        SKTDebugPrint(LOG_LEVEL_INFO, "disconn");
        tpMQTTDisconnect();
    }
result_error:
    tpMQTTDestory();
    return 0;
}
