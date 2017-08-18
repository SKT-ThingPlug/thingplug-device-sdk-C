/**
 * @file oneM2M_V1_14.c
 *
 * @brief oneM2M ver.1.14 file
 *
 * Copyright (C) 2017. SK Telecom, All Rights Reserved.
 * Written 2017, by SK Telecom
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oneM2M_V1_14.h""
#include "SKTtpDebug.h"
#include "MQTT_handler.h"



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
    char pc[512];
    memset(attr, 0, sizeof(attr));
    memset(pc, 0, sizeof(pc));
    char* resource = RESOURCE_STR_V1_14(resourceType);
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
    strncat(payload, XML_HEADER_V1_14, strlen(XML_HEADER_V1_14));

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
    strncat(payload, XML_FOOTER_V1_14, strlen(XML_FOOTER_V1_14));
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
static int CheckAttributes(int resourceType, int operation, char* fr, char* to, void* pc) {
    int rc = TP_SDK_SUCCESS;
    SKTDebugPrint(LOG_LEVEL_INFO, "ty : %d, op : %d, fr : %s, to : %s", resourceType, operation, fr, to);

    // check resourceType & operation
    switch(resourceType) {
        case CSEBase:
            if(operation != RETRIEVE) {
                rc = TP_SDK_NOT_SUPPORTED;
            }
            break;
        // case node:
        // case remoteCSE:
        case container:
        case mgmtCmd:
        case locationPolicy:
        case subscription:
        case AE:
        // case accessControlPolicy:
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
        if(to == NULL || /*rqi == NULL || pc == NULL ||*/ fr == NULL) {
            rc = TP_SDK_INVALID_PARAMETER;
        }
    }
    return rc;
}

/**
 * @brief Generate random id
 * @return unsigned int id
 */
unsigned int GenerateRequestId() {
    //oneM2M : 1 ~ 999999
    static unsigned int id = 0;
    unsigned int prefix = PREFIX_RQI;
    prefix = prefix%1000;

    /* Intializes random number generator */
    id++;
    id = id%1000;
    if(id == 0) id = 1;
    id = prefix*1000 + id;

    {
        char str[32];
        sprintf(str,"GenerateRequestId : %d\r\n", id);
        SKTDebugPrint(LOG_LEVEL_INFO, str);
    }
    return id;
}


/**
 * @brief oneM2M V1.14 request
 * @param[in] resourceType : oneM2M resource type value
 * @param[in] operation : operation
 * @param[in] fr : from
 * @param[in] to : target
 * @param[in] pc : other attributes
 * @return int : result code
 */
int tp_oneM2M_v1_14_Request(int resourceType, int operation, char* fr, char* to, void* pc) {
    int rc = CheckAttributes(resourceType, operation, fr, to, pc);
    if(rc != TP_SDK_SUCCESS) {
        return rc;
    }

    char *payload;
    payload = (char*)calloc(1,1024);
    memset(payload, 0, 1024);
    // request ID
    int requestID = GenerateRequestId();
    char rqi[7] = "";
    snprintf(rqi, sizeof(rqi), "%d", requestID);

    SetHeader(payload, resourceType, operation, fr, to, rqi);
    if(pc) {
        switch(resourceType) {
            case CSEBase:
                {
                }
                break;
            // case remoteCSE:
            //     {
            //     oneM2M_v1_14_remoteCSE* remoteCSE = (oneM2M_v1_14_remoteCSE *)pc;
            //     oneM2M_Attribute pc[] = {{ATTR_ET, remoteCSE->et}, {ATTR_ACPI, remoteCSE->acpi}, {ATTR_LBL, remoteCSE->lbl},
            //         {ATTR_CST, remoteCSE->cst},  {ATTR_POA, remoteCSE->poa}, {ATTR_CB, remoteCSE->cb}, {ATTR_CSI, remoteCSE->csi},
            //         {ATTR_NL, remoteCSE->nl}, {ATTR_PPT, remoteCSE->ppt}, {ATTR_RR, remoteCSE->rr}, {ATTR_RN, remoteCSE->rn}};
            //     SetPCElement(payload, pc, resourceType, 11, operation);
            //     }
            //     break;
            case container:
                {
                oneM2M_v1_14_container* containerObject = (oneM2M_v1_14_container *)pc;
                oneM2M_Attribute attrs[] = {{ATTR_ET, containerObject->et}, {ATTR_ACPI, containerObject->acpi}, {ATTR_LBL, containerObject->lbl},
                    {ATTR_MNI, containerObject->mni}, {ATTR_MBS, containerObject->mbs}, {ATTR_MIA, containerObject->mia}, {ATTR_LI, containerObject->li},
                    /*{ATTR_PPT, container->ppt},*/ {ATTR_RN, containerObject->rn}};
                SetPCElement(payload, attrs, resourceType, 8, operation);
                }
                break;
            case mgmtCmd:
                {
                oneM2M_v1_14_mgmtCmd* mgmtCmdObject = (oneM2M_v1_14_mgmtCmd *)pc;
                oneM2M_Attribute attrs[] = {{ATTR_ET, mgmtCmdObject->et}, {ATTR_ACPI, mgmtCmdObject->acpi}, {ATTR_LBL, mgmtCmdObject->lbl},
                    {ATTR_DC, mgmtCmdObject->dc}, {ATTR_CMT, mgmtCmdObject->cmt}, {ATTR_EXRA, mgmtCmdObject->exra}, {ATTR_EXT, mgmtCmdObject->ext},
                    {ATTR_EXM, mgmtCmdObject->exm}, {ATTR_EXF, mgmtCmdObject->exf}, {ATTR_EXY, mgmtCmdObject->exy}, {ATTR_EXN, mgmtCmdObject->exn},
                    {ATTR_RN, mgmtCmdObject->rn}};
                SetPCElement(payload, attrs, resourceType, 12, operation);
                }
                break;
            case contentInstance:
                {
                oneM2M_v1_14_contentInstance* contentInstanceObject = (oneM2M_v1_14_contentInstance *)pc;
                oneM2M_Attribute attrs[] = {{ATTR_LBL, contentInstanceObject->lbl}, {ATTR_CNF, contentInstanceObject->cnf},
                    {ATTR_CON, contentInstanceObject->con}, {ATTR_RN, contentInstanceObject->rn}};
                SetPCElement(payload, attrs, resourceType, 4, operation);
                }
                break;
            case locationPolicy:
                {
                oneM2M_v1_14_locationPolicy* locationPolicyObject = (oneM2M_v1_14_locationPolicy *)pc;
                oneM2M_Attribute attrs[] = {{ATTR_ET, locationPolicyObject->et}, {ATTR_ACPI, locationPolicyObject->acpi}, {ATTR_LBL, locationPolicyObject->lbl},
                    {ATTR_LOS, locationPolicyObject->los}, {ATTR_LOU, locationPolicyObject->lou}, {ATTR_LOT, locationPolicyObject->lot}, {ATTR_LOR, locationPolicyObject->lor},
                    {ATTR_LON, locationPolicyObject->lon}, {ATTR_LIT, locationPolicyObject->lit}, {ATTR_RN, locationPolicyObject->rn}};
                SetPCElement(payload, attrs, resourceType, 10, operation);
                }
                break;
            case AE:
                {
                oneM2M_v1_14_AE* AEObject = (oneM2M_v1_14_AE *)pc;
                oneM2M_Attribute attr[] = {{ATTR_ET, AEObject->et}, {ATTR_ACPI, AEObject->acpi}, {ATTR_LBL, AEObject->lbl},
                    {ATTR_APN, AEObject->apn}, {ATTR_API, AEObject->api}, {ATTR_POA, AEObject->poa}, {ATTR_RR, AEObject->rr},
                    {ATTR_NI, AEObject->ni}, {ATTR_MGA, AEObject->mga}, {ATTR_RN, AEObject->rn}};
                SetPCElement(payload, attr, resourceType, 10, operation);
                }
                break;
            case execInstance:
                {
                oneM2M_v1_14_execInstance* execInstanceObject = (oneM2M_v1_14_execInstance *)pc;
                oneM2M_Attribute attrs[] = {{ATTR_ACPI, execInstanceObject->acpi}, {ATTR_LBL, execInstanceObject->lbl}, {ATTR_EXD, execInstanceObject->exd},
                    {ATTR_EXR, execInstanceObject->exr}, {ATTR_EXS, execInstanceObject->exs}, {ATTR_EXT, execInstanceObject->ext}, {ATTR_RN, execInstanceObject->rn}};
                SetPCElement(payload, attrs, resourceType, 7, operation);
                }
                break;
            // case accessControlPolicy:
            //     {
            //     oneM2M_v1_14_accessControlPolicy* accessControlPolicy = (oneM2M_v1_14_accessControlPolicy *)pc;
            //     oneM2M_Attribute pc[] = {{ATTR_ET, accessControlPolicy->et}, {ATTR_LBL, accessControlPolicy->lbl}, {ATTR_PV, accessControlPolicy->pv},
            //         {ATTR_PVS, accessControlPolicy->pvs}, {ATTR_RN, accessControlPolicy->rn}};
            //     SetPCElement(payload, pc, resourceType, 5, operation);
            //     }
            //     break;
            case subscription:
                {
                oneM2M_v1_14_subscription* subscriptionObject = (oneM2M_v1_14_subscription *)pc;
                oneM2M_Attribute attrs[] = {{ATTR_ENC, subscriptionObject->enc}, {ATTR_NU, subscriptionObject->nu}, {ATTR_NCT, subscriptionObject->nct},
                    {ATTR_RN, subscriptionObject->rn}};
                SetPCElement(payload, attrs, resourceType, 4, operation);
                }
                break;
            default:;
                break;
        }
    }
    SetFooter(payload);
    SKTDebugPrint(LOG_LEVEL_INFO, "\r\n <payload> %s",payload);
    rc = handleMQTTPublishMessage(payload);

    free(payload);
    // publish success
    if(rc == 0) {
        rc = requestID;
    }
    return rc;
}

/**
 * @brief register AE
 * @param[in] fr : from
 * @param[in] to : to
 * @param[in] rqi : request id
 * @param[in] rn : resource name
 * @param[in] ni : node ID
 * @param[in] api : AE-ID
 * @param[in] serviceId : service id
 * @param[in] clientId : client id
 * @return int : result code
 */
int tp_v1_14_RegisterDevice(char* fr, char* to, char* rn, char* ni, char* api, char* serviceId, char* clientId) {
    oneM2M_v1_14_AE AEInfo;
    memset(&AEInfo, 0, sizeof(AEInfo));
    AEInfo.rn = rn;
    AEInfo.rr = "true";
    AEInfo.ni = ni;
    AEInfo.api = api;
    char mga[128] = "";
    snprintf(mga, sizeof(mga), "mqtt://%s", clientId);
    AEInfo.mga = mga;
    char poa[128] = "";
    snprintf(poa, sizeof(poa), "mqtt://oneM2M/req_msg/%s/%s", serviceId, clientId);
    AEInfo.poa = poa;
    int rc = tp_oneM2M_v1_14_Request(AE, CREATE, fr, to, &AEInfo);
    return rc;
}

/**
 * @brief register container
 * @param[in] fr : from
 * @param[in] to : to
 * @param[in] rn : container name
 * @return int : result code
 */
int tp_v1_14_RegisterContainer(char* fr, char* to, char* rn) {
    oneM2M_v1_14_container containerInfo;
    memset(&containerInfo, 0, sizeof(containerInfo));
    containerInfo.rn = rn;
    int rc = tp_oneM2M_v1_14_Request(container, CREATE, fr, to, (void *)&containerInfo);
    return rc;
}

/**
 * @brief register mgmtCmd
 * @param[in] fr : from
 * @param[in] to : to
 * @param[in] rn : mgmtCmd name
 * @param[in] cmt : cmdType
 * @param[in] ext : execTarget
 * @return int : result code
 */
int tp_v1_14_RegisterMgmtCmd(char* fr, char* to, char* rn, char* cmt, char* ext) {
    oneM2M_v1_14_mgmtCmd mgmtCmdInfo;
    memset(&mgmtCmdInfo, 0, sizeof(mgmtCmdInfo));
    mgmtCmdInfo.rn = rn;
    mgmtCmdInfo.cmt = cmt;
    mgmtCmdInfo.ext = ext;
    int rc = tp_oneM2M_v1_14_Request(mgmtCmd, CREATE, fr, to, (void *)&mgmtCmdInfo);
    return rc;
}

/**
 * @brief add content data of contentInstance
 * @param[in] data : data
 * @param[in] length : data length
 * @return int : result code
 */
int tp_v1_14_AddData(char* data, unsigned char length) {
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
 * @param[in] cnf : contentInfo
 * @param[in] con : content
 * @param[in] useAddedData : use added data flag
 * @return int : result code
 */
int tp_v1_14_Report(char* fr, char* to, char* cnf, char* con, unsigned char useAddedData) {
    oneM2M_v1_14_contentInstance contentInstanceInfo;
    memset(&contentInstanceInfo, 0, sizeof(contentInstanceInfo));
    contentInstanceInfo.cnf = cnf;
    if(useAddedData) {
        if(!gContent || !gContent->data) return TP_SDK_INVALID_PARAMETER;
        contentInstanceInfo.con = gContent->data;
    } else {
        if(!con) return TP_SDK_INVALID_PARAMETER;
        contentInstanceInfo.con = con;
    }
    int rc = tp_oneM2M_v1_14_Request(contentInstance, CREATE, fr, to, (void *)&contentInstanceInfo);
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
 * @param[in] exr : execResult
 * @param[in] exs : execStatus
 * @return int : result code
 */
int tp_v1_14_Result(char* fr, char* to, char* exr, char* exs) {
    oneM2M_v1_14_execInstance execInstanceInfo;
    memset(&execInstanceInfo, 0, sizeof(execInstanceInfo));
    execInstanceInfo.exr = exr;
    execInstanceInfo.exs = exs;
    int rc = tp_oneM2M_v1_14_Request(execInstance, UPDATE, fr, to, (void *)&execInstanceInfo);
    return rc;
}
