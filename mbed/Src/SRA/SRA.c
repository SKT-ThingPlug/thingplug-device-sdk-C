
/**
 * @file SRA.c
 *
 * @brief ServiceReadyAgent Process
 *
 * Copyright (C) 2016. SKT, All Rights Reserved.
 * Written 2016,by SKT
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "SRA.h"
#include "SMA.h"
#include "TTVManager.h"
#include "TTVBuilder.h"

void SRAGetTTV(char **out_buf, char type, char datatype, char *value )
{
    char *ndata;
    int nlen;
    nlen = TTVConvertData(type, datatype, value, &ndata);
    void *ttv, *node;	
    ttv = CreateTTV();
    node = CreateTTVNode();
    SetTTVNode(type, nlen, datatype, ndata, node);
    PushTTVNode( node, ttv);
    free(ndata);
    MakeTTVInstance(ttv);
    *out_buf = (char *)calloc(1, GetTTVInstanceSize(ttv)+1);
    memcpy((*out_buf), GetTTVInstance(ttv), GetTTVInstanceSize(ttv));
    DestroyTTV(ttv);
}