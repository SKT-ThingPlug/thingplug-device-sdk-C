
/**
 * @file TTVManager.c
 *
 * @brief Implementation Structures & Functions to handle TTV
 *
 * Copyright (C) 2017. SK Telecom, All Rights Reserved.
 * Written 2017,by SK Telecom
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>

#include "TTVManager.h"


/*
 ****************************************
 * TTV Implementations
 ****************************************
 */

#define __TTV_DEBUG__

void TTV_DEBUG(const char *format, ...)
{
#ifdef __TTV_DEBUG__
    FILE *gDebugStream   = stderr;
	va_list argp;
	va_start(argp, format);
	vfprintf(gDebugStream, format, argp);
	va_end(argp);
	fflush(gDebugStream);
#endif
}

void TTVRelease(void *node)
{
    TTV_NODE_T *tdv = (TTV_NODE_T *)node;
    if( tdv->mValue != NULL) {
        free(tdv->mValue);
    }
    free(tdv);
}

void* CreateTTV(void)
{
    TTV_CTX_T *context = (TTV_CTX_T *)calloc(1, sizeof(TTV_CTX_T));
    context->mCount = 0;
    context->mValueLength = 0;
    context->mInstance = NULL;
    context->mUpdate = 0x0;
    DMList_Init(&context->mTTVList, TTVRelease);
    return (void*)context;
}

void DestroyTTV(void *TTV)
{
    TTV_CTX_T *ctx = (TTV_CTX_T *) TTV;

    if(ctx == NULL) return;
    if(ctx->mInstance != NULL) {
        TTV_DEBUG("[TTV] DESTROY INSTANCE <%s>\n", ctx->mInstance);
        free(ctx->mInstance);
    }
    DMList_Close(&ctx->mTTVList);
    free(ctx);
}

void* CreateTTVNode(void)
{
    return calloc(1, sizeof(TTV_NODE_T));
}

void SetTTVNode(const char tag, const short len, const char datatype, const char *value, void *TTVNode)
{
    TTV_NODE_T *tdv = (TTV_NODE_T *)TTVNode;
    if( tdv->mValue != NULL ) free(tdv->mValue);

    tdv->mTag = tag;
    tdv->mDataType = datatype;
    tdv->mLength= len;
    tdv->mValue = calloc(1, len);
    memcpy(tdv->mValue, value, len);
}

char GetTTVTag(void *TTVNode)
{
    TTV_NODE_T *tdv = (TTV_NODE_T *) TTVNode;
    return tdv->mTag;
}

char GetTTVDataType(void *TTVNode)
{
    TTV_NODE_T *tdv = (TTV_NODE_T *) TTVNode;
    return tdv->mDataType;
}

short GetTTVLength(void *TTVNode)
{
    TTV_NODE_T *tdv = (TTV_NODE_T *) TTVNode;
    return tdv->mLength;
}

char* GetTTVValue(void *TTVNode)
{
    TTV_NODE_T *tdv = (TTV_NODE_T *) TTVNode;
    return tdv->mValue;
}

void DestroyTTVNode(void *TTVNode)
{
    TTV_NODE_T *tdv = (TTV_NODE_T *) TTVNode;
    if(tdv == NULL) return;
    TTV_DEBUG("[TTV] DESTROY TTV <TAG = 0x%x, DATA_TYPE= 0x%x>\n", tdv->mTag, tdv->mDataType);
    if( tdv->mValue != NULL ) free(tdv->mValue);
    free(tdv);
}


void PushTTVNode(void *TTVNode, void *TTV)
{
    TTV_CTX_T *ctx = (TTV_CTX_T *) TTV;
    TTV_NODE_T *tdv = (TTV_NODE_T *) TTVNode;
    DMList_Add(&ctx->mTTVList, TTVNode);
    ctx->mCount++;
    ctx->mValueLength += (unsigned short)tdv->mLength;
    ctx->mUpdate = 0x1;
    TTV_DEBUG("[TTV] PUSH <TAG = 0x%x, DATA_TYPE = 0x%x, LEN = 0x%x>\n", tdv->mTag, tdv->mDataType, (unsigned short)tdv->mLength);
}

void* PopTTVNode(void *TTV)
{
    TTV_CTX_T *ctx = (TTV_CTX_T *) TTV;
    TTV_NODE_T *tdv;
    DMListNode *node;

    if( ctx->mTTVList.count == 0) {
        TTV_DEBUG("[TTV] List is Empty!! \n");
        return NULL;
    }

    node = DMList_Remove(&ctx->mTTVList);
    tdv = (TTV_NODE_T *)node->data;
    ctx->mCount--;
    ctx->mValueLength -= (unsigned short)tdv->mLength;
    ctx->mUpdate = 0x1;

    return (void *)tdv;
}

void MakeTTVInstance(void *TTV)
{
    char *instanceHex;
    char *instanceBinary;
    int instanceLength;
    int i,pos = 0;
    TTV_CTX_T *ctx = (TTV_CTX_T *) TTV;
    TTV_NODE_T *tdv;
    DMListNode *node;

    if(ctx->mTTVList.count == 0) {
        TTV_DEBUG("[TTV] List is Empty!! \n");
        return;
    }

    if(ctx->mInstance != NULL) {
        free(ctx->mInstance);
    }

    instanceLength = ctx->mCount*2 + ctx->mValueLength;

    instanceBinary = (char *)calloc( 1, sizeof(char) * instanceLength);
    instanceHex = (char *)calloc( 1, 2 * sizeof(char) * instanceLength + 1);

    node = ctx->mTTVList.head;
    do{
        int i;
        tdv = (TTV_NODE_T *)node->data;
        TTV_DEBUG("[TTV] INSTANCE <<==  <TAG = 0x%x, DATA_TYPE = 0x%x , LEN = 0x%x\n"
                ,tdv->mTag, tdv->mDataType, (unsigned short)tdv->mLength);
        instanceBinary[pos++] = tdv->mTag;
        instanceBinary[pos++] = tdv->mDataType;
        for (i = 0; i < (unsigned short)tdv->mLength; i++) {
            instanceBinary[pos++] = tdv->mValue[i];
        }
        node = node->next;
    }while( node != ctx->mTTVList.head );

    for( i = 0; i < instanceLength; i++) {
        snprintf(&instanceHex[i*2],3,"%02x",(unsigned char)instanceBinary[i]);
    }

    free(instanceBinary);
    ctx->mInstance = instanceHex;
    ctx->mUpdate = 0x0;
}

char* GetTTVInstance(void *TTV)
{
    TTV_CTX_T *ctx = (TTV_CTX_T *) TTV;
    return ctx->mInstance;
}

int GetTTVInstanceSize(void *TTV)
{
    TTV_CTX_T *ctx = (TTV_CTX_T *) TTV;
    if( ctx->mUpdate != 0x0 ) {
        TTV_DEBUG("[TTV] You have to call function -> MakeInstance()\n");
        return 0;
    }
    return (int)2*(ctx->mCount*2 + ctx->mValueLength);
}

void PrintTTVNode(void *TTVNode)
{
    TTV_NODE_T *tdv = (TTV_NODE_T *) TTVNode;
    TTV_DEBUG("[TTV] PRINT <TAG = 0x%x, DATA_TYPE= 0x%x, VAL = %s>\n", tdv->mTag, tdv->mDataType, tdv->mValue);
}

void PrintTTV(void *TTV)
{
    TTV_CTX_T *ctx = (TTV_CTX_T *) TTV;
    TTV_DEBUG("[TTV] PRINT <ctx->mCount = %hu, ctx->mValueLength = %hu> \n", ctx->mCount, ctx->mValueLength);

    DMList_Print(&ctx->mTTVList, PrintTTV);
}


