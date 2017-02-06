/**
 * @file oneM2MCode.h
 *
 * @brief The fields define for oneM2M
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */
#ifndef _oneM2M_BINDING_H_
#define _oneM2M_BINDING_H_

/**
 * Return code: No error. Indicates successful completion
 */
#define TP_SDK_SUCCESS              0
/**
 * Return code: A generic error code indicating the failure
 */
#define TP_SDK_FAILURE              -1
/* error code -2 is MQTTAsync_PERSISTENCE_ERROR */
#define TP_SDK_MQTT_PERSISTENCE_ERROR -2
/**
 * Return code: The client is disconnected.
 */
#define TP_SDK_MQTT_DISCONNECTED -3
/**
 * Return code: The maximum number of messages allowed to be simultaneously 
 * in-flight has been reached.
 */
#define TP_SDK_MQTT_MAX_MESSAGES_INFLIGHT -4
/**
 * Return code: An invalid UTF-8 string has been detected.
 */
#define TP_SDK_MQTT_BAD_UTF8_STRING -5
/**
 * Return code: A NULL parameter has been supplied when this is invalid.
 */
#define TP_SDK_MQTT_NULL_PARAMETER -6
/**
 * Return code: The topic has been truncated (the topic string includes
 * embedded NULL characters). String functions will not access the full topic.
 * Use the topic length value to access the full topic.
 */
#define TP_SDK_MQTT_TOPICNAME_TRUNCATED -7
/**
 * Return code: A structure parameter does not have the correct eyecatcher
 * and version number.
 */
#define TP_SDK_MQTT_BAD_STRUCTURE -8
/**
 * Return code: A qos parameter is not 0, 1 or 2
 */
#define TP_SDK_MQTT_BAD_QOS -9
/**
 * Return code: All 65535 MQTT msgids are being used
 */
#define TP_SDK_MQTT_NO_MORE_MSGIDS -10
/**
 * Return code: the request is being discarded when not complete
 */
#define TP_SDK_MQTT_OPERATION_INCOMPLETE -11
/**
 * Return code: no more messages can be buffered
 */
#define TP_SDK_MQTT_MAX_BUFFERED_MESSAGES -12
/**
 * Return code: Resource type or Operation is not supported
 */
#define TP_SDK_NOT_SUPPORTED        -13
/**
 * Return code: Parameter is invalid
 */
#define TP_SDK_INVALID_PARAMETER    -14


//Table 8.2.4-1 : Resource and specialization type short names
#define ATTR_ACP "acp"					// resource accessControlPolicy
#define RSC_ACCESSCONTROLPOLICYANNC "acpA"
#define ATTR_AE "ae"                    // resource AE
#define RSC_AEANNC "aeA"
#define ATTR_CNT "cnt"                  // resource container
#define RSC_CONTAINERANNC "cntA"
#define ATTR_CIN "cin"                  // resource contentInstance
#define RSC_CONTENTINSTANCEANNC "cinA"
#define RSC_CSEBASE "csb"
#define RSC_DELIVERY "dlv"
#define RSC_EVENTCONFIG "evcg"
#define ATTR_EXIN "exin"                // resource execInstance
#define RSC_FANOUTPOINT "fopt"
#define RSC_GROUP "grp"
#define RSC_GROUPANNC "grpA"
#define ATTR_LCP "lcp"                  // resource locationPolicy
#define RSC_LOCATIONPOLICYANNC "lcpA"
#define RSC_M2MSERVICESUBSCRIPTIONPROFILE "mssp"
#define ATTR_MGC "mgc"                  // resource mgmtCmd 
#define RSC_MGMTOBJ "mgo"
#define RSC_MGMTOBJANNC "mgoA"
#define ATTR_NOD "nod"                   // resource node
#define RSC_NODEANNC "nodA"
#define RSC_POLLINGCHANNEL "pch"
#define RSC_POLLINGCHANNELURI "pcu"
#define ATTR_CSR "csr"                   // resource remoteCSE
#define RSC_REMOTECSEANNC "csrA"
#define RSC_REQUEST "req"
#define RSC_SCHEDULE "sch"
#define RSC_SCHEDULEANNC "schA"
#define RSC_SERVICESUBSCRIBEDAPPRULE "asar"
#define RSC_SERVICESUBSCRIBEDNODE "svsn"
#define RSC_STATSCOLLECT "stcl"
#define RSC_STATSCONFIG "stcg"
#define RSC_SUBSCRIPTION "sub"
#define RSC_FIRMWARE "fwr"
#define RSC_SOFTWARE "swr"
#define RSC_MEMORY "mem"
#define ATTR_ANI "ani"                  // resource areaNwkInfo
#define RSC_AREANWKDEVICEINFO "andi"
#define RSC_BATTERY "bat"
#define RSC_DEVICEINFO "dvi"
#define RSC_DEVICECAPABILITY "dvc"
#define RSC_REBOOT "rbt"
#define RSC_EVENTLOG "evl"
#define RSC_CMDHPOLICY "cmp"
#define RSC_ACTIVECMDHPOLICY "acmp"
#define RSC_CMDHDEFAULTS "cmdf"
#define RSC_CMDHDEFECVALUE "cmdv"
#define RSC_CMDHECDEFPARAMVALUES "cmpv"
#define RSC_CMDHLIMITS "cml"
#define RSC_CMDHNETWORKACCESSRULES "cmnr"
#define RSC_CMDHNWACCESSRULE "cmwr"
#define RSC_CMDHBUFFER "cmbf"


//Table 8.2.2-1 : Primitive parameter short names
#define ATTR_OP "op"                    // operation
#define ATTR_TO "to"                    // to
#define ATTR_FR "fr"                    // from
#define ATTR_RQI "rqi"                  // Request Identifier
#define ATTR_TY "ty"                    // Resource type
#define ATTR_NM "nm"                    // name 
#define ATTR_PC "pc"                    // Primitive Content
#define PARAM_ORIGINATING_TIMESTAMP "ot"
#define PARAM_REQUEST_EXPIRATION_TIMESTAMP "rqet"
#define PARAM_RESULT_EXPIRATION_TIMESTAMP "rset"
#define PARAM_OPERATION_EXECUTION TIME "oet"
#define PARAM_RESPONSE_TYPE "rt"
#define PARAM_RESULT_PERSISTENCE "rp"
#define PARAM_RESULT_CONTENT "rcn"
#define PARAM_EVENT_CATEGORY "ec"
#define PARAM_DELIVERY_AGGREGATION "da"
#define PARAM_GROUP_REQUEST_IDENTIFIER "gid"
#define PARAM_FILTER_CRITERIA "fc"
#define PARAM_CREATEDBEFORE "crb"
#define PARAM_CREATEDAFTER "cra"
#define PARAM_MODIFIEDSINCE "ms"
#define PARAM_UNMODIFIEDSINCE "us"
#define PARAM_STATETAGSMALLER "sts"
#define PARAM_STATETAGBIGGER "stb"
#define PARAM_EXPIREBEFORE "exb"
#define PARAM_EXPIREAFTER "exa"
#define ATTR_LBL "lbl"                   // Label
#define PARAM_RESOURCETYPE "rty"
#define PARAM_SIZEABOVE "sza"
#define PARAM_SIZEBELOW "szb"
#define PARAM_CONTENTTYPE "cty"
#define PARAM_LIMIT "lim"
#define PARAM_ATTRIBUTE "atr"
#define PARAM_FILTERUSAGE "fu"
#define PARAM_DISCOVERY_RESULT_TYPE "drt"
#define ATTR_RSC "rsc"                  // responseStatusCode


//Table 8.2.3-1 : Resource attribute short names
#define ATTR_ACPI "acpi"                // accessControlPolicyIDs
#define ATTR_ANNOUNCEDATTRIBUTE "aa"
#define ATTR_ANNOUNCETO "at"
#define ATTR_CT "ct"                    // Creation time
#define ATTR_ET "et"                    // Expiration time
#define ATTR_LT "lt"                    // Last modified time
#define ATTR_PI "pi"                    // Parent ID
#define ATTR_RI "ri"                    // Resource ID
#define ATTR_ST "st"                    // State tag
#define ATTR_RN "rn"                    // Resource name
#define ATTR_PV "pv"                    // privileges
#define ATTR_PVS "pvs"                  // selfPrivileges
#define ATTR_API "api"                  // App-ID
#define ATTR_AEI "aei"                  // AE-ID
#define ATTR_APN "apn"                  // appName
#define ATTR_POA "poa"                   // Point of access
#define ATTR_ONTOLOGYREF "or"
#define ATTR_NL "nl"                     // Node link
#define ATTR_CR "cr"                    // Creator
#define ATTR_MNI "mni"                  // maxNrOfInstances
#define ATTR_MBS "mbs"                  // maxByteSize
#define ATTR_MIA "mia"                  // maxInstanceAge
#define ATTR_CNI "cni"                  // Current number of instances
#define ATTR_CBS "cbs"                  // Current byte size
#define ATTR_LATEST "la"
#define ATTR_LI "li"                    // locationID
#define ATTR_CNF "cnf"                  // Content info
#define ATTR_CS "cs"                    // Content Size
#define ATTR_CON "con"                  // Content
#define ATTR_CST "cst"                  // CSE Type
#define ATTR_CSI "csi"                  // CSE ID
#define ATTR_SUPPORTEDRESOURCETYPE "srt"
#define ATTR_NOTIFICATIONCONGESTIONPOLICY "ncp"
#define ATTR_SOURCE "sr"
#define ATTR_TARGET "tg"
#define ATTR_LIFESPAN "ls"
#define ATTR_EVENTCAT "ec"
#define ATTR_DELIVERYMETADATA "dmd"
#define ATTR_AGGREGATEDREQUEST "arq"
#define ATTR_EVENT "ev"
#define ATTR_EVENTID "evi"
#define ATTR_EVENTTYPE "evt"
#define ATTR_EVENSTART "evs"
#define ATTR_EVENTEND "eve"
#define ATTR_OPERATIONTYPE "opt"
#define ATTR_DATASIZE "ds"
#define ATTR_EXS "exs"					// execStatus
#define ATTR_EXR "exr"                  // execResult
#define ATTR_EXD "exd"                  // execDisable
#define ATTR_EXT "ext"                  // execTarget
#define ATTR_EXM "exm"                  // execMode
#define ATTR_EXF "exf"                  // execFrequency
#define ATTR_EXY "exy"                  // execDelay
#define ATTR_EXN "exn"                  // execNumber
#define ATTR_EXRA "exra"                // execReqArgs
#define ATTR_EXE "exe"                  // execEnable
#define ATTR_MEMBERTYPE "mt"
#define ATTR_CURRENTNROFMEMBERS "cnm"
#define ATTR_MAXNROFMEMBERS "mnm"
#define ATTR_MEMBERID "mid"
#define ATTR_MEMBERSACCESSCONTROLPOLICYIDS "macp"
#define ATTR_MEMBERTYPEVALIDATED "mtv"
#define ATTR_CONSISTENCYSTRATEGY "csy"
#define ATTR_GROUPNAME "gn"
#define ATTR_LOS "los"                  // locationSource
#define ATTR_LOU "lou"                  // locationUpdatePeriod
#define ATTR_LOT "lot"                  // locationTargetID
#define ATTR_LOR "lor"                  // locationServer
#define ATTR_LOCATIONCONTAINERID "loi"
#define ATTR_LON "lon"                  // locationContainerName
#define ATTR_LOCATIONSTATUS "lost"
#define ATTR_SERVICEROLES "svr"
#define ATTR_DC "dc"                    // description
#define ATTR_CMT "cmt"                  // cmdType
#define ATTR_MGD "mgd"                  // mgmtDefinition
#define ATTR_OBJECTIDS "obis"
#define ATTR_OBJECTPATHS "obps"
#define ATTR_NI "ni"                    // Node ID
#define ATTR_HCL "hcl"                  // hostCSELink
#define ATTR_CB "cb"                    // CSEBase
#define ATTR_M2M_EXT_ID "mei"
#define ATTR_TRIGGER_RECIPIENT_ID "tri"
#define ATTR_RR "rr"                   // Request rechability
#define ATTR_ORIGINATOR "og"
#define ATTR_METAINFORMATION "mi"
#define ATTR_REQUESTSTATUS "rs"
#define ATTR_OPERATIONRESULT "ol"
#define ATTR_OPERATION "opn"
#define ATTR_REQUESTID "rid"
#define ATTR_SCHEDULEELEMENT "se"
#define ATTR_DEVICEIDENTIFIER "di"
#define ATTR_STATSCOLLECTID "sci"
#define ATTR_COLLECTINGENTITYID "cei"
#define ATTR_COLLECTEDENTITYID "cdi"
#define ATTR_GWST "gwst"                    // Gateway status
#define ATTR_SSST "ssst"                    // Sensor status
#define ATTR_STATUS "ss"
#define ATTR_STATSRULESTATUS "srs"
#define ATTR_STATMODEL "sm"
#define ATTR_COLLECTPERIOD "cp"
#define ATTR_EVENTNOTIFICATIONCRITERIA "enc"
#define ATTR_EXPIRATIONCOUNTER "exc"
#define ATTR_NOTIFICATIONURI "nu"
#define ATTR_NOTIFICATIONFORWARDINGURI "nfu"
#define ATTR_BATCHNOTIFY "bn"
#define ATTR_RATELIMIT "rl"
#define ATTR_PRESUBSCRIPTIONNOTIFY "psn"
#define ATTR_PENDINGNOTIFICATION "pn"
#define ATTR_NOTIFICATIONSTORAGEPRIORITY "nsp"
#define ATTR_LATESTNOTIFY "ln"
#define ATTR_NOTIFICATIONCONTENTTYPE "nct"
#define ATTR_NOTIFICATIONEVENTCAT "nec"
#define ATTR_SUBSCRIBERURI "su"
#define ATTR_VERSION "vr"
#define ATTR_URL "url"
#define ATTR_UPDATE "ud"
#define ATTR_UPDATESTATUS "uds"
#define ATTR_INSTALL "in"
#define ATTR_UNINSTALL "un"
#define ATTR_INSTALLSTATUS "ins"
#define ATTR_ACTIVATE "act"
#define ATTR_DEACTIVATE "dea"
#define ATTR_ACTIVATESTATUS "acts"
#define ATTR_MEMAVAILABLE "mma"
#define ATTR_MEMTOTAL "mmt"
#define ATTR_ANT "ant"                  // areaNwkType
#define ATTR_LDV "ldv"                  // listOfDevices
#define ATTR_DEVID "dvd"
#define ATTR_DEVTYPE "dvt"
#define ATTR_AREANWKID "awi"
#define ATTR_SLEEPINTERVAL "sli"
#define ATTR_SLEEPDURATION "sld"
#define ATTR_LISTOFNEIGHBORS "lnh"
#define ATTR_BATTERYLEVEL "btl"
#define ATTR_BATTERYSTATUS "bts"
#define ATTR_DEVICELABEL "dlb"
#define ATTR_MANUFACTURER "man"
#define ATTR_MODEL "mod"
#define ATTR_DEVICETYPE "dty"
#define ATTR_FWVERSION "fwv"
#define ATTR_SWVERSION "swv"
#define ATTR_HWVERSION "hwv"
#define ATTR_CAPABILITYNAME "can"
#define ATTR_ATTACHED "att"
#define ATTR_CAPABILITYACTIONSTATUS "cas"
#define ATTR_ENABLE "ena"
#define ATTR_DISABLE "dis"
#define ATTR_CURRENTSTATE "cus"
#define ATTR_REBOOT "rbo"
#define ATTR_FACTORYRESET "far"
#define ATTR_LOGTYPEID "lgt"
#define ATTR_LOGDATA "lgd"
#define ATTR_LOGACTIONSTATUS "lgs"
#define ATTR_LOGSTART "lga"
#define ATTR_LOGSTOP "lgo"
#define ATTR_NAME "nam"
#define ATTR_MGMTLINK "cmlk"
#define ATTR_ORDER "od"
#define ATTR_DEFECVALUE "dev"
#define ATTR_REQUESTORIGIN "ror"
#define ATTR_REQUESTCONTEXT "rct"
#define ATTR_REQUESTCONTEXTNOTIFICATION "rcn"
#define ATTR_REQUESTCHARACTERISTICS "rch"
#define ATTR_APPLICABLEEVENTCATEGORIES "aecs"
#define ATTR_APPLICABLEEVENTCATEGORY "aec"
#define ATTR_DEFAULTREQUESTEXPTIME "dqet"
#define ATTR_DEFAULTRESULTEXPTIME "dset"
#define ATTR_DEFAULTOPEXECTIME "doet"
#define ATTR_DEFAULTRESPPERSISTENCE "drp"
#define ATTR_DEFAULTDELAGGREGATION "dda"
#define ATTR_LIMITSEVENTCATEGORY "lec"
#define ATTR_LIMITSREQUESTEXPTIME "lqet"
#define ATTR_LIMITSRESULTEXPTIME "lset"
#define ATTR_LIMITSOPEXECTIME "loet"
#define ATTR_LIMITSRESPPERSISTENCE "lrp"
#define ATTR_LIMITSDELAGGREGATION "lda"
#define ATTR_TARGETENTWORK "ttn"
#define ATTR_MINREQVOLUME "mrv"
#define ATTR_BACKOFFPARAMETERS "bop"
#define ATTR_OTHERCONDITIONS "ohc"
#define ATTR_MAXBUFFERSIZE "mbfs"
#define ATTR_STORAGEPRIORITY "sgp"
#define ATTR_APPLICABLECREDIDS "aci"
#define ATTR_ALLOWEDAPP_IDS "aai"
#define ATTR_ALLOWEDAES "aae"
#define ATTR_RSM        "RSM"       // responseStatusMessage


enum RESOURCE_TYPE {
    accessControlPolicy = 1,
    AE,
    container,
    contentInstance,
    CSEBase,
    delivery,
    eventConfig,
    execInstance,
    group,
    locationPolicy,
    m2mServiceSubscriptionProfile,
    mgmtCmd,
    mgmtObj,
    node,
    pollingChannel,
    remoteCSE,
    request,
    schedule,
    serviceSubscribedNode,
    statsCollect,
    statsConfig,
    subscription,
    accessControlPolicyAnnc = 10001,
    AEAnnc,
    containerAnnc,
    contentInstanceAnnc,
    groupAnnc = 10009,
    locationPolicyAnnc,
    mgmtObjAnnc = 10013,
    nodeAnnc,
    remoteCSEAnnc = 10016,
    scheduleAnnc = 10018
};

enum  OPERATION {
    CREATE = 1,
    RETRIEVE,
    UPDATE,
    DELETE,
    NOTIFY
};


typedef struct {
	char *name;
	char *value;
} oneM2M_Attribute;

#endif
