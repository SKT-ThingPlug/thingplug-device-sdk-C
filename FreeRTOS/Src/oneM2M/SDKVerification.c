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
#ifdef ONEM2M_V1_12
#include "oneM2M_V1_12.h"
#else
#include "oneM2M_V1.h"
#endif
#include "SKTtpDebug.h"
#include "cmsis_os.h"


#ifdef ONEM2M_V1_12
#define TOPIC_SUBSCRIBE_REQ                 "/oneM2M/req/%s/%s" // FIXME
#define TOPIC_SUBSCRIBE_RES                 "/oneM2M/resp/%s/%s"
#define TOPIC_SUBSCRIBE_SIZE                2
//#define TOPIC_PUBLISH                       "/oneM2M/req/%s/%s"

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
//#define TOPIC_PUBLISH                       "/oneM2M/req/%s/ThingPlug"

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


#define ONEM2M_MEM_ALLOC(x, length, type)	x = (type*)calloc(length, sizeof(type));
#define ONEM2M_DEF_N_MEM_ALLOC(type, x)		type* x=NULL; ONEM2M_MEM_ALLOC(x, 1, type);
#define ONEM2M_MEM_FREE(x)					if(x!=NULL) { free(x); x=NULL; }

#define DEFAULT_BUF_SIZE					128

static enum VERIFICATION_STEP mStep = VERIFICATION_CSEBASE_RETRIEVE;

#ifdef ONEM2M_V1_12
static char mAEID[128] = "";
static char mNodeLink[23] = "";
#else
static char mDeviceKey[DEFAULT_BUF_SIZE];

static char mNodeLink[DEFAULT_BUF_SIZE];
static char mHostCSELink[DEFAULT_BUF_SIZE];
static char mRemoteCSEResourceName[DEFAULT_BUF_SIZE];
static char mContentInstanceResourceName[DEFAULT_BUF_SIZE];
static char mAEResourceName[DEFAULT_BUF_SIZE];
static char mLocationPolicyResourceName[DEFAULT_BUF_SIZE];
static char mMgmtCmdResourceName[DEFAULT_BUF_SIZE];
static char mAreaNwkInfoResourceName[DEFAULT_BUF_SIZE];
static char mContainerResourceName[DEFAULT_BUF_SIZE];
#endif
static char mClientID[24] = "";

/**
 * @brief do verification step
 */
void DoVerificationStep() {
	int resourceType;
	int operation;
	char to[DEFAULT_BUF_SIZE];
	char buffer[DEFAULT_BUF_SIZE];
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
		fr = ""; // registered CSE-ID
		memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
		oneM2M_v1_12_remoteCSE* remoteCSE = (oneM2M_v1_12_remoteCSE *)calloc(sizeof(oneM2M_v1_12_remoteCSE), 1);
		remoteCSE->rn = NAME_REMOTECSE;
		remoteCSE->cb = ONEM2M_CB;
		remoteCSE->cst = "2";
		remoteCSE->csi = "/"; // registered CSE-ID with slash
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
		fr = ""; // registered CSE-ID
		snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, NAME_REMOTECSE);
		oneM2M_v1_12_remoteCSE* remoteCSE = (oneM2M_v1_12_remoteCSE *)calloc(sizeof(oneM2M_v1_12_remoteCSE), 1);
		remoteCSE->acpi = "";
		remoteCSE->nl = "";
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
		fr = ""; // registered CSE-ID
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
		sprintf(to, ONEM2M_TO);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_CSEBase, pCSEBase);
		pCSEBase->ni = ONEM2M_NODEID;
		pc = (void *)pCSEBase;
		}
		break;

	// CREATE
	case VERIFICATION_NODE_CREATE:
		{
		resourceType = node;
		operation = CREATE;
		sprintf(to, ONEM2M_TO);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_node, pNode);
		pNode->ni = ONEM2M_NODEID;
		sprintf(buffer, NAME_MGA, mClientID);
		pNode->mga = buffer;
		pc = (void *)pNode;
		}
		break;
	case VERIFICATION_REMOTECSE_CREATE:
		{
		resourceType = remoteCSE;
		operation = CREATE;
		sprintf(to, ONEM2M_TO);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_remoteCSE, pRemoteCSE);
		pRemoteCSE->cst = "3";
		pRemoteCSE->ni = ONEM2M_NODEID;
		pRemoteCSE->nm = ONEM2M_NODEID;
		pRemoteCSE->passCode = ONEM2M_PASSCODE;
//		sprintf(buffer, , ONEM2M_NODEID);
//		pRemoteCSE->poa = buffer;
		pRemoteCSE->rr = "true";
		pRemoteCSE->nl = mNodeLink;
		pc = (void *)pRemoteCSE;
		}
		break;
	case VERIFICATION_CONTAINER_CREATE:
		{
		resourceType = container;
		operation = CREATE;
		sprintf(to, TO_REMOTECSE, ONEM2M_TO, mRemoteCSEResourceName);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_container, pContainer);
		pContainer->ni = ONEM2M_NODEID;
//		sprintf(buffer, NAME_CONTAINER, ONEM2M_NODEID);
		pContainer->nm = NAME_CONTAINER;
		pContainer->dKey = mDeviceKey;
		pContainer->lbl = "con";
		pc = (void *)pContainer;
		}
		break;
	case VERIFICATION_MGMTCMD_CREATE:
		{
		resourceType = mgmtCmd;
		operation = CREATE;
		sprintf(to, ONEM2M_TO);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_mgmtCmd, pMgmtCmd);
		pMgmtCmd->ni = ONEM2M_NODEID;
		sprintf(buffer, NAME_MGMTCMD, ONEM2M_NODEID, "mgmtCmd_01");
		pMgmtCmd->nm = buffer;
		pMgmtCmd->dKey = mDeviceKey;
		pMgmtCmd->cmt = "sensor_1";
		pMgmtCmd->exe = "false";
		pMgmtCmd->ext = mNodeLink;
		pMgmtCmd->lbl = ONEM2M_NODEID;
		pc = (void *)pMgmtCmd;
		}
		break;
	case VERIFICATION_CONTENTINSTANCE_CREATE:
		{
		resourceType = contentInstance;
		operation = CREATE;
		sprintf(to, TO_CONTAINER, ONEM2M_TO, mRemoteCSEResourceName, mContainerResourceName);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_contentInstance, pContentInstance);
		pContentInstance->ni = ONEM2M_NODEID;
		pContentInstance->dKey = mDeviceKey;
//		pContentInstance->cnf = "LoRa/Sensor";
//		pContentInstance->con = "{\"time\" : \"20000000\", \"value\" : \"500\"}";		//getLoRaInstance();
		pContentInstance->cnf = "text";
		pContentInstance->con = "45";
		pc = (void *)pContentInstance;
		}
		break;
	case VERIFICATION_LOCATIONPOLICY_CREATE:
		{
		resourceType = locationPolicy;
		operation = CREATE;
		sprintf(to, ONEM2M_TO);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_locationPolicy, pLocationPolicy);
		pLocationPolicy->ni = ONEM2M_NODEID;
		sprintf(buffer, NAME_LOCATIONPOLICY, ONEM2M_NODEID);
		pLocationPolicy->nm = buffer;
		pLocationPolicy->dKey = mDeviceKey;
		pLocationPolicy->los = "2";
		pc = (void *)pLocationPolicy;
		}
		break;
	case VERIFICATION_AE_CREATE:
		{
		resourceType = AE;
		operation = CREATE;
		sprintf(to, TO_REMOTECSE, ONEM2M_TO, mRemoteCSEResourceName);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_AE, pAE);
		pAE->ni = ONEM2M_NODEID;
		pAE->dKey = mDeviceKey;
		pAE->api= "1.2.481.1.0001.002.1234";
		sprintf(buffer, NAME_AE, ONEM2M_NODEID);
		pAE->apn= buffer;
		pc = (void *)pAE;
		}
		break;
	case VERIFICATION_AREANWKINFO_CREATE:
		{
		resourceType = mgmtObj;
		operation = CREATE;
		sprintf(to, TO_NODE, ONEM2M_TO, ONEM2M_NODEID);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_areaNwkInfo, pAreaNwkInfo);
		pAreaNwkInfo->ni = ONEM2M_NODEID;
		sprintf(buffer, NAME_AREANWKINFO, ONEM2M_NODEID);
		pAreaNwkInfo->nm = buffer;
		pAreaNwkInfo->dKey = mDeviceKey;
		pAreaNwkInfo->mgd = "1004";
		pAreaNwkInfo->ant = "type";
		pAreaNwkInfo->ldv = "";
		pc = (void *)pAreaNwkInfo;
		}
		break;

	// UPDATE
	case VERIFICATION_AREANWKINFO_UPDATE:
		{
		resourceType = mgmtObj;
		operation = UPDATE;
		sprintf(to, TO_AREANWKINFO, ONEM2M_TO, ONEM2M_NODEID, mAreaNwkInfoResourceName);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_areaNwkInfo, pAreaNwkInfo);
		pAreaNwkInfo->ni = ONEM2M_NODEID;
		sprintf(buffer, NAME_AREANWKINFO, ONEM2M_NODEID);
		pAreaNwkInfo->nm = buffer;
		pAreaNwkInfo->dKey = mDeviceKey;
		pAreaNwkInfo->ant = "type2";
		pAreaNwkInfo->ldv = "1";
		pc = (void *)pAreaNwkInfo;
		}
		break;
	case VERIFICATION_AE_UPDATE:
		{
		resourceType = AE;
		operation = UPDATE;
		sprintf(to, TO_AE, ONEM2M_TO, mRemoteCSEResourceName, mAEResourceName);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_AE, pAE);
		pAE->ni = ONEM2M_NODEID;
		pAE->dKey = mDeviceKey;
		sprintf(buffer, "%s_AE_02", ONEM2M_NODEID);
		pAE->apn= buffer;
		pc = (void *)pAE;
		}
		break;
	case VERIFICATION_LOCATIONPOLICY_UPDATE:
		{
		resourceType = locationPolicy;
		operation = UPDATE;
		sprintf(to, TO_LOCATIONPOLICY, ONEM2M_TO, mLocationPolicyResourceName);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_locationPolicy, pLocationPolicy);
		pLocationPolicy->ni = ONEM2M_NODEID;
		pLocationPolicy->dKey = mDeviceKey;
		pLocationPolicy->los = "3";
		pLocationPolicy->lbl = ONEM2M_NODEID;
		pc = (void *)pLocationPolicy;
		}
		break;
	case VERIFICATION_MGMTCMD_UPDATE:
		{
		resourceType = mgmtCmd;
		operation = UPDATE;
		sprintf(to, TO_MGMTCMD, ONEM2M_TO, mMgmtCmdResourceName);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_mgmtCmd, pMgmtCmd);
		pMgmtCmd->ni = ONEM2M_NODEID;
//		pMgmtCmd->dKey = mDeviceKey;
		pMgmtCmd->uKey = UKEY;
		pMgmtCmd->exe = "true";
//		pMgmtCmd->lbl = ONEM2M_NODEID;
		pMgmtCmd->ext = mNodeLink;
		pc = (void *)pMgmtCmd;
		}
		break;
	case VERIFICATION_CONTAINER_UPDATE:
		{
		resourceType = container;
		operation = UPDATE;
		sprintf(to, TO_CONTAINER, ONEM2M_TO, mRemoteCSEResourceName, mContainerResourceName);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_container, pContainer);
		pContainer->ni = ONEM2M_NODEID;
		pContainer->dKey = mDeviceKey;
		pContainer->lbl = "event";
		pc = (void *)pContainer;
		}
		break;
	case VERIFICATION_REMOTECSE_UPDATE:
		{
		resourceType = remoteCSE;
		operation = UPDATE;
		sprintf(to, TO_REMOTECSE, ONEM2M_TO, mRemoteCSEResourceName);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_remoteCSE, pRemoteCSE);
		pRemoteCSE->ni = ONEM2M_NODEID;
		pRemoteCSE->dKey = mDeviceKey;
		pc = (void *)pRemoteCSE;
		}
		break;
	case VERIFICATION_NODE_UPDATE:
		{
		resourceType = node;
		operation = UPDATE;
		sprintf(to, TO_NODE, ONEM2M_TO, ONEM2M_NODEID);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_node, pNode);
		pNode->ni = ONEM2M_NODEID;
		pNode->dKey = mDeviceKey;
		pNode->hcl = mHostCSELink;
		pc = (void *)pNode;
		}
		break;

	// DELETE
	case VERIFICATION_AREANWKINFO_DELETE:
		{
		resourceType = mgmtObj;
		operation = DELETE;
		sprintf(to, TO_AREANWKINFO, ONEM2M_TO, ONEM2M_NODEID, mAreaNwkInfoResourceName);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_areaNwkInfo, pAreaNwkInfo);
		pAreaNwkInfo->ni = ONEM2M_NODEID;
		pAreaNwkInfo->dKey = mDeviceKey;
		pc = (void *)pAreaNwkInfo;
		}
		break;
	case VERIFICATION_AE_DELETE:
		{
		resourceType = AE;
		operation = DELETE;
		sprintf(to, TO_AE, ONEM2M_TO, mRemoteCSEResourceName, mAEResourceName);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_AE, pAE);
		pAE->ni = ONEM2M_NODEID;
		pAE->dKey = mDeviceKey;
		pc = (void *)pAE;
		}
		break;
	case VERIFICATION_LOCATIONPOLICY_DELETE:
		{
		resourceType = locationPolicy;
		operation = DELETE;
		sprintf(to, TO_LOCATIONPOLICY, ONEM2M_TO, mLocationPolicyResourceName);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_locationPolicy, pLocationPolicy);
		pLocationPolicy->ni = ONEM2M_NODEID;
		pLocationPolicy->dKey = mDeviceKey;
		pc = (void *)pLocationPolicy;
		}
		break;
	case VERIFICATION_CONTENTINSTANCE_DELETE:
		{
		resourceType = contentInstance;
		operation = DELETE;
		sprintf(to, TO_CONTENTINSTANCE, ONEM2M_TO, mRemoteCSEResourceName, mContainerResourceName, mContentInstanceResourceName);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_contentInstance, pContentInstance);
		pContentInstance->ni = ONEM2M_NODEID;
		pContentInstance->dKey = mDeviceKey;
		pc = (void *)pContentInstance;
		}
		break;
	case VERIFICATION_MGMTCMD_DELETE:
		{
		resourceType = mgmtCmd;
		operation = DELETE;
		sprintf(to, TO_MGMTCMD, ONEM2M_TO, mMgmtCmdResourceName);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_mgmtCmd, pMgmtCmd);
		pMgmtCmd->ni = ONEM2M_NODEID;
		pMgmtCmd->dKey = mDeviceKey;
		pc = (void *)pMgmtCmd;
		}
		break;
	case VERIFICATION_CONTAINER_DELETE:
		{
		resourceType = container;
		operation = DELETE;
		sprintf(to, TO_CONTAINER, ONEM2M_TO, mRemoteCSEResourceName, mContainerResourceName);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_container, pContainer);
		pContainer->ni = ONEM2M_NODEID;
		pContainer->dKey = mDeviceKey;
		pc = (void *)pContainer;
		}
		break;
	case VERIFICATION_REMOTECSE_DELETE:
		{
		resourceType = remoteCSE;
		operation = DELETE;
		sprintf(to, TO_REMOTECSE, ONEM2M_TO, mRemoteCSEResourceName);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_remoteCSE, pRemoteCSE);
		pRemoteCSE->ni = ONEM2M_NODEID;
		pRemoteCSE->dKey = mDeviceKey;
		pc = (void *)pRemoteCSE;
		}
		break;
	case VERIFICATION_NODE_DELETE:
		{
		resourceType = node;
		operation = DELETE;
		sprintf(to, TO_NODE, ONEM2M_TO, ONEM2M_NODEID);

		ONEM2M_DEF_N_MEM_ALLOC(oneM2M_node, pNode);
		pNode->ni = ONEM2M_NODEID;
		pNode->dKey = mDeviceKey;
		pc = (void *)pNode;
		}
		break;
	default:
		mStep = VERIFICATION_END;
		break;
#endif
	}

	if(mStep < VERIFICATION_END) {
		if(pc != NULL) {
#ifdef ONEM2M_V1_12
			tp_oneM2M_V1_12_Request(resourceType, operation, fr, to, ri, pc);
#else
			tp_oneM2M_V1_Request(resourceType, operation, to, ri, pc);
#endif
			ONEM2M_MEM_FREE(pc);
		}
	}
}

void SDK_Verification_SimpleXmlParser(char* payload, char* name, char* value, int isPC) {
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
}

void cbMQTTConnected(int result) {
	SKTDebugPrint(LOG_LEVEL_INFO, "cbMQTTConnected() result : %d", result);
}

void cbMQTTSubscribed(int result) {
	SKTDebugPrint(LOG_LEVEL_INFO, "cbMQTTSubscribed() result : %d", result);
    DoVerificationStep();
}

void cbMQTTDisconnected(int result) {
	SKTDebugPrint(LOG_LEVEL_INFO, "cbMQTTDisconnected() result : %d", result);
}

void cbMQTTConnectionLost(char* cause) {
	SKTDebugPrint(LOG_LEVEL_INFO, "cbMQTTConnectionLost() result : %s", cause);
}

void cbMQTTMessageDelivered(int token) {
	SKTDebugPrint(LOG_LEVEL_INFO, "cbMQTTMessageDelivered() token : %d, step : %d", token, mStep);
}

void cbMQTTMessageArrived(char* topic, char* msg, int msgLen) {
	SKTDebugPrint(LOG_LEVEL_INFO, "cbMQTTMessageArrived() topic : %s, step : %d", topic, mStep);

	char* payload = (char*)calloc(msgLen + 1, sizeof(char));
	memcpy(payload, msg, msgLen);
	SKTDebugPrint(LOG_LEVEL_INFO, "payload : %s", payload);

	char rsc[DEFAULT_BUF_SIZE];
	memset(rsc, 0, DEFAULT_BUF_SIZE);
	SDK_Verification_SimpleXmlParser(payload, ATTR_RSC, rsc, 0);
	char rsm[DEFAULT_BUF_SIZE];
	memset(rsm, 0, DEFAULT_BUF_SIZE);
	SDK_Verification_SimpleXmlParser(payload, ATTR_RSM, rsm, 0);

#ifdef ONEM2M_V1_12
	switch(mStep) {
		case VERIFICATION_CSEBASE_RETRIEVE:
			mStep = VERIFICATION_NODE_CREATE;
			break;
		case VERIFICATION_NODE_CREATE:
			SDK_Verification_SimpleXmlParser(payload, ATTR_RI, mNodeLink, 1);
			if(strlen(mNodeLink) == 0) {
				mStep = VERIFICATION_END;
			} else {
				mStep = VERIFICATION_AE_CREATE;
			}
			break;
	   case VERIFICATION_AE_CREATE:
		   SDK_Verification_SimpleXmlParser(payload, ATTR_AEI, mAEID, 1);
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
			mStep = VERIFICATION_NODE_UPDATE;
			break;
		case VERIFICATION_NODE_UPDATE:
			mStep = VERIFICATION_AE_UPDATE;
			break;
		case VERIFICATION_REMOTECSE_UPDATE:
			// TODO
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
			break;
		case VERIFICATION_AE_DELETE:
			mStep = VERIFICATION_NODE_DELETE;
			break;
		case VERIFICATION_CONTAINER_DELETE:
			mStep = VERIFICATION_ACCESSCONTROLPOLICY_DELETE;
			break;
		case VERIFICATION_MGMTCMD_DELETE:
			mStep = VERIFICATION_CONTAINER_DELETE;
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
#else
	switch(mStep) {
		case VERIFICATION_NODE_CREATE:
			{
			SDK_Verification_SimpleXmlParser(payload, ATTR_RI, mNodeLink, 1);
			SDK_Verification_SimpleXmlParser(payload, ATTR_HCL, mHostCSELink, 1);
			if(strlen(mNodeLink) == 0) {
				mStep = VERIFICATION_END;
			}
			}
			break;
		case VERIFICATION_REMOTECSE_CREATE:
			{
			SKTDebugPrint(LOG_LEVEL_INFO, "mDeviceKey = 0x%x", mDeviceKey);
			SDK_Verification_SimpleXmlParser(payload, ATTR_DKEY, mDeviceKey, 0);
			SDK_Verification_SimpleXmlParser(payload, ATTR_RN, mRemoteCSEResourceName, 0);
			if(strlen(mDeviceKey) == 0 || strlen(mRemoteCSEResourceName) == 0) {
				mStep = VERIFICATION_END;
			}
			SKTDebugPrint(LOG_LEVEL_INFO, "mDeviceKey = %s", mDeviceKey);
			}
			break;
		case VERIFICATION_CONTAINER_CREATE:
			{
			SDK_Verification_SimpleXmlParser(payload, ATTR_RN, mContainerResourceName, 1);
			if(strlen(mContainerResourceName) == 0) {
				mStep = VERIFICATION_END;
			}
			}
			break;
		case VERIFICATION_MGMTCMD_CREATE:
			{
			SDK_Verification_SimpleXmlParser(payload, ATTR_RN, mMgmtCmdResourceName, 1);
			if(strlen(mMgmtCmdResourceName) == 0) {
				mStep = VERIFICATION_END;
			}
			}
			break;
		case VERIFICATION_CONTENTINSTANCE_CREATE:
			{
			SDK_Verification_SimpleXmlParser(payload, ATTR_RN, mContentInstanceResourceName, 1);
			if(strlen(mContentInstanceResourceName) == 0) {
				mStep = VERIFICATION_END;
			}
			}
			break;
		case VERIFICATION_LOCATIONPOLICY_CREATE:
			{
			SDK_Verification_SimpleXmlParser(payload, ATTR_RN, mLocationPolicyResourceName, 1);
			if(strlen(mLocationPolicyResourceName) == 0) {
				mStep = VERIFICATION_END;
			}
			}
			break;
		case VERIFICATION_AE_CREATE:
			{
			SDK_Verification_SimpleXmlParser(payload, ATTR_RN, mAEResourceName, 1);
			if(strlen(mAEResourceName) == 0) {
				mStep = VERIFICATION_END;
			}
			}
			break;
		case VERIFICATION_AREANWKINFO_CREATE:
			{
			SDK_Verification_SimpleXmlParser(payload, ATTR_RN, mAreaNwkInfoResourceName, 1);
			if(strlen(mAreaNwkInfoResourceName) == 0) {
				mStep = VERIFICATION_END;
			}
			}
			break;
		default:
			break;
	}

	mStep++;
#endif

	SKTDebugPrint(LOG_LEVEL_INFO, "=============================================================================");
	free(payload);
	DoVerificationStep();
}

int SDKVerificationMain()
{
	SKTDebugInit(True, LOG_LEVEL_INFO, stdout);

	SKTDebugPrint(LOG_LEVEL_INFO, "");
	SKTDebugPrint(LOG_LEVEL_INFO, "==========================================================================");
	SKTDebugPrint(LOG_LEVEL_INFO, "*** SDKVerification example ***");
	SKTDebugPrint(LOG_LEVEL_INFO, "==========================================================================");

	int rc = 0;

	// set callbacks
	rc = tpMQTTSetCallbacks(cbMQTTConnected, cbMQTTSubscribed, cbMQTTDisconnected, cbMQTTConnectionLost, cbMQTTMessageDelivered, cbMQTTMessageArrived);
	if(rc != 0) {
		SKTDebugPrint(LOG_LEVEL_DEBUG, "#### tpMQTTSetCallbacks failed. rc = %d", rc);
		goto exit;
	}
	SKTDebugPrint(LOG_LEVEL_DEBUG, "tpMQTTSetCallbacks result : %d", rc);

	// create
	char subscribeTopic[TOPIC_SUBSCRIBE_SIZE][MQTT_TOPIC_MAX_LENGTH];
	memset(subscribeTopic, 0, sizeof(subscribeTopic));
	char publishTopic[MQTT_TOPIC_MAX_LENGTH] = "";	
	snprintf(mClientID, sizeof(mClientID), "%s_%s", ACCOUNT_USER, ONEM2M_CLIENTID);
#ifdef ONEM2M_V1_12
    sprintf(subscribeTopic[0], TOPIC_SUBSCRIBE_REQ, ONEM2M_SERVICENAME, ONEM2M_AE_RESOURCENAME);
    sprintf(subscribeTopic[1], TOPIC_SUBSCRIBE_RES, ONEM2M_AE_RESOURCENAME, ONEM2M_SERVICENAME);
    sprintf(publishTopic, TOPIC_PUBLISH, ONEM2M_AE_RESOURCENAME, ONEM2M_SERVICENAME);
#else
	sprintf(subscribeTopic[0], TOPIC_SUBSCRIBE_REQ, mClientID);
	sprintf(subscribeTopic[1], TOPIC_SUBSCRIBE_RES, mClientID);
	sprintf(publishTopic, TOPIC_PUBLISH, mClientID, ONEM2M_CSEBASE);
#endif
	char* st[] = {subscribeTopic[0], subscribeTopic[1]};

	int port = (!MQTT_ENABLE_SERVER_CERT_AUTH ? MQTT_PORT : MQTT_SECURE_PORT);
	rc = tpMQTTCreate(MQTT_HOST, port, MQTT_KEEP_ALIVE, ACCOUNT_USER, ACCOUNT_PASSWORD, MQTT_ENABLE_SERVER_CERT_AUTH, st, TOPIC_SUBSCRIBE_SIZE, publishTopic, mClientID);
	SKTDebugPrint(LOG_LEVEL_DEBUG, "tpMQTTCreate result : %d", rc);
	if(rc == 0) {
		while(mStep < VERIFICATION_END) {
			if(tpMQTTYield(1000) != 0)	// wait for seconds
			{
				osDelay(500);
			}
		}
		tpMQTTDisconnect();
	}

exit:
	tpMQTTDestory();
	return 0;
}
