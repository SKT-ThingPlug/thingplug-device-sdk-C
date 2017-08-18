/**
 * @file SDKVerification.c
 *
 * @brief Simple SDK verification application
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */

#include <stdlib.h>
#include <string.h>

#include "Configuration.h"
#include "oneM2M.h"
#include "oneM2M_V1_14.h"
#include "SKTtpDebug.h"
#include "cmsis_os.h"

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
#define TO_MGMTCMDRESULT                    "%s/mgc-%s/exin-%s"

#define SIZE_RESPONSE_CODE                  10
#define SIZE_RESPONSE_MESSAGE               128
#define SIZE_TOPIC                          128
#define SIZE_PAYLOAD                        1024

enum VERIFICATION_STEP
{
    VERIFICATION_CSEBASE_RETRIEVE = 0,
    VERIFICATION_NODE_CREATE,
    VERIFICATION_REMOTECSE_CREATE,
    VERIFICATION_CONTAINER_CREATE,
    VERIFICATION_MGMTCMD_CREATE,
    VERIFICATION_CONTENTINSTANCE_CREATE,
    VERIFICATION_LOCATIONPOLICY_CREATE,
    VERIFICATION_AE_CREATE,
    VERIFICATION_AREANWKINFO_CREATE,

    VERIFICATION_AREANWKINFO_UPDATE = 9,
    VERIFICATION_AE_UPDATE,
    VERIFICATION_LOCATIONPOLICY_UPDATE,
    VERIFICATION_MGMTCMD_UPDATE,
    VERIFICATION_CONTAINER_UPDATE,
    VERIFICATION_REMOTECSE_UPDATE,
    VERIFICATION_NODE_UPDATE,

    VERIFICATION_AREANWKINFO_DELETE = 16,
    VERIFICATION_AE_DELETE,
    VERIFICATION_LOCATIONPOLICY_DELETE,
    VERIFICATION_CONTENTINSTANCE_DELETE,
    VERIFICATION_MGMTCMD_DELETE,
    VERIFICATION_MGMTCMD_FIRMWARE_DELETE,
    VERIFICATION_CONTAINER_DELETE,
    VERIFICATION_REMOTECSE_DELETE,
    VERIFICATION_NODE_DELETE,

    VERIFICATION_ACCESSCONTROLPOLICY_CREATE,
    VERIFICATION_ACCESSCONTROLPOLICY_UPDATE,
    VERIFICATION_ACCESSCONTROLPOLICY_DELETE,

    VERIFICATION_SUBSCRIPTION_CREATE,
    VERIFICATION_SUBSCRIPTION_UPDATE,
    VERIFICATION_SUBSCRIPTION_DELETE,

    VERIFICATION_WAIT,
    VERIFICATION_END

};

// member variables
static enum VERIFICATION_STEP mStep = VERIFICATION_CSEBASE_RETRIEVE;
static char mToStart[128] = "";
static char mAEID[128] = "";
static char mNodeLink[23] = "";
static char mContentInstanceResourceID[23] = "";
static char mClientID[24] = "";

static int DO_CREATE_ONLY = 0;
/**
 * @brief do verification step
 */
void DoVerificationStep() {

    int resourceType = 0;
    int operation = 0;
    char to[512] = "";
    char* fr = NULL;
    void* pc = NULL;

    switch(mStep) {

        // RETRIEVE
        case VERIFICATION_CSEBASE_RETRIEVE:
        {
            resourceType = CSEBase;
            operation = RETRIEVE;
            fr = "S";
            snprintf(to, sizeof(to), ONEM2M_TO, ONEM2M_SERVICE_ID);
            memcpy(mToStart, to, strlen(to));
        }
        break;

        // CREATE
        case VERIFICATION_AE_CREATE:
        {
            resourceType = AE;
            operation = CREATE;
            fr = "S";
            memcpy(to, mToStart, strlen(mToStart));
            oneM2M_v1_14_AE* AECreate = (oneM2M_v1_14_AE *)calloc(sizeof(oneM2M_v1_14_AE), 1);
            AECreate->rn = ONEM2M_AE_NAME;
            AECreate->api = ACCOUNT_CREDENTIAL_ID;
            AECreate->rr = "true";
            AECreate->ni = ONEM2M_AE_NAME;
            char mga[128] = "";
            snprintf(mga, sizeof(mga), ONEM2M_MGA, mClientID);
            AECreate->mga = mga;
            char poa[128] = "";
            snprintf(poa, sizeof(poa), ONEM2M_POA, ONEM2M_SERVICE_ID, mClientID);
            AECreate->poa = poa;
            pc = (void *)AECreate;
        }
        break;
        case VERIFICATION_CONTAINER_CREATE:
        {
            resourceType = container;
            operation = CREATE;
            fr = mAEID;
            snprintf(to, sizeof(to), TO_AE, mToStart, ONEM2M_AE_NAME);
            oneM2M_v1_14_container* containerCreate = (oneM2M_v1_14_container *)calloc(sizeof(oneM2M_v1_14_container), 1);
            containerCreate->rn = NAME_CONTAINER;
            pc = (void *)containerCreate;
        }
        break;
        case VERIFICATION_CONTENTINSTANCE_CREATE:
        {
            resourceType = contentInstance;
            operation = CREATE;
            fr = mAEID;
            snprintf(to, sizeof(to), TO_CONTAINER, mToStart, ONEM2M_AE_NAME, NAME_CONTAINER);
            oneM2M_v1_14_contentInstance* contentInstanceCreate = (oneM2M_v1_14_contentInstance *)calloc(sizeof(oneM2M_v1_14_contentInstance), 1);
            contentInstanceCreate->cnf = "text";
            contentInstanceCreate->con = "45";
            pc = (void *)contentInstanceCreate;
        }
        break;
        case VERIFICATION_MGMTCMD_CREATE:
        {
            resourceType = mgmtCmd;
            operation = CREATE;
            fr = mAEID;
            memcpy(to, mToStart, strlen(mToStart));
            oneM2M_v1_14_mgmtCmd* mgmtCmdCreate = (oneM2M_v1_14_mgmtCmd *)calloc(sizeof(oneM2M_v1_14_mgmtCmd), 1);
            mgmtCmdCreate->rn = NAME_MGMTCMD;
            mgmtCmdCreate->cmt = CMT_MGMTCMD;
            mgmtCmdCreate->ext = mNodeLink;
            pc = (void *)mgmtCmdCreate;
        }
        break;
        case VERIFICATION_SUBSCRIPTION_CREATE:
        {
            resourceType = subscription;
            operation = CREATE;
            fr = mAEID;
            memcpy(to, mToStart, strlen(mToStart));
            snprintf(to, sizeof(to), TO_CONTAINER, mToStart, ONEM2M_AE_NAME, NAME_CONTAINER);
            oneM2M_v1_14_subscription* subscriptionObject = (oneM2M_v1_14_subscription *)calloc(sizeof(oneM2M_v1_14_subscription), 1);
            subscriptionObject->rn = NAME_SUBSCRIPTION;
            // eventNotificationCriteria:notificationEventType
            // 1 : Update_of_Resource,
            // 2 : Delete_of_Resource,
            // 3 : Create_of_Direct_Child_Resource,
            // 4 : Delete_of_Direct_Child_Resource
            subscriptionObject->enc = "<net>3</net>";
            char nu[128] = "";
            snprintf(nu, sizeof(nu), ONEM2M_NU, mClientID);
            subscriptionObject->nu = nu;
            // notificationContentType
            // 1 : All Attributes, 2 : Modefied Attributes, 3 : ResourceID
            subscriptionObject->nct = "1";

            pc = (void *)subscriptionObject;
        }
        break;
        //     case VERIFICATION_ACCESSCONTROLPOLICY_CREATE:
        //          {
        //         resourceType = accessControlPolicy;
        //         operation = CREATE;
        //          fr = mAEID;
        //         snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, ONEM2M_AE_RESOURCENAME);
        //         oneM2M_v1_12_accessControlPolicy* accessControlPolicy = (oneM2M_v1_12_accessControlPolicy *)calloc(sizeof(oneM2M_v1_12_accessControlPolicy), 1);
        //          accessControlPolicy->rn = NAME_ACCESSCONTROLPOLICY;
        //          snprintf(buffer, sizeof(buffer), "<acr><acor>%s</acor><acop>63</acop></acr>", mAEID);
        //          accessControlPolicy->pv = buffer;
        //          //memset(buffer, 0, sizeof(buffer));
        //         accessControlPolicy->pvs = buffer;
        //         pc = (void *)accessControlPolicy;
        //         }
        //         break;

        // UPDATE
        case VERIFICATION_AE_UPDATE:
        {
            resourceType = AE;
            operation = UPDATE;
            fr = mAEID;
            snprintf(to, sizeof(to), TO_AE, mToStart, ONEM2M_AE_NAME);
            oneM2M_v1_14_AE* AEUpdate = (oneM2M_v1_14_AE *)calloc(sizeof(oneM2M_v1_14_AE), 1);
            AEUpdate->rr = "false";
            AEUpdate->poa = "http://";
            pc = (void *)AEUpdate;
        }
        break;
        case VERIFICATION_CONTAINER_UPDATE:
        {
            resourceType = container;
            operation = UPDATE;
            fr = mAEID;
            snprintf(to, sizeof(to), TO_CONTAINER, mToStart, ONEM2M_AE_NAME, NAME_CONTAINER);
            oneM2M_v1_14_container* containerUpdate = (oneM2M_v1_14_container *)calloc(sizeof(oneM2M_v1_14_container), 1);
            containerUpdate->lbl = "lbl";
            pc = (void *)containerUpdate;
        }
        break;
        case VERIFICATION_MGMTCMD_UPDATE:
        {
            resourceType = mgmtCmd;
            operation = UPDATE;
            fr = mAEID;
            snprintf(to, sizeof(to), TO_MGMTCMD, mToStart, NAME_MGMTCMD);
            oneM2M_v1_14_mgmtCmd* mgmtCmdUpdate = (oneM2M_v1_14_mgmtCmd *)calloc(sizeof(oneM2M_v1_14_mgmtCmd), 1);
            mgmtCmdUpdate->dc = "test";
            pc = (void *)mgmtCmdUpdate;
        }
        break;
        //     case VERIFICATION_ACCESSCONTROLPOLICY_UPDATE:
        //         {
        //         resourceType = accessControlPolicy;
        //         operation = UPDATE;
        //         fr = mAEID;
        //         snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, ONEM2M_AE_RESOURCENAME, NAME_ACCESSCONTROLPOLICY);
        //         oneM2M_v1_12_accessControlPolicy* accessControlPolicy = (oneM2M_v1_12_accessControlPolicy *)calloc(sizeof(oneM2M_v1_12_accessControlPolicy), 1);
        //         snprintf(buffer, sizeof(buffer), "<acr><acor>%s</acor><acop>63</acop></acr>", mAEID);
        //         accessControlPolicy->pv = buffer;
        //         //memset(buffer, 0, sizeof(buffer));
        //         accessControlPolicy->pvs = buffer;
        //         pc = (void *)accessControlPolicy;
        //         }
        //         break;
        case VERIFICATION_SUBSCRIPTION_UPDATE:
        {
            resourceType = subscription;
            operation = UPDATE;
            fr = mAEID;
            snprintf(to, sizeof(to), TO_SUBSCRIPTION, mToStart, ONEM2M_AE_NAME, NAME_CONTAINER, NAME_SUBSCRIPTION);
            oneM2M_v1_14_subscription* subscriptionObject = (oneM2M_v1_14_subscription *)calloc(sizeof(oneM2M_v1_14_subscription), 1);
            subscriptionObject->nct = "2";
            pc = (void *)subscriptionObject;
        }
        break;

        // DELETE
        case VERIFICATION_AE_DELETE:
        {
            resourceType = AE;
            operation = DELETE;
            fr = mAEID;
            snprintf(to, sizeof(to), TO_AE, mToStart, ONEM2M_AE_NAME);
        }
        break;
        case VERIFICATION_CONTAINER_DELETE:
        {
            resourceType = container;
            operation = DELETE;
            fr = mAEID;
            snprintf(to, sizeof(to), TO_CONTAINER, mToStart, ONEM2M_AE_NAME, NAME_CONTAINER);
        }
        break;
        case VERIFICATION_CONTENTINSTANCE_DELETE:
        {
            resourceType = contentInstance;
            operation = DELETE;
            fr = mAEID;
            snprintf(to, sizeof(to), TO_CONTENTINSTANCE, mToStart, ONEM2M_AE_NAME, NAME_CONTAINER, mContentInstanceResourceID);
        }
        break;
        case VERIFICATION_MGMTCMD_DELETE:
        {
            resourceType = mgmtCmd;
            operation = DELETE;
            fr = mAEID;
            snprintf(to, sizeof(to), TO_MGMTCMD, mToStart, NAME_MGMTCMD);
        }
        break;
        case VERIFICATION_MGMTCMD_FIRMWARE_DELETE:
        {
            resourceType = mgmtCmd;
            operation = DELETE;
            fr = mAEID;
            snprintf(to, sizeof(to), TO_MGMTCMD, mToStart, NAME_MGMTCMD_FIRMWARE);
        }
        break;
        //     case VERIFICATION_ACCESSCONTROLPOLICY_DELETE:
        //         {
        //         resourceType = accessControlPolicy;
        //         operation = DELETE;
        //         fr = mAEID;
        //         snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, ONEM2M_AE_RESOURCENAME, NAME_ACCESSCONTROLPOLICY);
        //         }
        //         break;
        case VERIFICATION_SUBSCRIPTION_DELETE:
        {
            resourceType = subscription;
            operation = DELETE;
            fr = mAEID;
            snprintf(to, sizeof(to), TO_SUBSCRIPTION, mToStart, ONEM2M_AE_NAME, NAME_CONTAINER, NAME_SUBSCRIPTION);
        }
        break;

        default:
        {
            mStep = VERIFICATION_END;
        }
        break;
    }

    if(mStep < VERIFICATION_END) {
        tp_oneM2M_v1_14_Request(resourceType, operation, fr, to, pc);
        if(pc) free(pc);
    }
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
        SKTDebugPrint(LOG_LEVEL_INFO, name);
        SKTDebugPrint(LOG_LEVEL_INFO, value);
    }
    return rc;
}

// static char* strnstr(char *s, char *text, size_t slen) {
//  char c, sc;
//  size_t len;

//  if ((c = *text++) != '\0') {
//      len = strlen(text);
//      do {
//          do {
//              if ((sc = *s++) == '\0' || slen-- < 1)
//                  return (NULL);
//          } while (sc != c);
//          if (len > slen)
//              return (NULL);
//      } while (strncmp(s, text, len) != 0);
//      s--;
//  }
//  return ((char *)s);
// }


static char* IsCMD(char* payload) {
    return strstr(payload, "exin");
}

static int IsNotification(char* payload) {
    char* request = strstr(payload, "<m2m:rqp");
    char* cin = strstr(payload, "<cin");
    return request && cin;
}

static void UpdateExecInstance(char* nm, char* ri) {
    SKTDebugPrint(LOG_LEVEL_INFO, "UpdateExecInstance");
    char to[512] = "";
    snprintf(to, sizeof(to), TO_MGMTCMDRESULT, mToStart, nm+4, ri);
    tp_v1_14_Result(mAEID, to, "0", "3");
}

static void ProcessCMD(char* payload, int payloadLen) {
    SKTDebugPrint(LOG_LEVEL_INFO, "payload->");
    SKTDebugPrint(LOG_LEVEL_INFO, payload);
    char nm[128] = "";
    char exra[128] = "";
    char resourceId[23] = "";
    SimpleXmlParser(payload, ATTR_NM, nm, 1);
    SimpleXmlParser(payload, ATTR_EXRA, exra, 1);
    SimpleXmlParser(payload, ATTR_RI, resourceId, 1);
    UpdateExecInstance(nm, resourceId);
}

static void MQTTConnected(int result) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTConnected");
}

static void MQTTSubscribed(int result) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTSubscribed");
    DoVerificationStep();
}

static void MQTTDisconnected(int result) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTDisconnected");
}

static void MQTTConnectionLost(char* cause) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTConnectionLost");
}

static void MQTTMessageDelivered(int token) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTMessageDelivered");
}

static void MQTTMessageArrived(char* topic, char* msg, int msgLen) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTMessageArrived <%d>",msgLen);
    char *payload;
    payload = calloc(1,SIZE_PAYLOAD);
    memcpy(payload, msg, msgLen);
    SKTDebugPrint(LOG_LEVEL_INFO, "payload->");
    SKTDebugPrint(LOG_LEVEL_INFO, payload);

    if(IsCMD(msg)) {
        if(mStep == VERIFICATION_WAIT) {
            ProcessCMD(msg, msgLen);
        }
        return;
    }
    if(IsNotification(msg)) {
        return;
    }

    char rsc[SIZE_RESPONSE_CODE] = "";
    SimpleXmlParser(payload, ATTR_RSC, rsc, 0);
    char rsm[SIZE_RESPONSE_MESSAGE] = "";
    SimpleXmlParser(payload, ATTR_RSM, rsm, 0);

    switch(mStep) {
        case VERIFICATION_CSEBASE_RETRIEVE:
            mStep = VERIFICATION_AE_CREATE;
            break;
        case VERIFICATION_AE_CREATE:
            SimpleXmlParser(payload, ATTR_AEI, mAEID, 1);
            SimpleXmlParser(payload, ATTR_NL, mNodeLink, 1);
            if(strlen(mAEID) == 0 || strlen(mNodeLink) == 0) {
                mStep = VERIFICATION_END;
            } else {
                mStep = VERIFICATION_CONTAINER_CREATE;
            }
            break;
        case VERIFICATION_CONTAINER_CREATE:
            mStep = VERIFICATION_MGMTCMD_CREATE;
            break;
        case VERIFICATION_MGMTCMD_CREATE:
            mStep = VERIFICATION_SUBSCRIPTION_CREATE;
            break;
        case VERIFICATION_SUBSCRIPTION_CREATE:
            mStep = VERIFICATION_CONTENTINSTANCE_CREATE;
            break;
        case VERIFICATION_CONTENTINSTANCE_CREATE:
            SimpleXmlParser(payload, ATTR_RI, mContentInstanceResourceID, 1);
            if(strlen(mContentInstanceResourceID) == 0) {
                mStep = VERIFICATION_END;
            } else {
                if(DO_CREATE_ONLY) {
                    mStep = VERIFICATION_WAIT;
                } else {
                    mStep = VERIFICATION_AE_UPDATE;
                }
            }
            break;
        case VERIFICATION_AE_UPDATE:
            mStep = VERIFICATION_CONTAINER_UPDATE;
            break;
        case VERIFICATION_CONTAINER_UPDATE:
            mStep = VERIFICATION_MGMTCMD_UPDATE;
            break;
        case VERIFICATION_MGMTCMD_UPDATE:
            mStep = VERIFICATION_SUBSCRIPTION_UPDATE;
            break;
        case VERIFICATION_SUBSCRIPTION_UPDATE:
            mStep = VERIFICATION_SUBSCRIPTION_DELETE;
            break;
        case VERIFICATION_SUBSCRIPTION_DELETE:
            mStep = VERIFICATION_CONTENTINSTANCE_DELETE;
            break;
        case VERIFICATION_CONTENTINSTANCE_DELETE:
            mStep = VERIFICATION_CONTAINER_DELETE;
            break;
        case VERIFICATION_CONTAINER_DELETE:
            mStep = VERIFICATION_MGMTCMD_DELETE;
            break;
        case VERIFICATION_MGMTCMD_DELETE:
            mStep = VERIFICATION_MGMTCMD_FIRMWARE_DELETE;
            break;
        case VERIFICATION_MGMTCMD_FIRMWARE_DELETE:
            mStep = VERIFICATION_AE_DELETE;
            break;
        case VERIFICATION_AE_DELETE:
            mStep = VERIFICATION_END;
            break;
        default:
            break;
    }
    SKTDebugPrint(LOG_LEVEL_INFO, "=============================================================================");

    if(mStep != VERIFICATION_WAIT) {
        DoVerificationStep();
    }
    free(payload);
}

void SDKVerificationMain(void)
{
    SKTDebugInit(True, LOG_LEVEL_INFO,stdout);
    char str[256];

    SKTDebugPrint(LOG_LEVEL_INFO, "==========================================================================");
    SKTDebugPrint(LOG_LEVEL_INFO, "*** SDKVerification example ***");
    SKTDebugPrint(LOG_LEVEL_INFO, "==========================================================================");

    int rc;
    //set callbacks
    rc = tpMQTTSetCallbacks(MQTTConnected, MQTTSubscribed, MQTTDisconnected, MQTTConnectionLost, MQTTMessageDelivered, MQTTMessageArrived);
    sprintf(str,"tpMQTTSetCallbacks result : %d", rc);
    SKTDebugPrint(LOG_LEVEL_INFO, str);
    if(rc != 0) {
        goto result_error;
    }

    // create
    char subscribeTopic[TOPIC_SUBSCRIBE_SIZE][SIZE_TOPIC];
    char publishTopic[SIZE_TOPIC] = "";
    memset(subscribeTopic, 0, sizeof(subscribeTopic));
    snprintf(mClientID, sizeof(mClientID), MQTT_CLIENT_ID, ACCOUNT_USER_ID, ONEM2M_AE_NAME);
    sprintf(str,"client id : %s", mClientID);
    SKTDebugPrint(LOG_LEVEL_INFO, str);

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
    sprintf(str,"tpSDKCreate result : %d", rc);
    SKTDebugPrint(LOG_LEVEL_INFO, str);

    if(rc == 0) {
        while(mStep < VERIFICATION_END) {
            if(tpMQTTYield(1000) != 0)  // wait for seconds
            {
                osDelay (3000);
            }
        }
        tpMQTTDisconnect();
    }
 result_error:
    tpMQTTDestory();
}
