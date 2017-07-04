/**
 * @file List.h
 *
 * @brief Header for DeviceMiddleware List API
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016,by SK Telecom
 */

#ifndef _DM_LIST_H_
#define _DM_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

//#include <pthread.h>
#include <stdbool.h>


typedef struct _list_node{
    void *data;
    struct _list_node *next;
    struct _list_node *prev;
}DMListNode;

typedef void (*Releaser)(void*);

typedef void (*Printer)(void*);

typedef bool (*CompareTo)(void*, void*);

// list structure
typedef struct _list_head {
    int count;
    Releaser free;
    DMListNode* head;
    DMListNode* tail;
} DMList;

void DMList_Init(DMList *list, Releaser relfn);
void DMList_Close(DMList *list);
void DMList_Add(DMList *list, void *data);
DMListNode* DMList_Remove(DMList *list);
DMListNode* DMList_GetNode(DMList *list, void *data, CompareTo compfn);
void DMList_Print(DMList *list, Printer printfn);

#ifdef __cplusplus
}
#endif

#endif//_DM_LIST_H_
