/**
 * @file SimpleSDKVerification.c
 *
 * @brief Simple SDK verification application
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "oneM2M.h"
#include "Configuration.h"
#ifdef ONEM2M_V1_12
#include "oneM2M_V1_12.h"
#else
#include "oneM2M_V1.h"
#endif

#include "SKTtpDebug.h"

#ifdef ONEM2M_V1_12
#define TOPIC_SUBSCRIBE_REQ                 "/oneM2M/req/%s/%s"
#define TOPIC_SUBSCRIBE_RES                 "/oneM2M/resp/%s/%s"
#define TOPIC_SUBSCRIBE_SIZE                2
// #define TOPIC_PUBLISH                       "/oneM2M/req/%s/%s"

#define TO_AE                               "%s/%s"
#define TO_CONTAINER                        "%s/%s/%s"
#define TO_CONTENTINSTANCE					"%s/%s/%s/cin-%s"
#define TO_MGMTCMDRESULT                    "%s/mgc-%s/exin-%s"
#else
#define TO_REMOTECSE                        "%s/remoteCSE-%s"
#define TO_NODE                             "%s/node-%s"
#define TO_CONTAINER                        "%s/remoteCSE-%s/container-%s"
#define TO_CONTENTINSTANCE                  "%s/remoteCSE-%s/container-%s/contentInstance-%s"
#define TO_AREANWKINFO                      "%s/node-%s/areaNwkInfo-%s"
#define TO_AE                               "%s/remoteCSE-%s/AE-%s"
#define TO_LOCATIONPOLICY                   "%s/locationPolicy-%s"
#define TO_MGMTCMD                          "%s/mgmtCmd-%s"
#define TO_MGMTCMDRESULT                    "%s/mgmtCmd-%s/execInstance-%s"

#define TOPIC_SUBSCRIBE_REQ                 "/oneM2M/req/+/%s"
#define TOPIC_SUBSCRIBE_RES                 "/oneM2M/resp/%s/+"
#define TOPIC_SUBSCRIBE_SIZE                2

#define NAME_MGA                            "MQTT|%s"

#define UKEY								"(TBD.)"
#endif

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
    VERIFICATION_CONTAINER_DELETE,
    VERIFICATION_REMOTECSE_DELETE,
    VERIFICATION_NODE_DELETE,

	VERIFICATION_ACCESSCONTROLPOLICY_CREATE,
	VERIFICATION_ACCESSCONTROLPOLICY_UPDATE,
	VERIFICATION_ACCESSCONTROLPOLICY_DELETE,
	
    VERIFICATION_END

};

static enum VERIFICATION_STEP mStep = VERIFICATION_CSEBASE_RETRIEVE;


#ifdef ONEM2M_V1_12
static char mAEID[128] = "";
static char mNodeLink[23] = "";
static char mContentInstanceResourceID[23] = "";
#else
static char mDeviceKey[128] = "";

static char mNodeLink[23] = "";
static char mHostCSELink[23] = "";
static char mRemoteCSEResourceName[128] = "";
static char mContentInstanceResourceName[128] = "";
static char mAEResourceName[128] = "";
static char mLocationPolicyResourceName[128] = "";
static char mMgmtCmdResourceName[128] = "";
static char mAreaNwkInfoResourceName[128] = "";
static char mContainerResourceName[128] = "";
#endif
static char mClientID[24] = "";

/**
 * @brief do verification step
 */
void DoVerificationStep() {
    int resourceType;
    int operation;
    char to[512] = "";
    char buffer[128] = "";
    char* ri = ONEM2M_RI;
    char* fr = NULL;
    void* pc = NULL;
    
    switch(mStep) {
#ifdef ONEM2M_V1_12
    // RETRIEVE
    case VERIFICATION_CSEBASE_RETRIEVE:
        {
        resourceType = CSEBase;
        operation = RETRIEVE;
        fr = APP_AEID;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        }
        break;
  	// CREATE
    case VERIFICATION_NODE_CREATE:
        {
        resourceType = node;
        operation = CREATE;
        fr = APP_AEID;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_v1_12_node* node = (oneM2M_v1_12_node *)calloc(sizeof(oneM2M_v1_12_node), 1);
        //memset(&node, 0, sizeof(node));
        node->rn = NAME_NODE;
        node->ni = "node_01";
        pc = (void *)node;
        } 
        break;		
    case VERIFICATION_REMOTECSE_CREATE:
        {
        resourceType = remoteCSE;
        operation = CREATE;
    	fr = "(TBD.)"; // registered CSE-ID
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_v1_12_remoteCSE* remoteCSE = (oneM2M_v1_12_remoteCSE *)calloc(sizeof(oneM2M_v1_12_remoteCSE), 1);
        remoteCSE->rn = NAME_REMOTECSE;
        remoteCSE->cb = ONEM2M_CB;
        remoteCSE->cst = "2";
        remoteCSE->csi = "(TBD.)"; // registered CSE-ID with slash
        remoteCSE->rr = "true";
        pc = (void *)remoteCSE;
        }
        break;
    case VERIFICATION_AE_CREATE:
        {
        resourceType = AE;
        operation = CREATE;
        fr = "S";
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_v1_12_AE* AE = (oneM2M_v1_12_AE *)calloc(sizeof(oneM2M_v1_12_AE), 1);
        //memset(&AE, 0, sizeof(AE));
        AE->rn = ONEM2M_AE_RESOURCENAME;
        AE->rr = "true";
		char poa[128] = "";
		snprintf(poa, sizeof(poa), "mqtt:///oneM2M/req/%s/%s", ONEM2M_SERVICENAME, ONEM2M_AE_RESOURCENAME);
        AE->poa = poa;
        AE->api = "middleware"; // pre-registered device name
        pc = (void *)AE;
        }
        break;
    case VERIFICATION_CONTAINER_CREATE:
        {
        resourceType = container;
        operation = CREATE;
        fr = mAEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, ONEM2M_AE_RESOURCENAME);
        oneM2M_v1_12_container* container = (oneM2M_v1_12_container *)calloc(sizeof(oneM2M_v1_12_container), 1);
        //memset(&container, 0, sizeof(container));
        container->rn = NAME_CONTAINER;
        pc = (void *)container;
        }
        break;
    case VERIFICATION_CONTENTINSTANCE_CREATE:
        {
        resourceType = contentInstance;
        operation = CREATE;
        fr = mAEID;
        snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, ONEM2M_AE_RESOURCENAME, NAME_CONTAINER);
        oneM2M_v1_12_contentInstance* contentInstance = (oneM2M_v1_12_contentInstance *)calloc(sizeof(oneM2M_v1_12_contentInstance), 1);
        //memset(&contentInstance, 0, sizeof(contentInstance));
        contentInstance->cnf = "text";
        contentInstance->con = "45";
        pc = (void *)contentInstance;
        }
        break;
    case VERIFICATION_MGMTCMD_CREATE:
        {
        resourceType = mgmtCmd;
        operation = CREATE;
        fr = APP_AEID;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_v1_12_mgmtCmd* mgmtCmd = (oneM2M_v1_12_mgmtCmd *)calloc(sizeof(oneM2M_v1_12_mgmtCmd), 1);
        //memset(&mgmtCmd, 0, sizeof(mgmtCmd));
        mgmtCmd->rn = NAME_MGMTCMD;
        mgmtCmd->cmt = "1";
        mgmtCmd->ext = mNodeLink;
        pc = (void *)mgmtCmd;
        }
        break;
    case VERIFICATION_LOCATIONPOLICY_CREATE:
        {
        resourceType = locationPolicy;
        operation = CREATE;
	    	fr = mAEID;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_v1_12_locationPolicy* locationPolicy = (oneM2M_v1_12_locationPolicy *)calloc(sizeof(oneM2M_v1_12_locationPolicy), 1);
		    locationPolicy->rn = NAME_LOCATIONPOLICY;
        locationPolicy->lon = "cnt-location";
        locationPolicy->lit = "1";
        locationPolicy->los = "2";
        pc = (void *)locationPolicy;
        }
        break;
    case VERIFICATION_ACCESSCONTROLPOLICY_CREATE:
    		{
        resourceType = accessControlPolicy;
        operation = CREATE;
		    fr = mAEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, ONEM2M_AE_RESOURCENAME);
        oneM2M_v1_12_accessControlPolicy* accessControlPolicy = (oneM2M_v1_12_accessControlPolicy *)calloc(sizeof(oneM2M_v1_12_accessControlPolicy), 1);
		    accessControlPolicy->rn = NAME_ACCESSCONTROLPOLICY;        
		    snprintf(buffer, sizeof(buffer), "<acr><acor>%s</acor><acop>63</acop></acr>", mAEID);
		    accessControlPolicy->pv = buffer;
		    //memset(buffer, 0, sizeof(buffer));
        accessControlPolicy->pvs = buffer;
        pc = (void *)accessControlPolicy;
        }
        break;

    // UPDATE
    case VERIFICATION_NODE_UPDATE:
        {
        resourceType = node;
        operation = UPDATE;
        fr = APP_AEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, NAME_NODE);
        oneM2M_v1_12_node* node = (oneM2M_v1_12_node *)calloc(sizeof(oneM2M_v1_12_node), 1);
        pc = (void *)node;
        }
        break;    
  case VERIFICATION_REMOTECSE_UPDATE:
        {
        resourceType = remoteCSE;
        operation = UPDATE;
        fr = "/middleware"; // registered CSE-ID
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, "csr-middleware");
        oneM2M_v1_12_remoteCSE* remoteCSE = (oneM2M_v1_12_remoteCSE *)calloc(sizeof(oneM2M_v1_12_remoteCSE), 1);
        remoteCSE->cb = "CB00045"; //ONEM2M_CB;
        remoteCSE->cst = "2";
        remoteCSE->csi = "/middleware"; // registered CSE-ID with slash
        remoteCSE->rr = "false";
//	    remoteCSE->nl = "ND00000000000000000046";		
        pc = (void *)remoteCSE;
        }
        break;
    case VERIFICATION_AE_UPDATE:
        {
        resourceType = AE;
        operation = UPDATE;
        fr = mAEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, ONEM2M_AE_RESOURCENAME);
        oneM2M_v1_12_AE* AE = (oneM2M_v1_12_AE *)calloc(sizeof(oneM2M_v1_12_AE), 1);
        AE->poa = "http://";
        pc = (void *)AE;
        }
        break;
    case VERIFICATION_CONTAINER_UPDATE:
        {
        resourceType = container;
        operation = UPDATE;
        fr = mAEID;
        snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, ONEM2M_AE_RESOURCENAME, NAME_CONTAINER);
        oneM2M_v1_12_container* container = (oneM2M_v1_12_container *)calloc(sizeof(oneM2M_v1_12_container), 1);
        container->lbl = "lbl";
        pc = (void *)container;
        }
        break;
    case VERIFICATION_MGMTCMD_UPDATE:
        {
        resourceType = mgmtCmd;
        operation = UPDATE;
        fr = APP_AEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, NAME_MGMTCMD);
        oneM2M_v1_12_mgmtCmd* mgmtCmd = (oneM2M_v1_12_mgmtCmd *)calloc(sizeof(oneM2M_v1_12_mgmtCmd), 1);
        mgmtCmd->dc = "test";
        pc = (void *)mgmtCmd;
        }
        break;
    case VERIFICATION_LOCATIONPOLICY_UPDATE:
        {
        resourceType = locationPolicy;
        operation = UPDATE;
        fr = APP_AEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, NAME_LOCATIONPOLICY);
        oneM2M_v1_12_locationPolicy* locationPolicy = (oneM2M_v1_12_locationPolicy *)calloc(sizeof(oneM2M_v1_12_locationPolicy), 1);
        locationPolicy->los = "1"; // locationSource : Network_based
        pc = (void *)locationPolicy;
        }
        break;
    case VERIFICATION_ACCESSCONTROLPOLICY_UPDATE:
        {
        resourceType = accessControlPolicy;
        operation = UPDATE;
        fr = mAEID;
        snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, ONEM2M_AE_RESOURCENAME, NAME_ACCESSCONTROLPOLICY);
        oneM2M_v1_12_accessControlPolicy* accessControlPolicy = (oneM2M_v1_12_accessControlPolicy *)calloc(sizeof(oneM2M_v1_12_accessControlPolicy), 1);
        snprintf(buffer, sizeof(buffer), "<acr><acor>%s</acor><acop>63</acop></acr>", mAEID);
        accessControlPolicy->pv = buffer;
        //memset(buffer, 0, sizeof(buffer));
        accessControlPolicy->pvs = buffer;
        pc = (void *)accessControlPolicy;
        }
        break;

    // DELETE
    case VERIFICATION_NODE_DELETE:
        {
        resourceType = node;
        operation = DELETE;
        fr = APP_AEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, NAME_NODE);
        }
        break;        
    case VERIFICATION_REMOTECSE_DELETE:
        {
        resourceType = remoteCSE;
        operation = DELETE;
        fr = "/middleware"; // registered CSE-ID
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, NAME_REMOTECSE);
        }
        break;
    case VERIFICATION_AE_DELETE:
        {
        resourceType = AE;
        operation = DELETE;
        fr = mAEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, ONEM2M_AE_RESOURCENAME);
        }
        break;
    case VERIFICATION_CONTAINER_DELETE:
        {
        resourceType = container;
        operation = DELETE;
        fr = mAEID;
        snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, ONEM2M_AE_RESOURCENAME, NAME_CONTAINER);
        }
        break;
	case VERIFICATION_CONTENTINSTANCE_DELETE:
		{
		resourceType = contentInstance;
		operation = DELETE;
		fr = mAEID;
		snprintf(to, sizeof(to), TO_CONTENTINSTANCE, ONEM2M_TO, ONEM2M_AE_RESOURCENAME, NAME_CONTAINER, mContentInstanceResourceID);
		}
		break;		
    case VERIFICATION_MGMTCMD_DELETE:		
        {
        resourceType = mgmtCmd;
        operation = DELETE;
        fr = APP_AEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, NAME_MGMTCMD);
        }
        break;
    case VERIFICATION_LOCATIONPOLICY_DELETE:
        {
        resourceType = locationPolicy;
        operation = DELETE;
        fr = APP_AEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, NAME_LOCATIONPOLICY);
        }
        break;
    case VERIFICATION_ACCESSCONTROLPOLICY_DELETE:
        {
        resourceType = accessControlPolicy;
        operation = DELETE;
        fr = mAEID;
        snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, ONEM2M_AE_RESOURCENAME, NAME_ACCESSCONTROLPOLICY);
        }
        break;
#else	
    // RETRIEVE
    case VERIFICATION_CSEBASE_RETRIEVE:
        {
        resourceType = CSEBase;
        operation = RETRIEVE;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_CSEBase CSEBase;
        memset(&CSEBase, 0, sizeof(CSEBase));
        CSEBase.ni = ONEM2M_NODEID;
        pc = (void *)&CSEBase;
        }
        break;

    // CREATE
    case VERIFICATION_NODE_CREATE:
        {
        resourceType = node;
        operation = CREATE;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_node node;
        memset(&node, 0, sizeof(node));
        node.ni = ONEM2M_NODEID;
        snprintf(buffer, sizeof(buffer), NAME_MGA, mClientID);
        node.mga = buffer;
        pc = (void *)&node;
        }
        break;
    case VERIFICATION_REMOTECSE_CREATE:
        {
        resourceType = remoteCSE;
        operation = CREATE;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_remoteCSE remoteCSE;
        memset(&remoteCSE, 0, sizeof(remoteCSE));
        remoteCSE.cst = "3";
        remoteCSE.ni = ONEM2M_NODEID;
        remoteCSE.nm = ONEM2M_NODEID;
        remoteCSE.passCode = ONEM2M_PASSCODE;
        // snprintf(buffer, sizeof(buffer), NAME_POA, ONEM2M_NODEID);
        // remoteCSE.poa = buffer;
        remoteCSE.rr = "true";
        remoteCSE.nl = mNodeLink;
        pc = (void *)&remoteCSE;
        }
        break;
    case VERIFICATION_CONTAINER_CREATE:
        {
        resourceType = container;
        operation = CREATE;
        snprintf(to, sizeof(to), TO_REMOTECSE, ONEM2M_TO, mRemoteCSEResourceName);
        oneM2M_container container;
        memset(&container, 0, sizeof(container));
        container.ni = ONEM2M_NODEID;
        snprintf(buffer, sizeof(buffer), NAME_CONTAINER, ONEM2M_NODEID);
        container.nm = buffer;
        container.dKey = mDeviceKey;
        container.lbl = "con";
        pc = (void *)&container;
        }
        break;
    case VERIFICATION_MGMTCMD_CREATE:
        {
        resourceType = mgmtCmd;
        operation = CREATE;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_mgmtCmd mgmtCmd;
        memset(&mgmtCmd, 0, sizeof(mgmtCmd));
        mgmtCmd.ni = ONEM2M_NODEID;
        snprintf(buffer, sizeof(buffer), NAME_MGMTCMD, ONEM2M_NODEID, CMT_DEVRESET);
        mgmtCmd.nm = buffer;
        mgmtCmd.dKey = mDeviceKey;
        mgmtCmd.cmt = "sensor_1";
        mgmtCmd.exe = "false";
        mgmtCmd.ext = mNodeLink;
        mgmtCmd.lbl = ONEM2M_NODEID;
        pc = (void *)&mgmtCmd;
        }
        break;
    case VERIFICATION_CONTENTINSTANCE_CREATE:
        {
        resourceType = contentInstance;
        operation = CREATE;
        snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, mRemoteCSEResourceName, mContainerResourceName);
        oneM2M_contentInstance contentInstance;
        memset(&contentInstance, 0, sizeof(contentInstance));
        contentInstance.ni = ONEM2M_NODEID;
        contentInstance.dKey = mDeviceKey;
        contentInstance.cnf = "text";
        contentInstance.con = "45";
        pc = (void *)&contentInstance;
        }
        break;
    case VERIFICATION_LOCATIONPOLICY_CREATE:
        {
        resourceType = locationPolicy;
        operation = CREATE;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_locationPolicy locationPolicy;
        memset(&locationPolicy, 0, sizeof(locationPolicy));
        locationPolicy.ni = ONEM2M_NODEID;
        snprintf(buffer, sizeof(buffer), NAME_LOCATIONPOLICY, ONEM2M_NODEID);
        locationPolicy.nm = buffer;
        locationPolicy.dKey = mDeviceKey;
        locationPolicy.los = "2";
        pc = (void *)&locationPolicy;
        }
        break;
    case VERIFICATION_AE_CREATE:
        {
        resourceType = AE;
        operation = CREATE;
        snprintf(to, sizeof(to), TO_REMOTECSE, ONEM2M_TO, mRemoteCSEResourceName);
        oneM2M_AE AE;
        memset(&AE, 0, sizeof(AE));
        AE.ni = ONEM2M_NODEID;
        AE.dKey = mDeviceKey;
        AE.api= "1.2.481.1.0001.002.1234";
        snprintf(buffer, sizeof(buffer), NAME_AE, ONEM2M_NODEID);
        AE.apn= buffer;
        pc = (void *)&AE;
        }
        break;
    case VERIFICATION_AREANWKINFO_CREATE:
        {
        resourceType = mgmtObj;
        operation = CREATE;
        snprintf(to, sizeof(to), TO_NODE, ONEM2M_TO, ONEM2M_NODEID);
        oneM2M_areaNwkInfo areaNwkInfo;
        memset(&areaNwkInfo, 0, sizeof(areaNwkInfo));
        areaNwkInfo.ni = ONEM2M_NODEID;
        snprintf(buffer, sizeof(buffer), NAME_AREANWKINFO, ONEM2M_NODEID);
        areaNwkInfo.nm = buffer;
        areaNwkInfo.dKey = mDeviceKey;
        areaNwkInfo.mgd = "1004";
        areaNwkInfo.ant = "type";
        areaNwkInfo.ldv = "";
        pc = (void *)&areaNwkInfo;
        }
        break;

    // UPDATE
    case VERIFICATION_AREANWKINFO_UPDATE:
        {
        resourceType = mgmtObj;
        operation = UPDATE;
        snprintf(to, sizeof(to), TO_AREANWKINFO, ONEM2M_TO, ONEM2M_NODEID, mAreaNwkInfoResourceName);
        oneM2M_areaNwkInfo areaNwkInfo;
        memset(&areaNwkInfo, 0, sizeof(areaNwkInfo));
        areaNwkInfo.ni = ONEM2M_NODEID;
        snprintf(buffer, sizeof(buffer), NAME_AREANWKINFO, ONEM2M_NODEID);
        areaNwkInfo.nm = buffer;
        areaNwkInfo.dKey = mDeviceKey;
        areaNwkInfo.ant = "type2";
        areaNwkInfo.ldv = "1";
        pc = (void *)&areaNwkInfo;
        }
        break;
    case VERIFICATION_AE_UPDATE:
        {
        resourceType = AE;
        operation = UPDATE;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, mRemoteCSEResourceName, mAEResourceName);
        oneM2M_AE AE;
        memset(&AE, 0, sizeof(AE));
        AE.ni = ONEM2M_NODEID;
        AE.dKey = mDeviceKey;
        snprintf(buffer, sizeof(buffer), "%s_AE_02", ONEM2M_NODEID);
        AE.apn= buffer;
        pc = (void *)&AE;
        }
        break;
    case VERIFICATION_LOCATIONPOLICY_UPDATE:
        {
        resourceType = locationPolicy;
        operation = UPDATE;
        snprintf(to, sizeof(to), TO_LOCATIONPOLICY, ONEM2M_TO, mLocationPolicyResourceName);
        oneM2M_locationPolicy locationPolicy;
        memset(&locationPolicy, 0, sizeof(locationPolicy));
        locationPolicy.ni = ONEM2M_NODEID;
        locationPolicy.dKey = mDeviceKey;
        locationPolicy.los = "3";
        locationPolicy.lbl = ONEM2M_NODEID; 
        pc = (void *)&locationPolicy;
        }
        break;
    case VERIFICATION_MGMTCMD_UPDATE:
        {
        resourceType = mgmtCmd;
        operation = UPDATE;
        snprintf(to, sizeof(to), TO_MGMTCMD, ONEM2M_TO, mMgmtCmdResourceName);
        oneM2M_mgmtCmd mgmtCmd;
        memset(&mgmtCmd, 0, sizeof(mgmtCmd));
        mgmtCmd.ni = ONEM2M_NODEID;
        mgmtCmd.uKey = UKEY;
        mgmtCmd.exe = "true";
//        mgmtCmd.lbl = ONEM2M_NODEID;
        mgmtCmd.ext = mNodeLink;//mNodeLink;
        pc = (void *)&mgmtCmd;
        }
        break;
    case VERIFICATION_CONTAINER_UPDATE:
        {
        resourceType = container;
        operation = UPDATE;
        snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, mRemoteCSEResourceName, mContainerResourceName);
        oneM2M_container container;
        memset(&container, 0, sizeof(container));
        container.ni = ONEM2M_NODEID;
        container.dKey = mDeviceKey;
        container.lbl = "event";
        pc = (void *)&container;
        }
        break;
    case VERIFICATION_REMOTECSE_UPDATE:
        {
        resourceType = remoteCSE;
        operation = UPDATE;
        snprintf(to, sizeof(to), TO_REMOTECSE, ONEM2M_TO, mRemoteCSEResourceName);
        oneM2M_remoteCSE remoteCSE;
        memset(&remoteCSE, 0, sizeof(remoteCSE));
        remoteCSE.ni = ONEM2M_NODEID;
        remoteCSE.dKey = mDeviceKey;
        pc = (void *)&remoteCSE;
        }
        break;
    case VERIFICATION_NODE_UPDATE:
        {
        resourceType = node;
        operation = UPDATE;
        snprintf(to, sizeof(to), TO_NODE, ONEM2M_TO, ONEM2M_NODEID);
        oneM2M_node node;
        memset(&node, 0, sizeof(node));
        node.ni = ONEM2M_NODEID;
        node.dKey = mDeviceKey;
        snprintf(buffer, sizeof(buffer), NAME_MGA, mClientID);
        node.mga = buffer;
        // node.hcl = mHostCSELink;
        pc = (void *)&node;
        }
        break;

    // DELETE
    case VERIFICATION_AREANWKINFO_DELETE:
        {
        resourceType = mgmtObj;
        operation = DELETE;
        snprintf(to, sizeof(to), TO_AREANWKINFO, ONEM2M_TO, ONEM2M_NODEID, mAreaNwkInfoResourceName);
        oneM2M_areaNwkInfo areaNwkInfo;
        memset(&areaNwkInfo, 0, sizeof(areaNwkInfo));
        areaNwkInfo.ni = ONEM2M_NODEID;
        areaNwkInfo.dKey = mDeviceKey;
        pc = (void *)&areaNwkInfo;            
        }
        break;
    case VERIFICATION_AE_DELETE:
        resourceType = AE;
        operation = DELETE;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, mRemoteCSEResourceName, mAEResourceName);
        oneM2M_AE AE;
        memset(&AE, 0, sizeof(AE));
        AE.ni = ONEM2M_NODEID;
        AE.dKey = mDeviceKey;
        pc = (void *)&AE;
        break;
    case VERIFICATION_LOCATIONPOLICY_DELETE:
        resourceType = locationPolicy;
        operation = DELETE;
        snprintf(to, sizeof(to), TO_LOCATIONPOLICY, ONEM2M_TO, mLocationPolicyResourceName);
        oneM2M_locationPolicy locationPolicy;
        memset(&locationPolicy, 0, sizeof(locationPolicy));
        locationPolicy.ni = ONEM2M_NODEID;
        locationPolicy.dKey = mDeviceKey;
        pc = (void *)&locationPolicy;
        break;
    case VERIFICATION_CONTENTINSTANCE_DELETE:
        resourceType = contentInstance;
        operation = DELETE;
        snprintf(to, sizeof(to), TO_CONTENTINSTANCE, ONEM2M_TO, mRemoteCSEResourceName, mContainerResourceName, mContentInstanceResourceName);
        oneM2M_contentInstance contentInstance;
        memset(&contentInstance, 0, sizeof(contentInstance));
        contentInstance.ni = ONEM2M_NODEID;
        contentInstance.dKey = mDeviceKey;
        pc = (void *)&contentInstance;
        break;
    case VERIFICATION_MGMTCMD_DELETE:
        resourceType = mgmtCmd;
        operation = DELETE;
        snprintf(to, sizeof(to), TO_MGMTCMD, ONEM2M_TO, mMgmtCmdResourceName);
        oneM2M_mgmtCmd mgmtCmd;
        memset(&mgmtCmd, 0, sizeof(mgmtCmd));
        mgmtCmd.ni = ONEM2M_NODEID;
        mgmtCmd.dKey = mDeviceKey;
        pc = (void *)&mgmtCmd;
        break;
    case VERIFICATION_CONTAINER_DELETE:
        resourceType = container;
        operation = DELETE;
        snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, mRemoteCSEResourceName, mContainerResourceName);
        oneM2M_container container;
        memset(&container, 0, sizeof(container));
        container.ni = ONEM2M_NODEID;
        container.dKey = mDeviceKey;
        pc = (void *)&container;
        break;
    case VERIFICATION_REMOTECSE_DELETE:
        resourceType = remoteCSE;
        operation = DELETE;
        snprintf(to, sizeof(to), TO_REMOTECSE, ONEM2M_TO, mRemoteCSEResourceName);
        oneM2M_remoteCSE remoteCSE;
        memset(&remoteCSE, 0, sizeof(remoteCSE));
        remoteCSE.ni = ONEM2M_NODEID;
        remoteCSE.dKey = mDeviceKey;
        pc = (void *)&remoteCSE;
        break;
    case VERIFICATION_NODE_DELETE:
        resourceType = node;
        operation = DELETE;
        snprintf(to, sizeof(to), TO_NODE, ONEM2M_TO, ONEM2M_NODEID);
        oneM2M_node node;
        memset(&node, 0, sizeof(node));
        node.ni = ONEM2M_NODEID;
        pc = (void *)&node;
        break;
#endif		
    default:
        {
        mStep = VERIFICATION_END;
        }
        break;
    }
    
    if(mStep < VERIFICATION_END) {
#ifdef ONEM2M_V1_12
        tp_oneM2M_V1_12_Request(resourceType, operation, fr, to, ri, pc);
        if(pc) free(pc);
#else
        if(pc != NULL) {
            tp_oneM2M_V1_Request(resourceType, operation, to, ri, pc);
        } 
#endif
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
    }

    snprintf(start, sizeof(start), "<%s>", name);
    snprintf(end, sizeof(end), "</%s>", name);
    char* s = strstr(pl, start) + strlen(start);
    char* e = strstr(pl, end);

    if(s && e) {
        memcpy(value, s, e-s);
        SKTDebugPrint(LOG_LEVEL_INFO, "[%s : %s]", name, value);
    }    
    return rc;
}


void MQTTConnected(int result) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTConnected result : %d", result);    
}

void MQTTSubscribed(int result) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTSubscribed result : %d", result);
    DoVerificationStep();
}

void MQTTDisconnected(int result) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTDisconnected result : %d", result);
}

void MQTTConnectionLost(char* cause) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTConnectionLost result : %s", cause);
}

void MQTTMessageDelivered(int token) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTMessageDelivered token : %d, step : %d", token, mStep);
}

void MQTTMessageArrived(char* topic, char* msg, int msgLen) {
    SKTDebugPrint(LOG_LEVEL_INFO, "MQTTMessageArrived topic : %s, step : %d", topic, mStep);
    char payload[1024] = "";
    memcpy(payload, msg, msgLen);
    SKTDebugPrint(LOG_LEVEL_INFO, "payload : %s", payload);
    char rsc[10] = "";
    SimpleXmlParser(payload, ATTR_RSC, rsc, 0);
    char rsm[128] = "";
    SimpleXmlParser(payload, ATTR_RSM, rsm, 0);

#ifdef ONEM2M_V1_12
    switch(mStep) {
        case VERIFICATION_CSEBASE_RETRIEVE:
            mStep = VERIFICATION_NODE_CREATE;
            break;
   		case VERIFICATION_NODE_CREATE:
            SimpleXmlParser(payload, ATTR_RI, mNodeLink, 1);
            if(strlen(mNodeLink) == 0) {
                mStep = VERIFICATION_END;
            } else {
                mStep = VERIFICATION_AE_CREATE;
            }
            break;
	   case VERIFICATION_REMOTECSE_CREATE:
			break;
       case VERIFICATION_AE_CREATE:
            SimpleXmlParser(payload, ATTR_AEI, mAEID, 1);
            if(strlen(mAEID) == 0) {
                mStep = VERIFICATION_END;
            } else {
                mStep = VERIFICATION_CONTAINER_CREATE;
            }
            break;
        case VERIFICATION_CONTAINER_CREATE:
            mStep = VERIFICATION_MGMTCMD_CREATE;
            break;
	    case VERIFICATION_MGMTCMD_CREATE:
	        mStep = VERIFICATION_ACCESSCONTROLPOLICY_CREATE;			
	        break;
	    case VERIFICATION_LOCATIONPOLICY_CREATE:
    	    mStep = VERIFICATION_CONTENTINSTANCE_CREATE;			
        	break;
	    case VERIFICATION_ACCESSCONTROLPOLICY_CREATE:
        	mStep = VERIFICATION_LOCATIONPOLICY_CREATE;			
    	    break;			
	    case VERIFICATION_CONTENTINSTANCE_CREATE:
			SimpleXmlParser(payload, ATTR_RI, mContentInstanceResourceID, 1);
			if(strlen(mContentInstanceResourceID) == 0) {
				mStep = VERIFICATION_END;
			} else {
	            mStep = VERIFICATION_NODE_UPDATE;
			}
            break;
        case VERIFICATION_NODE_UPDATE:
            mStep = VERIFICATION_AE_UPDATE;
            break;
        case VERIFICATION_REMOTECSE_UPDATE:
            break;
        case VERIFICATION_AE_UPDATE:
            mStep = VERIFICATION_CONTAINER_UPDATE;
            break;    
        case VERIFICATION_CONTAINER_UPDATE:
            mStep = VERIFICATION_MGMTCMD_UPDATE;
            break;
        case VERIFICATION_MGMTCMD_UPDATE:
            mStep = VERIFICATION_LOCATIONPOLICY_UPDATE;
            break;
        case VERIFICATION_LOCATIONPOLICY_UPDATE:
            mStep = VERIFICATION_ACCESSCONTROLPOLICY_UPDATE;
            break;    
        case VERIFICATION_ACCESSCONTROLPOLICY_UPDATE:
            mStep = VERIFICATION_LOCATIONPOLICY_DELETE;
            break;
        case VERIFICATION_NODE_DELETE:
            mStep = VERIFICATION_END;
            break;
        case VERIFICATION_REMOTECSE_DELETE:
			mStep = VERIFICATION_END;
            break;
        case VERIFICATION_AE_DELETE:
            mStep = VERIFICATION_NODE_DELETE;
            break;
        case VERIFICATION_CONTAINER_DELETE:
            mStep = VERIFICATION_ACCESSCONTROLPOLICY_DELETE;
            break;
		case VERIFICATION_CONTENTINSTANCE_DELETE:
            mStep = VERIFICATION_CONTAINER_DELETE;
			break;
        case VERIFICATION_MGMTCMD_DELETE:
            mStep = VERIFICATION_CONTENTINSTANCE_DELETE;
            break;
        case VERIFICATION_LOCATIONPOLICY_DELETE:
            mStep = VERIFICATION_MGMTCMD_DELETE;
            break;
        case VERIFICATION_ACCESSCONTROLPOLICY_DELETE:
            mStep = VERIFICATION_AE_DELETE;
            break;
        default:
            break;
    }
    SKTDebugPrint(LOG_LEVEL_INFO, "=============================================================================");    
	DoVerificationStep();
#else
    switch(mStep) {
        case VERIFICATION_NODE_CREATE:
            {
            SimpleXmlParser(payload, ATTR_RI, mNodeLink, 1);
            SimpleXmlParser(payload, ATTR_HCL, mHostCSELink, 1);
            if(strlen(mNodeLink) == 0) {
                mStep = VERIFICATION_END;
            }
            }
            break;
            
        case VERIFICATION_REMOTECSE_CREATE:
            {
            SimpleXmlParser(payload, ATTR_DKEY, mDeviceKey, 0);
            SimpleXmlParser(payload, ATTR_RN, mRemoteCSEResourceName, 0);
            if(strlen(mDeviceKey) == 0 || strlen(mRemoteCSEResourceName) == 0) {
                mStep = VERIFICATION_END;
            }
            }
            break;
        case VERIFICATION_LOCATIONPOLICY_CREATE:
            {
            SimpleXmlParser(payload, ATTR_RN, mLocationPolicyResourceName, 1);
            if(strlen(mLocationPolicyResourceName) == 0) {
                mStep = VERIFICATION_END;
            }
            }
            break;
        case VERIFICATION_AE_CREATE:
            {
            SimpleXmlParser(payload, ATTR_RN, mAEResourceName, 1);
            if(strlen(mAEResourceName) == 0) {
                mStep = VERIFICATION_END;
            }
            }
            break;
        case VERIFICATION_CONTENTINSTANCE_CREATE:
            {
            SimpleXmlParser(payload, ATTR_RN, mContentInstanceResourceName, 1);
            if(strlen(mContentInstanceResourceName) == 0) {
                mStep = VERIFICATION_END;
            }
            }
            break;
        case VERIFICATION_MGMTCMD_CREATE:
            {
            SimpleXmlParser(payload, ATTR_RN, mMgmtCmdResourceName, 1);
            if(strlen(mMgmtCmdResourceName) == 0) {
                mStep = VERIFICATION_END;
            }
            }
            break;
        case VERIFICATION_AREANWKINFO_CREATE:
            {
            SimpleXmlParser(payload, ATTR_RN, mAreaNwkInfoResourceName, 1);
            if(strlen(mAreaNwkInfoResourceName) == 0) {
                mStep = VERIFICATION_END;
            }
            }
            break;
        case VERIFICATION_CONTAINER_CREATE:
            {
            SimpleXmlParser(payload, ATTR_RN, mContainerResourceName, 1);
            if(strlen(mContainerResourceName) == 0) {
                mStep = VERIFICATION_END;
            }
            }
            break;
		case VERIFICATION_NODE_UPDATE:
			{
				mStep = VERIFICATION_END;
			}
			break;
			
        default:
            break;
    }
    SKTDebugPrint(LOG_LEVEL_INFO, "=============================================================================");    
    mStep++;
    DoVerificationStep();
#endif    

}

/**
 * @brief     get Device MAC Address without Colon.
 *
 * @return    mac address
 */
char* GetMacAddressWithoutColon() {
    int i, sock;
    struct ifreq ifr;
    char mac_adr[18] = "";

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return NULL;
    }

    strcpy(ifr.ifr_name, "eth0");
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
        close(sock);
        return NULL;
    }

    for (i = 0; i < 6; i++) {
        sprintf(&mac_adr[i*2],"%02X",((unsigned char*)ifr.ifr_hwaddr.sa_data)[i]);
    }
    close(sock);

    return strdup(mac_adr);
}

/**
 * @brief main
 * @param[in] argc
 * @param[in] argv
 */
int main(int argc, char **argv) {
    SKTDebugInit(1, LOG_LEVEL_INFO, NULL);
    int rc;

    // set callbacks
    rc = tpMQTTSetCallbacks(MQTTConnected, MQTTSubscribed, MQTTDisconnected, MQTTConnectionLost, MQTTMessageDelivered, MQTTMessageArrived);
    SKTDebugPrint(LOG_LEVEL_INFO, "tpMQTTSetCallbacks result : %d", rc);

    // create
    char subscribeTopic[2][128];
    char publishTopic[128] = "";
    // char clientID[24] = "";
    char* mac = GetMacAddressWithoutColon();
    memset(subscribeTopic, 0, sizeof(subscribeTopic));
    snprintf(mClientID, sizeof(mClientID), "%s_%s", ACCOUNT_USER, mac+8);
    if(mac) free(mac);
#ifdef ONEM2M_V1_12
    snprintf(subscribeTopic[0], sizeof(subscribeTopic[0]), TOPIC_SUBSCRIBE_REQ, ONEM2M_SERVICENAME, ONEM2M_AE_RESOURCENAME);
    snprintf(subscribeTopic[1], sizeof(subscribeTopic[1]), TOPIC_SUBSCRIBE_RES, ONEM2M_AE_RESOURCENAME, ONEM2M_SERVICENAME);
    snprintf(publishTopic, sizeof(publishTopic), TOPIC_PUBLISH, ONEM2M_AE_RESOURCENAME, ONEM2M_SERVICENAME);
#else
    snprintf(subscribeTopic[0], sizeof(subscribeTopic[0]), TOPIC_SUBSCRIBE_REQ, mClientID);
    snprintf(subscribeTopic[1], sizeof(subscribeTopic[1]), TOPIC_SUBSCRIBE_RES, mClientID);
    snprintf(publishTopic, sizeof(publishTopic), TOPIC_PUBLISH, mClientID, ONEM2M_CSEBASE);
#endif
    char* st[] = {subscribeTopic[0], subscribeTopic[1]};

	int port = (!MQTT_ENABLE_SERVER_CERT_AUTH ? MQTT_PORT : MQTT_SECURE_PORT);
	rc = tpSDKCreate(MQTT_HOST, port, MQTT_KEEP_ALIVE, ACCOUNT_USER, ACCOUNT_PASSWORD, MQTT_ENABLE_SERVER_CERT_AUTH, st, TOPIC_SUBSCRIBE_SIZE, publishTopic, mClientID);
    SKTDebugPrint(LOG_LEVEL_INFO, "tpSDKCreate result : %d", rc);

    while (mStep < VERIFICATION_END) {
        #if defined(WIN32) || defined(WIN64)
            Sleep(100);
        #else
            usleep(10000L);
        #endif
    }
    tpSDKDestory();
    return 0;
}
