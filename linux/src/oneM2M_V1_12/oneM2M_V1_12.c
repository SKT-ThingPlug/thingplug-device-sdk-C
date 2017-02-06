/**
 * @file oneM2M_V1_12.c
 *
 * @brief oneM2M ver.1.12 file
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MQTT.h"
#include "oneM2M_V1_12.h"
#ifdef SPT_DEBUG_ENABLE
#include "SKTDebug.h"
#else
#include "SKTtpDebug.h"
#endif


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
 * @param[in] op : operation
 */
static void SetPCElement(char* payload, oneM2M_Attribute* attrs, int resourceType, int size, int op) {

    if(op == DELETE) return;
    
    char attr[512];
    char pc[1024];
    memset(attr, 0, sizeof(attr));
    memset(pc, 0, sizeof(pc));
    char* resource = RESOURCE_STR_V1_12(resourceType);
    char* resourceName = attrs[size-1].value;

    if(resourceName) {
        SetElement(attr, attrs, size-1);
        snprintf(pc, sizeof(pc), "<%s><m2m:%s rn=\"%s\">%s</m2m:%s></%s>", 
                    ATTR_PC, resource, resourceName, attr, resource, ATTR_PC);
    } else {
        SetElement(attr, attrs, size);
        snprintf(pc, sizeof(pc), "<%s><m2m:%s>%s</m2m:%s></%s>", 
                    ATTR_PC, resource, attr, resource, ATTR_PC);
    }
    strncat(payload, pc, strlen(pc));
}

/**
 * @brief make xml header with ty, op, ri, to
 * @param[in] payload : payload buffer
 * @param[in] resourceType : oneM2M resource type value
 * @param[in] operation : operation
 * @param[in] fr : from
 * @param[in] to : target
 * @param[in] rqi : request id
 */
static void SetHeader(char* payload, int resourceType, int operation, char* fr, char* to, char* rqi) {
    strncat(payload, XML_HEADER_V1_12, strlen(XML_HEADER_V1_12));

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
        oneM2M_Attribute attr[] = {{ATTR_OP, op}, {ATTR_FR, fr}, {ATTR_TO, to}, {ATTR_TY, ty}, {ATTR_RQI, rqi}};
        SetElement(payload, attr, 5);
    } else {
        oneM2M_Attribute attr[] = {{ATTR_OP, op}, {ATTR_FR, fr}, {ATTR_TO, to}, {ATTR_RQI, rqi}};
        SetElement(payload, attr, 4);
    }
}

/**
 * @brief make xml footer
 */
static void SetFooter(char* payload) {
    strncat(payload, XML_FOOTER_V1_12, strlen(XML_FOOTER_V1_12));
}

/**
 * @brief check attributes
 * @param[in] resourceType : oneM2M resource type value
 * @param[in] operation : operation
 * @param[in] to : target
 * @param[in] fr : from 
 * @param[in] rqi : request id
 * @param[in] pc : other attributes
 * @return int : result code
 */
static int CheckAttributes(int resourceType, int operation, char* fr, char* to, char* rqi, void* pc) {
    int rc = TP_SDK_SUCCESS;
#ifdef SPT_DEBUG_ENABLE
	SKTtpDebugLog(LOG_LEVEL_INFO, "ty : %d, op : %d, fr : %s, to : %s, rqi : %s", resourceType, operation, fr, to, rqi);
#else
    SKTDebugPrint(LOG_LEVEL_INFO, "ty : %d, op : %d, fr : %s, to : %s, rqi : %s", resourceType, operation, fr, to, rqi);
#endif
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
		case accessControlPolicy:
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

    // check to & ri & pc & fr is NULL?
    if(rc == TP_SDK_SUCCESS) {
        if(to == NULL || rqi == NULL || /*pc == NULL ||*/ fr == NULL) {
            rc = TP_SDK_INVALID_PARAMETER;
        }
    }
    return rc;
}

/**
 * @brief oneM2M V1.12 request
 * @param[in] resourceType : oneM2M resource type value
 * @param[in] operation : operation
 * @param[in] fr : from
 * @param[in] to : target
 * @param[in] rqi : request id
 * @param[in] pc : other attributes
 * @return int : result code
 */ 
int tp_oneM2M_V1_12_Request(int resourceType, int operation, char* fr, char* to, char* rqi, void* pc) {
    int rc = CheckAttributes(resourceType, operation, fr, to, rqi, pc);
    if(rc != TP_SDK_SUCCESS) {
        return rc;
    }
    
    char payload[1024];
    memset(payload, 0, sizeof(payload));
    
    SetHeader(payload, resourceType, operation, fr, to, rqi);

	if(pc) {
	    switch(resourceType) {
	        case CSEBase:
	            {
	            }
	            break;
	        case node:
	            {
	            oneM2M_v1_12_node* node = (oneM2M_v1_12_node *)pc;
	            oneM2M_Attribute pc[] = {{ATTR_ET, node->et}, {ATTR_ACPI, node->acpi}, {ATTR_LBL, node->lbl}, 
	                {ATTR_NI, node->ni}, {ATTR_MGA, node->mga}, {ATTR_RN, node->rn}}; //, {ATTR_HCL, node->hcl}};
	            SetPCElement(payload, pc, resourceType, 6, operation);
	            }
	            break;
	        case remoteCSE:
	            {
	            oneM2M_v1_12_remoteCSE* remoteCSE = (oneM2M_v1_12_remoteCSE *)pc;
	            oneM2M_Attribute pc[] = {{ATTR_ET, remoteCSE->et}, {ATTR_ACPI, remoteCSE->acpi}, {ATTR_LBL, remoteCSE->lbl}, 
	                {ATTR_CST, remoteCSE->cst},  {ATTR_POA, remoteCSE->poa}, {ATTR_CB, remoteCSE->cb}, {ATTR_CSI, remoteCSE->csi},
	                {ATTR_NL, remoteCSE->nl}, {ATTR_PPT, remoteCSE->ppt}, {ATTR_RR, remoteCSE->rr}, {ATTR_RN, remoteCSE->rn}};
	            SetPCElement(payload, pc, resourceType, 11, operation);
	            }
	            break;            
	        case container:
	            {
	            oneM2M_v1_12_container* container = (oneM2M_v1_12_container *)pc;
	            oneM2M_Attribute pc[] = {{ATTR_ET, container->et}, {ATTR_ACPI, container->acpi}, {ATTR_LBL, container->lbl},
	                {ATTR_MNI, container->mni}, {ATTR_MBS, container->mbs}, {ATTR_MIA, container->mia}, {ATTR_LI, container->li},
	                {ATTR_PPT, container->ppt}, {ATTR_RN, container->rn}};
	            SetPCElement(payload, pc, resourceType, 9, operation);
	            }
	            break;
	        case mgmtCmd:
	            {
	            oneM2M_v1_12_mgmtCmd* mgmtCmd = (oneM2M_v1_12_mgmtCmd *)pc;
	            oneM2M_Attribute pc[] = {{ATTR_ET, mgmtCmd->et}, {ATTR_ACPI, mgmtCmd->acpi}, {ATTR_LBL, mgmtCmd->lbl},
	                {ATTR_DC, mgmtCmd->dc}, {ATTR_CMT, mgmtCmd->cmt}, {ATTR_EXRA, mgmtCmd->exra}, {ATTR_EXT, mgmtCmd->ext},
	                {ATTR_EXM, mgmtCmd->exm}, {ATTR_EXF, mgmtCmd->exf}, {ATTR_EXY, mgmtCmd->exy}, {ATTR_EXN, mgmtCmd->exn},
	                {ATTR_RN, mgmtCmd->rn}};
	            SetPCElement(payload, pc, resourceType, 12, operation);
	            }
	            break;
	        case contentInstance:
	            {
	            oneM2M_v1_12_contentInstance* contentInstance = (oneM2M_v1_12_contentInstance *)pc;
	            oneM2M_Attribute pc[] = {{ATTR_LBL, contentInstance->lbl}, {ATTR_CNF, contentInstance->cnf}, {ATTR_CON, contentInstance->con},
	                {ATTR_RN, contentInstance->rn}};
	            SetPCElement(payload, pc, resourceType, 4, operation);
	            }
	            break;
	        case locationPolicy:
	            {
	            oneM2M_v1_12_locationPolicy* locationPolicy = (oneM2M_v1_12_locationPolicy *)pc;
	            oneM2M_Attribute pc[] = {{ATTR_ET, locationPolicy->et}, {ATTR_ACPI, locationPolicy->acpi}, {ATTR_LBL, locationPolicy->lbl},
	                {ATTR_LOS, locationPolicy->los}, {ATTR_LOU, locationPolicy->lou}, {ATTR_LOT, locationPolicy->lot}, {ATTR_LOR, locationPolicy->lor},
	                {ATTR_LON, locationPolicy->lon}, {ATTR_LIT, locationPolicy->lit}, {ATTR_RN, locationPolicy->rn}};
	            SetPCElement(payload, pc, resourceType, 10, operation);
	            }
	            break;
	        case AE:
	            {
	            oneM2M_v1_12_AE* AE = (oneM2M_v1_12_AE *)pc;
	            oneM2M_Attribute pc[] = {{ATTR_ET, AE->et}, {ATTR_ACPI, AE->acpi}, {ATTR_LBL, AE->lbl}, 
	                {ATTR_APN, AE->apn}, {ATTR_API, AE->api}, {ATTR_POA, AE->poa}, {ATTR_PPT, AE->ppt},
	                {ATTR_RR, AE->rr}, {ATTR_RN, AE->rn}};
	            SetPCElement(payload, pc, resourceType, 9, operation);
	            }
	            break;
	        case execInstance:
	            {
	            oneM2M_v1_12_execInstance* execInstance = (oneM2M_v1_12_execInstance *)pc;
	            oneM2M_Attribute pc[] = {{ATTR_ACPI, execInstance->acpi}, {ATTR_LBL, execInstance->lbl}, {ATTR_EXD, execInstance->exd},
	                {ATTR_EXR, execInstance->exr}, {ATTR_EXS, execInstance->exs}, {ATTR_EXT, execInstance->ext}, {ATTR_RN, execInstance->rn}};
	            SetPCElement(payload, pc, resourceType, 7, operation);
	            }
	            break;
	        case accessControlPolicy:
	            {
	            oneM2M_v1_12_accessControlPolicy* accessControlPolicy = (oneM2M_v1_12_accessControlPolicy *)pc;
	            oneM2M_Attribute pc[] = {{ATTR_ET, accessControlPolicy->et}, {ATTR_LBL, accessControlPolicy->lbl}, {ATTR_PV, accessControlPolicy->pv},
	                {ATTR_PVS, accessControlPolicy->pvs}, {ATTR_RN, accessControlPolicy->rn}};
	            SetPCElement(payload, pc, resourceType, 5, operation);
	            }
	            break;            
	        default:;
	            break;
	    }
	}
    SetFooter(payload);
#ifdef SPT_DEBUG_ENABLE
	SKTtpDebugLog(LOG_LEVEL_INFO, "request : %s", payload);
#else
    SKTDebugPrint(LOG_LEVEL_INFO, "request : %s", payload);
#endif
    rc = MQTTAsyncPublishMessage(payload);
    return rc;

}

/**
 * @brief register node, AE
 * @param[in] resourceType : resource type 
 * @param[in] fr : from
 * @param[in] to : to 
 * @param[in] ri : request id
 * @param[in] rn : resource name
 * @param[in] ni : node ID
 * @param[in] mga : node mga
 * @param[in] poa : AE poa
 * @param[in] api : AE App-ID
 * @return int : result code
 */
int tp_v1_12_RegisterDevice(int resourceType, char* fr, char* to, char* rqi, char* rn, char* ni, char* mga, char* poa, char* api) {
	int rc = 0;
    switch(resourceType) {
        case node: ;
			oneM2M_v1_12_node nodeInfo;
			memset(&nodeInfo, 0, sizeof(nodeInfo));			
			nodeInfo.rn = rn;
			nodeInfo.ni = ni;
			nodeInfo.mga = mga;
			rc = tp_oneM2M_V1_12_Request(resourceType, CREATE, fr, to, rqi, &nodeInfo);
			break;
		case AE: ;
			oneM2M_v1_12_AE AEInfo;
			memset(&AEInfo, 0, sizeof(AEInfo));
			AEInfo.rn = rn;
			AEInfo.poa = poa;
			AEInfo.api = api;
			AEInfo.rr = "true";
			rc = tp_oneM2M_V1_12_Request(resourceType, CREATE, fr, to, rqi, &AEInfo);
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
 * @param[in] rn : container name
 * @return int : result code
 */
int tp_v1_12_RegisterContainer(char* fr, char* to, char* rqi, char* rn) {
	oneM2M_v1_12_container containerInfo;
	memset(&containerInfo, 0, sizeof(containerInfo));
	containerInfo.rn = rn;
	int rc = tp_oneM2M_V1_12_Request(container, CREATE, fr, to, rqi, (void *)&containerInfo);
	return rc;
}

/**
 * @brief register mgmtCmd
 * @param[in] fr : from
 * @param[in] to : to
 * @param[in] ri : request id
 * @param[in] rn : mgmtCmd name
 * @param[in] cmt : cmdType
 * @param[in] ext : execTarget
 * @return int : result code
 */
int tp_v1_12_RegisterMgmtCmd(char* fr, char* to, char* rqi, char* rn, char* cmt, char* ext) {
	oneM2M_v1_12_mgmtCmd mgmtCmdInfo;
	memset(&mgmtCmdInfo, 0, sizeof(mgmtCmdInfo));
	mgmtCmdInfo.rn = rn;
	mgmtCmdInfo.cmt = cmt;
	mgmtCmdInfo.ext = ext;
	int rc = tp_oneM2M_V1_12_Request(mgmtCmd, CREATE, fr, to, rqi, (void *)&mgmtCmdInfo);
	return rc;
}

/**
 * @brief add content data of contentInstance
 * @param[in] data : data
 * @param[in] length : data length
 * @return int : result code
 */
int tp_v1_12_AddData(char* data, unsigned char length) {
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
#ifdef SPT_DEBUG_ENABLE
	SKTtpDebugLog(LOG_LEVEL_INFO, "Content data : %s, length : %d", gContent->data, gContent->len);
#else
	SKTDebugPrint(LOG_LEVEL_INFO, "Content data : %s, length : %d", gContent->data, gContent->len);
#endif
	return TP_SDK_SUCCESS;
}

/**
 * @brief report
 * @param[in] fr : from
 * @param[in] to : to
 * @param[in] ri : request id
 * @param[in] cnf : contentInfo
 * @param[in] con : content
 * @param[in] useAddedData : use added data flag
 * @return int : result code
 */
int tp_v1_12_Report(char* fr, char* to, char* rqi, char* cnf, char* con, unsigned char useAddedData) {
	oneM2M_v1_12_contentInstance contentInstanceInfo;
	memset(&contentInstanceInfo, 0, sizeof(contentInstanceInfo));
	contentInstanceInfo.cnf = cnf;
	if(useAddedData) {
		if(!gContent || !gContent->data) return TP_SDK_INVALID_PARAMETER;
		contentInstanceInfo.con = gContent->data;
	} else {
		if(!con) return TP_SDK_INVALID_PARAMETER;
		contentInstanceInfo.con = con;
	}
	int rc = tp_oneM2M_V1_12_Request(contentInstance, CREATE, fr, to, rqi, (void *)&contentInstanceInfo);
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
 * @param[in] exr : execResult
 * @param[in] exs : execStatus
 * @return int : result code
 */
int tp_v1_12_Result(char* fr, char* to, char* rqi, char* exr, char* exs) {
	oneM2M_v1_12_execInstance execInstanceInfo;
	memset(&execInstanceInfo, 0, sizeof(execInstanceInfo));
	execInstanceInfo.exr = exr;
	execInstanceInfo.exs = exs;
	int rc = tp_oneM2M_V1_12_Request(execInstance, UPDATE, fr, to, rqi, (void *)&execInstanceInfo);
	return rc;
}

