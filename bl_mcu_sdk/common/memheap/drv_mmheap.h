/**
 * @file drv_mmheap.h
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#ifndef __DRV_MMHEAP_H
#define __DRV_MMHEAP_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bflb_platform.h"

#ifndef MMHEAP_LOCK
#define MMHEAP_LOCK() __disable_irq()
#endif

#ifndef MMHEAP_UNLOCK
#define MMHEAP_UNLOCK() __enable_irq()
#endif

#ifndef MMHEAP_ASSERT
#define MMHEAP_ASSERT(A) \
    if (!(A))            \
    bflb_platform_printf("mmheap malloc error:%s,%d\r\n", __FILE__, __LINE__)

#endif

#ifndef MMHEAP_MALLOC_FAIL
#define MMHEAP_MALLOC_FAIL() bflb_platform_printf("mmheap malloc fail:%s,%d\r\n", __FILE__, __LINE__)
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct heap_region {
    void *addr;      //内存区起始地址
    size_t mem_size; //内存大小
};

struct heap_node {
    struct heap_node *next_node;
    size_t mem_size;
};

struct heap_info {
    struct heap_node *pStart;
    struct heap_node *pEnd;
    size_t total_size; //总内存
};

struct heap_state {
    size_t remain_size;   //内存剩余量
    size_t free_node_num; //空闲节点个数
    size_t max_node_size; //最大节点内存
    size_t min_node_size; //最小节点内存
};

void mmheap_init(struct heap_info *pRoot, const struct heap_region *pRigon);
void *mmheap_align_alloc(struct heap_info *pRoot, size_t align_size, size_t want_size);
void *mmheap_alloc(struct heap_info *pRoot, size_t want_size);
void *mmheap_realloc(struct heap_info *pRoot, void *src_addr, size_t want_size);
void mmheap_free(struct heap_info *pRoot, void *addr);
void mmheap_get_state(struct heap_info *pRoot, struct heap_state *pState);
#ifdef __cplusplus
}
#endif

#endif