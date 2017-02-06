/**
 * @file oneM2M_V1.c
 *
 * @brief oneM2M ver.1 file
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/MQTT.h"
#include "include/oneM2M_V1.h"

/**
 * @brief make attribute string data
 * @param[in] payload : payload buffer
 * @param[in] attrs : oneM2M_Attribute list data pointer
 * @param[in] size : size of attrs list
 */
static void SetElement(char* payload, oneM2M_Attribute* attrs, int size) {
    int i = 0;
    char attr[512];
 
    for(i = 0; i < size; i++) {
        if(attrs[i].value != NULL /*&& strlen(attrs[i].value) > 0*/) {
            memset(attr, 0, sizeof(attr));
            snprintf(attr, sizeof(attr), "<%s>%s</%s>", attrs[i].name, attrs[i].value, attrs[i].name);
            strncat(payload, attr, strlen(attr));
        }
    }
}

/**
 * @brief make primitive_content attribute string data
 * @param[in] payload : payload buffer
 * @param[in] attrs : oneM2M_Attribute list data pointer
 * @param[in] resourceType : oneM2M resource type value
 * @param[in] size : size of attrs list
 */
static void SetPCElement(char* payload, oneM2M_Attribute* attrs, int resourceType, int size) {
    char attr[512];
    char pc[1024];
    memset(attr, 0, sizeof(attr));
    memset(pc, 0, sizeof(pc));
    SetElement(attr, attrs, size);
    char* resource = RESOURCE_STR(resourceType);
    snprintf(pc, sizeof(pc), "<%s><%s>%s</%s></%s>", 
                ATTR_PC, resource, attr, resource, ATTR_PC);
    strncat(payload, pc, strlen(pc));
}

/**
 * @brief make xml header with ty, op, ri, to
 * @param[in] payload : payload buffer
 * @param[in] resourceType : oneM2M resource type value
 * @param[in] operation : operation
 * @param[in] to : target
 * @param[in] ri : request id
 */
static void SetHeader(char* payload, int resourceType, int operation, char* to, char* ri) {
    strncat(payload, XML_HEADER_V1, strlen(XML_HEADER_V1));

    // operation
    char op[2];
    memset(op, 0, sizeof(op));
    snprintf(op, sizeof(op), "%d", operation);

    // op & to & ri
    if(operation == CREATE) {        
        // ty
        char ty[6];
        memset(ty, 0, sizeof(ty));
        snprintf(ty, sizeof(ty), "%d", resourceType);
        oneM2M_Attribute attr[] = {{ATTR_OP, op}, {ATTR_TO, to}, {ATTR_TY, ty}, {ATTR_RI, ri}};
        SetElement(payload, attr, 4);
    } else {
        oneM2M_Attribute attr[] = {{ATTR_OP, op}, {ATTR_TO, to}, {ATTR_RI, ri}};
        SetElement(payload, attr, 3);
    }
}

/**
 * @brief make xml footer
 */
static void SetFooter(char* payload) {
    strncat(payload, XML_FOOTER_V1, strlen(XML_FOOTER_V1));
}

/**
 * @brief check attributes
 * @param[in] resourceType : oneM2M resource type value
 * @param[in] operation : operation
 * @param[in] to : target
 * @param[in] ri : request id
 * @param[in] pc : other attributes
 * @return int : result code
 */
static int CheckAttributes(int resourceType, int operation, char* to, char* ri, void* pc) {
    int rc = TP_SDK_SUCCESS;
    // check resourceType & operation
    switch(resourceType) {
        case CSEBase:
            if(operation != RETRIEVE) {
                rc = TP_SDK_NOT_SUPPORTED;
            }
            break;
        case node:
        case remoteCSE:
        case container:
        case mgmtCmd:
        case locationPolicy:
        case AE:
        case mgmtObj:
            if(operation == RETRIEVE) {
                rc = TP_SDK_NOT_SUPPORTED;
            }
            break;
        case contentInstance:
            if(operation == RETRIEVE || operation == UPDATE) {
                rc = TP_SDK_NOT_SUPPORTED;
            }
            break;
        case execInstance:
            if(operation != UPDATE) {
                rc = TP_SDK_NOT_SUPPORTED;
            }
            break;
        default:;
            rc = TP_SDK_NOT_SUPPORTED;
            break;
    }

    // check to & ri & pc is NULL?
    if(rc == TP_SDK_SUCCESS) {
        if(to == NULL || ri == NULL || pc == NULL) {
            rc = TP_SDK_INVALID_PARAMETER;
        }
    }
    return rc;
}

/**
 * @brief oneM2M V1 request
 * @param[in] resourceType : oneM2M resource type value
 * @param[in] operation : operation
 * @param[in] to : target
 * @param[in] ri : request id
 * @param[in] pc : other attributes
 * @return int : result code
 */ 
int tp_oneM2M_V1_Request(int resourceType, int operation, char* to, char* ri, void* pc) {
    int rc = CheckAttributes(resourceType, operation, to, ri, pc);
    if(rc != TP_SDK_SUCCESS) {
        return rc;
    }
    char payload[1024];
    memset(payload, 0, sizeof(payload));
    
    SetHeader(payload, resourceType, operation, to, ri);

    switch(resourceType) {
        case CSEBase:
            {
            oneM2M_CSEBase* CSEBase = (oneM2M_CSEBase *)pc;
            oneM2M_Attribute attr = {ATTR_FR, CSEBase->ni};
            SetElement(payload, &attr, 1);
            }
            break;
        case node:
            {
            oneM2M_node* node = (oneM2M_node *)pc;
            oneM2M_Attribute attr[] = {{ATTR_FR, node->ni}, {ATTR_NM, node->ni}, {ATTR_DKEY, node->dKey}};
            SetElement(payload, attr, 3);
            if(operation != DELETE) {
                oneM2M_Attribute pc[] = {{ATTR_NI, node->ni}, {ATTR_HCL, node->hcl}, {ATTR_MGA, node->mga}};
                SetPCElement(payload, pc, resourceType, 3);
            }
            }
            break;
        case remoteCSE:
            {
            oneM2M_remoteCSE* remoteCSE = (oneM2M_remoteCSE *)pc;
            oneM2M_Attribute attr[] = {{ATTR_FR, remoteCSE->ni}, {ATTR_PASSCODE, remoteCSE->passCode}, {ATTR_DKEY, remoteCSE->dKey}, {ATTR_NM, remoteCSE->nm}};
            SetElement(payload, attr, 4);
            if(operation != DELETE) {
                oneM2M_Attribute pc[] = {{ATTR_CST, remoteCSE->cst}, {ATTR_CSI, remoteCSE->ni}, {ATTR_POA, remoteCSE->poa}, {ATTR_RR, remoteCSE->rr},  {ATTR_NL, remoteCSE->nl}};
                SetPCElement(payload, pc, resourceType, 5);
            }
            }
            break;            
        case container:
            {
            oneM2M_container* container = (oneM2M_container *)pc;
            oneM2M_Attribute attr[] = {{ATTR_FR, container->ni}, {ATTR_NM, container->nm}, {ATTR_DKEY, container->dKey}};
            SetElement(payload, attr, 3);
            if(operation != DELETE) {
                oneM2M_Attribute lbl = {ATTR_LBL, container->lbl};
                SetPCElement(payload, &lbl, resourceType, 1);
            }
            }
            break;
        case mgmtCmd:
            {
            oneM2M_mgmtCmd* mgmtCmd = (oneM2M_mgmtCmd *)pc;
            oneM2M_Attribute attr[] = {{ATTR_FR, mgmtCmd->ni}, {ATTR_NM, mgmtCmd->nm}, {ATTR_DKEY, mgmtCmd->dKey}, {ATTR_UKEY, mgmtCmd->uKey}};
            SetElement(payload, attr, 4);
            if(operation != DELETE) {
                oneM2M_Attribute pc[] = {{ATTR_CMT, mgmtCmd->cmt}, {ATTR_EXE, mgmtCmd->exe}, {ATTR_EXT, mgmtCmd->ext}, {ATTR_LBL, mgmtCmd->lbl}};
                SetPCElement(payload, pc, resourceType, 4);
            }
            }
            break;
        case contentInstance:
            {
            oneM2M_contentInstance* contentInstance = (oneM2M_contentInstance *)pc;
            oneM2M_Attribute attr[] = {{ATTR_FR, contentInstance->ni}, {ATTR_DKEY, contentInstance->dKey}};
            SetElement(payload, attr, 2);
            if(operation != DELETE) {
                oneM2M_Attribute pc[] = {{ATTR_CNF, contentInstance->cnf}, {ATTR_CON, contentInstance->con}, {ATTR_LBL, contentInstance->lbl}};
                SetPCElement(payload, pc, resourceType, 3);
            }
            }
            break;
        case locationPolicy:
            {
            oneM2M_locationPolicy* locationPolicy = (oneM2M_locationPolicy *)pc;
            oneM2M_Attribute attr[] = {{ATTR_FR, locationPolicy->ni}, {ATTR_DKEY, locationPolicy->dKey}, {ATTR_NM, locationPolicy->nm}};
            SetElement(payload, attr, 3);
            if(operation != DELETE) {
                oneM2M_Attribute pc[] = {{ATTR_LOS, locationPolicy->los}, {ATTR_LBL, locationPolicy->ni}};
                SetPCElement(payload, pc, resourceType, 2);
            }
            }
            break;
        case AE:
            {
            oneM2M_AE* AE = (oneM2M_AE *)pc;
            oneM2M_Attribute attr[] = {{ATTR_FR, AE->ni}, {ATTR_DKEY, AE->dKey}};
            SetElement(payload, attr, 2);
            if(operation != DELETE) {
                oneM2M_Attribute pc[] = {{ATTR_API, AE->api}, {ATTR_APN, AE->apn}};
                SetPCElement(payload, pc, resourceType, 2);
            }
            }
            break;
        case mgmtObj:
            {
            oneM2M_areaNwkInfo* areaNwkInfo = (oneM2M_areaNwkInfo *)pc;
            oneM2M_Attribute attr[] = {{ATTR_FR, areaNwkInfo->ni}, {ATTR_NM, areaNwkInfo->nm}, {ATTR_DKEY, areaNwkInfo->dKey}};
            SetElement(payload, attr, 3);
            if(operation != DELETE) {
                oneM2M_Attribute pc[] = {{ATTR_MGD, areaNwkInfo->mgd}, {ATTR_ANT, areaNwkInfo->ant}, {ATTR_LDV, areaNwkInfo->ldv}};
                SetPCElement(payload, pc, resourceType, 3);
            }
            }
            break;
        case execInstance:
            {
            oneM2M_mgmtCmdResult* mgmtCmdResult = (oneM2M_mgmtCmdResult *)pc;
            oneM2M_Attribute attr[] = {{ATTR_FR, mgmtCmdResult->ni}, {ATTR_DKEY, mgmtCmdResult->dKey}};
            SetElement(payload, attr, 2);
            oneM2M_Attribute pc[] = {{ATTR_EXR, mgmtCmdResult->exr}, {ATTR_EXS, mgmtCmdResult->exs}};
            SetPCElement(payload, pc, resourceType, 2);
            }
            break;
        default:;
            break;
    }

    SetFooter(payload);
    rc = MQTTPublishMessage(payload);
    return rc;

}


/**
 * @brief register node, remoteCSE
 * @param[in] resourceType : resource type 
 * @param[in] fr : from
 * @param[in] to : to
 * @param[in] ri : request id
 * @param[in] mga : node mga
 * @param[in] nm : remoteCSE name
 * @param[in] cst : remoteCSE type
 * @param[in] passcode : remoteCSE passcode
 * @param[in] poa : remoteCSE pointOfAccess
 * @param[in] nl : remoteCSE nodeLink
 * @return int : result code
 */
int tpRegisterDevice(int resourceType, char* fr, char* to, char* ri, char* mga, char* nm, char* cst, char* passcode, char* poa, char* nl) {
	int rc = 0;
    switch(resourceType) {
        case node: ;
			oneM2M_node nodeInfo;
			memset(&nodeInfo, 0, sizeof(nodeInfo));
			nodeInfo.ni = fr;
			nodeInfo.mga = mga;
			rc = tp_oneM2M_V1_Request(resourceType, CREATE, to, ri, (void *)&nodeInfo);
			break;
		case remoteCSE: ;
			oneM2M_remoteCSE remoteCSEInfo;
			memset(&remoteCSEInfo, 0, sizeof(remoteCSEInfo));
			remoteCSEInfo.ni = fr;
			remoteCSEInfo.nm = nm;
			remoteCSEInfo.cst = cst;
			remoteCSEInfo.passCode = passcode;
			remoteCSEInfo.poa = poa;
			remoteCSEInfo.rr = "true";
			remoteCSEInfo.nl = nl;
			rc = tp_oneM2M_V1_Request(resourceType, CREATE, to, ri, (void *)&remoteCSEInfo);
			break;
		default:
			return TP_SDK_NOT_SUPPORTED;
   	}
	return rc;
}

/**
 * @brief register container
 * @param[in] fr : from
 * @param[in] to : to
 * @param[in] ri : request id
 * @param[in] nm : container name
 * @param[in] dKey : dKey
 * @param[in] lbl : labels
 * @return int : result code
 */
int tpRegisterContainer(char* fr, char* to, char* ri, char* nm, char* dKey, char* lbl) {
	oneM2M_container containerInfo;
	memset(&containerInfo, 0, sizeof(containerInfo));
	containerInfo.ni = fr;
	containerInfo.nm = nm;
	containerInfo.dKey = dKey;
	containerInfo.lbl = lbl;
	int rc = tp_oneM2M_V1_Request(container, CREATE, to, ri, (void *)&containerInfo);
	return rc;
}

/**
 * @brief register mgmtCmd
 * @param[in] fr : from
 * @param[in] to : to
 * @param[in] ri : request id
 * @param[in] nm : mgmtCmd name
 * @param[in] dKey : dKey
 * @param[in] cmt : cmdType
 * @param[in] exe : execEnable
 * @param[in] ext : execTarget
 * @param[in] lbl : labels
 * @return int : result code
 */
int tpRegisterMgmtCmd(char* fr, char* to, char* ri, char* nm, char* dKey, char* cmt, char* exe, char* ext, char* lbl) {
	oneM2M_mgmtCmd mgmtCmdInfo;
	memset(&mgmtCmdInfo, 0, sizeof(mgmtCmdInfo));
	mgmtCmdInfo.ni = fr;
	mgmtCmdInfo.nm = nm;
	mgmtCmdInfo.dKey = dKey;
	mgmtCmdInfo.cmt = cmt;
	mgmtCmdInfo.exe = exe;
	mgmtCmdInfo.ext = ext;
	mgmtCmdInfo.lbl = lbl;
	int rc = tp_oneM2M_V1_Request(mgmtCmd, CREATE, to, ri, (void *)&mgmtCmdInfo);
	return rc;
}

/**
 * @brief add content data of contentInstance
 * @param[in] data : data
 * @param[in] length : data length
 * @return int : result code
 */
int tpAddData(char* data, unsigned char length) {
	if(!data || length < 1) return TP_SDK_FAILURE;
	if(!gContent) {
		gContent = (Content *)calloc(1, sizeof(Content));
		gContent->data = (char *)calloc(1, length + 1);
		memcpy(gContent->data, data, length);
		gContent->len = length;
	} else {
		gContent->len = gContent->len + length;
		gContent->data = (char *)realloc(gContent->data, gContent->len + 1);
		gContent->data[gContent->len] = '\0';
		strncat(gContent->data, data, length);
	}
	return TP_SDK_SUCCESS;
}

/**
 * @brief report
 * @param[in] fr : from
 * @param[in] to : to
 * @param[in] ri : request id
 * @param[in] dKey : dKey
 * @param[in] cnf : contentInfo
 * @param[in] con : content
 * @param[in] useAddedData : use added data flag
 * @return int : result code
 */
int tpReport(char* fr, char* to, char* ri, char* dKey, char* cnf, char* con, unsigned char useAddedData) {
	oneM2M_contentInstance contentInstanceInfo;
	memset(&contentInstanceInfo, 0, sizeof(contentInstanceInfo));
	contentInstanceInfo.ni = fr;
	contentInstanceInfo.dKey = dKey;
	contentInstanceInfo.cnf = cnf;
	if(useAddedData) {
		if(!gContent || !gContent->data) return TP_SDK_INVALID_PARAMETER;
		contentInstanceInfo.con = gContent->data;
	} else {
		if(!con) return TP_SDK_INVALID_PARAMETER;
		contentInstanceInfo.con = con;
	}
	int rc = tp_oneM2M_V1_Request(contentInstance, CREATE, to, ri, (void *)&contentInstanceInfo);
	if(useAddedData) {
		if(gContent) {
			if(gContent->data) {
				free(gContent->data);
				gContent->data = NULL;
			}
			free(gContent);
			gContent = NULL;
		}
	}
	return rc;
}

/**
 * @brief report
 * @param[in] fr : from
 * @param[in] to : to
 * @param[in] ri : request id
 * @param[in] dKey : dKey
 * @param[in] exr : execResult
 * @param[in] exs : execStatus
 * @return int : result code
 */
int tpResult(char* fr, char* to, char* ri, char* dKey, char* exr, char* exs) {
	  oneM2M_mgmtCmdResult mgmtCmdResultInfo;
	  memset(&mgmtCmdResultInfo, 0, sizeof(mgmtCmdResultInfo));
	  mgmtCmdResultInfo.ni = fr;
	  mgmtCmdResultInfo.dKey = dKey;
	  mgmtCmdResultInfo.exr = exr;
	  mgmtCmdResultInfo.exs = exs;
	  int rc = tp_oneM2M_V1_Request(execInstance, UPDATE, to, ri, (void *)&mgmtCmdResultInfo);
	  return rc;
}
