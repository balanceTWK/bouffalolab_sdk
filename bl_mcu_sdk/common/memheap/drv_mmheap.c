/**
 * @file drv_mmheap.c
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

#include "drv_mmheap.h"

#define MEM_MANAGE_ALIGNMENT_BYTE_DEFAULT 8
#define MEM_MANAGE_BITS_PER_BYTE          8
#define MEM_MANAGE_MEM_STRUCT_SIZE        mmheap_align_up(sizeof(struct heap_node), MEM_MANAGE_ALIGNMENT_BYTE_DEFAULT)
#define MEM_MANAGE_MINUM_MEM_SIZE         (MEM_MANAGE_MEM_STRUCT_SIZE << 1)
#define MEM_MANAGE_ALLOCA_LABAL           ((size_t)(1 << (sizeof(size_t) * MEM_MANAGE_BITS_PER_BYTE - 1)))

static inline size_t mmheap_align_down(size_t data, size_t align_byte)
{
    return data & ~(align_byte - 1);
}

static inline size_t mmheap_align_up(size_t data, size_t align_byte)
{
    return (data + align_byte - 1) & ~(align_byte - 1);
}

static inline struct heap_node *mmheap_addr_sub(const void *addr)
{
    return (struct heap_node *)((const uint8_t *)addr - MEM_MANAGE_MEM_STRUCT_SIZE);
}

static inline void *mmheap_addr_add(const struct heap_node *mem_node)
{
    return (void *)((const uint8_t *)mem_node + MEM_MANAGE_MEM_STRUCT_SIZE);
}

//将内存节点插入空闲列表中
static inline void mmheap_insert_node_to_freelist(struct heap_info *pRoot, struct heap_node *pNode)
{
    struct heap_node *pPriv_Node;
    struct heap_node *pNext_Node;
    //寻找地址与pNode相近的节点
    for (pPriv_Node = pRoot->pStart; pPriv_Node->next_node < pNode; pPriv_Node = pPriv_Node->next_node)
        ;
    pNext_Node = pPriv_Node->next_node;
    //尝试pNode与前一个块进行合并
    if ((uint8_t *)mmheap_addr_add(pPriv_Node) + pPriv_Node->mem_size == (uint8_t *)pNode) {
        if (pPriv_Node != pRoot->pStart) { //不是Start块的话可以合并
            pPriv_Node->mem_size += MEM_MANAGE_MEM_STRUCT_SIZE + pNode->mem_size;
            pNode = pPriv_Node;
        } else { //后面如果是Start块不进行合并，以免浪费内存
            pRoot->pStart->next_node = pNode;
        }
    } else { //不能合并时直接插入到空闲单链表中
        pPriv_Node->next_node = pNode;
    }
    //尝试后面一个块与pNode进行合并
    if ((uint8_t *)mmheap_addr_add(pNode) + pNode->mem_size == (uint8_t *)pNext_Node) {
        if (pNext_Node != pRoot->pEnd) { //不是end块的话可以进行块合并
            pNode->mem_size += MEM_MANAGE_MEM_STRUCT_SIZE + pNext_Node->mem_size;
            pNode->next_node = pNext_Node->next_node;
        } else { //后面如果是end块不进行合并，以免浪费内存
            pNode->next_node = pRoot->pEnd;
        }
    } else { //不能合并时直接插入到空闲单链表中
        pNode->next_node = pNext_Node;
    }
}

//获取管理内存的状态
void mmheap_get_state(struct heap_info *pRoot, struct heap_state *pState)
{
    MMHEAP_ASSERT(pRoot->pStart != NULL);
    MMHEAP_ASSERT(pRoot->pEnd != NULL);
    pState->max_node_size = pRoot->pStart->next_node->mem_size;
    pState->min_node_size = pRoot->pStart->next_node->mem_size;
    pState->remain_size = 0;
    pState->free_node_num = 0;
    MMHEAP_LOCK();
    for (struct heap_node *pNode = pRoot->pStart->next_node; pNode->next_node != NULL; pNode = pNode->next_node) {
        pState->remain_size += pNode->mem_size;
        pState->free_node_num++;
        if (pNode->mem_size > pState->max_node_size)
            pState->max_node_size = pNode->mem_size;
        if (pNode->mem_size < pState->min_node_size)
            pState->min_node_size = pNode->mem_size;
    }
    MMHEAP_UNLOCK();
}

//对齐分配内存
void *mmheap_align_alloc(struct heap_info *pRoot, size_t align_size, size_t want_size)
{
    void *pReturn = NULL;
    struct heap_node *pPriv_Node, *pNow_Node;

    MMHEAP_ASSERT(pRoot->pStart != NULL);
    MMHEAP_ASSERT(pRoot->pEnd != NULL);

    if (want_size == 0) {
        return NULL;
    }

    if ((want_size & MEM_MANAGE_ALLOCA_LABAL) != 0) { //内存过大
        MMHEAP_MALLOC_FAIL();
        return NULL;
    }

    if (align_size & (align_size - 1)) { //内存对齐输入非法值
        MMHEAP_MALLOC_FAIL();
        return NULL;
    }

    MMHEAP_LOCK();
    if (want_size < MEM_MANAGE_MINUM_MEM_SIZE)
        want_size = MEM_MANAGE_MINUM_MEM_SIZE;
    if (align_size < MEM_MANAGE_ALIGNMENT_BYTE_DEFAULT)
        align_size = MEM_MANAGE_ALIGNMENT_BYTE_DEFAULT;
    //确保分配的单元都是MEM_MANAGE_ALIGNMENT_BYTE_DEFAULT的整数倍
    want_size = mmheap_align_up(want_size, MEM_MANAGE_ALIGNMENT_BYTE_DEFAULT);

    pPriv_Node = pRoot->pStart;
    pNow_Node = pRoot->pStart->next_node;

    while (pNow_Node->next_node != NULL) {
        if (pNow_Node->mem_size >= want_size + MEM_MANAGE_MEM_STRUCT_SIZE) {
            size_t use_align_size;
            size_t new_size;
            pReturn = (void *)mmheap_align_up((size_t)mmheap_addr_add(pNow_Node), align_size); //计算出对齐的地址
            use_align_size = (uint8_t *)pReturn - (uint8_t *)mmheap_addr_add(pNow_Node);       //计算对齐所消耗的内存
            if (use_align_size != 0) {                                                         //内存不对齐
                if (use_align_size < MEM_MANAGE_MINUM_MEM_SIZE + MEM_MANAGE_MEM_STRUCT_SIZE) { //不对齐的值过小
                    pReturn = (void *)mmheap_align_up(
                        (size_t)mmheap_addr_add(pNow_Node) + MEM_MANAGE_MINUM_MEM_SIZE + MEM_MANAGE_MEM_STRUCT_SIZE, align_size);
                    use_align_size = (uint8_t *)pReturn - (uint8_t *)mmheap_addr_add(pNow_Node);
                }
                if (use_align_size <= pNow_Node->mem_size) {
                    new_size = pNow_Node->mem_size - use_align_size; //计算去除对齐消耗的内存剩下的内存大小
                    if (new_size >= want_size) {                     //满足条件，可以进行分配
                        struct heap_node *pNew_Node = mmheap_addr_sub(pReturn);
                        pNow_Node->mem_size -= new_size + MEM_MANAGE_MEM_STRUCT_SIZE; //分裂节点
                        pNew_Node->mem_size = new_size;                               //新节点本来也不在空闲链表中，不用从空闲链表中排出
                        pNew_Node->next_node = NULL;
                        pNow_Node = pNew_Node;
                        break;
                    }
                }
            } else {                                          //内存直接就是对齐的
                pPriv_Node->next_node = pNow_Node->next_node; //排出空闲链表
                pNow_Node->next_node = NULL;
                break;
            }
        }
        pPriv_Node = pNow_Node;
        pNow_Node = pNow_Node->next_node;
    }

    if (pNow_Node == pRoot->pEnd) { //分配失败
        MMHEAP_UNLOCK();
        MMHEAP_MALLOC_FAIL();
        return NULL;
    }

    if (pNow_Node->mem_size >= MEM_MANAGE_MINUM_MEM_SIZE + MEM_MANAGE_MEM_STRUCT_SIZE + want_size) {           //节点内存还有富余
        struct heap_node *pNew_Node = (struct heap_node *)((uint8_t *)mmheap_addr_add(pNow_Node) + want_size); //计算将要移入空闲链表的节点地址
        pNew_Node->mem_size = pNow_Node->mem_size - want_size - MEM_MANAGE_MEM_STRUCT_SIZE;
        pNew_Node->next_node = NULL;
        pNow_Node->mem_size = want_size;
        mmheap_insert_node_to_freelist(pRoot, pNew_Node);
    }
    pNow_Node->mem_size |= MEM_MANAGE_ALLOCA_LABAL; //标记内存已分配
    MMHEAP_UNLOCK();
    return pReturn;
}

void *mmheap_alloc(struct heap_info *pRoot, size_t want_size)
{
    return mmheap_align_alloc(pRoot, MEM_MANAGE_ALIGNMENT_BYTE_DEFAULT, want_size);
}

void *mmheap_realloc(struct heap_info *pRoot, void *src_addr, size_t want_size)
{
    void *pReturn = NULL;
    struct heap_node *pNext_Node, *pPriv_Node;
    struct heap_node *pSrc_Node;
    MMHEAP_ASSERT(pRoot->pStart != NULL);
    MMHEAP_ASSERT(pRoot->pEnd != NULL);
    if (src_addr == NULL) {
        return mmheap_align_alloc(pRoot, MEM_MANAGE_ALIGNMENT_BYTE_DEFAULT, want_size);
    }
    if (want_size == 0) {
        mmheap_free(pRoot, src_addr);
        return NULL;
    }

    MMHEAP_LOCK();
    if ((want_size & MEM_MANAGE_ALLOCA_LABAL) != 0) {
        MMHEAP_UNLOCK();
        MMHEAP_MALLOC_FAIL();
        return NULL;
    }

    pSrc_Node = mmheap_addr_sub(src_addr);

    if ((pSrc_Node->mem_size & MEM_MANAGE_ALLOCA_LABAL) == 0) { //源地址未被分配，调用错误
        MMHEAP_UNLOCK();
        MMHEAP_ASSERT((pSrc_Node->mem_size & MEM_MANAGE_ALLOCA_LABAL) != 0);
        MMHEAP_MALLOC_FAIL();
        return NULL;
    }

    pSrc_Node->mem_size &= ~MEM_MANAGE_ALLOCA_LABAL;    //清除分配标记
    if (pSrc_Node->mem_size >= want_size) {             //块预留地址足够大
        pSrc_Node->mem_size |= MEM_MANAGE_ALLOCA_LABAL; //恢复分配标记
        pReturn = src_addr;
        MMHEAP_UNLOCK();
        return pReturn;
    }
    //开始在空闲列表中寻找与本块相近的块
    for (pPriv_Node = pRoot->pStart; pPriv_Node->next_node < pSrc_Node; pPriv_Node = pPriv_Node->next_node)
        ;
    pNext_Node = pPriv_Node->next_node;

    if (pNext_Node != pRoot->pEnd &&
        ((uint8_t *)src_addr + pSrc_Node->mem_size == (uint8_t *)pNext_Node) &&
        (pSrc_Node->mem_size + pNext_Node->mem_size + MEM_MANAGE_MEM_STRUCT_SIZE >= want_size)) {
        //满足下一节点非end，内存连续，内存剩余足够
        pReturn = src_addr;
        pPriv_Node->next_node = pNext_Node->next_node; //排出空闲列表
        pSrc_Node->mem_size += MEM_MANAGE_MEM_STRUCT_SIZE + pNext_Node->mem_size;
        want_size = mmheap_align_up(want_size, MEM_MANAGE_ALIGNMENT_BYTE_DEFAULT);
        if (pSrc_Node->mem_size >= MEM_MANAGE_MINUM_MEM_SIZE + MEM_MANAGE_MEM_STRUCT_SIZE + want_size) { //去除分配的剩余空间足够开辟新块
            struct heap_node *pNew_Node = (struct heap_node *)((uint8_t *)mmheap_addr_add(pSrc_Node) + want_size);
            pNew_Node->next_node = NULL;
            pNew_Node->mem_size = pSrc_Node->mem_size - want_size - MEM_MANAGE_MEM_STRUCT_SIZE;
            pSrc_Node->mem_size = want_size;
            mmheap_insert_node_to_freelist(pRoot, pNew_Node);
        }
        pSrc_Node->mem_size |= MEM_MANAGE_ALLOCA_LABAL; //恢复分配标记
        MMHEAP_UNLOCK();
    } else {
        MMHEAP_UNLOCK();
        pReturn = mmheap_align_alloc(pRoot, MEM_MANAGE_ALIGNMENT_BYTE_DEFAULT, want_size);
        if (pReturn == NULL) {
            pSrc_Node->mem_size |= MEM_MANAGE_ALLOCA_LABAL; //恢复分配标记
            MMHEAP_MALLOC_FAIL();
            return NULL;
        }
        MMHEAP_LOCK();
        memcpy(pReturn, src_addr, pSrc_Node->mem_size);
        pSrc_Node->mem_size |= MEM_MANAGE_ALLOCA_LABAL; //恢复分配标记
        MMHEAP_UNLOCK();
        mmheap_free(pRoot, src_addr);
    }
    return pReturn;
}

//释放内存
void mmheap_free(struct heap_info *pRoot, void *addr)
{
    struct heap_node *pFree_Node;
    MMHEAP_ASSERT(pRoot->pStart != NULL);
    MMHEAP_ASSERT(pRoot->pEnd != NULL);
    MMHEAP_LOCK();
    if (addr == NULL) {
        MMHEAP_UNLOCK();
        return;
    }
    pFree_Node = mmheap_addr_sub(addr);

    if ((pFree_Node->mem_size & MEM_MANAGE_ALLOCA_LABAL) == 0) { //释放错误，没有标记
        MMHEAP_UNLOCK();
        MMHEAP_ASSERT((pFree_Node->mem_size & MEM_MANAGE_ALLOCA_LABAL) != 0);
        return;
    }

    if (pFree_Node->next_node != NULL) { //释放错误
        MMHEAP_UNLOCK();
        MMHEAP_ASSERT(pFree_Node->next_node == NULL);
        return;
    }
    pFree_Node->mem_size &= ~MEM_MANAGE_ALLOCA_LABAL;  //清除分配标记
    mmheap_insert_node_to_freelist(pRoot, pFree_Node); //插入到空闲链表中
    MMHEAP_UNLOCK();
}

void mmheap_init(struct heap_info *pRoot, const struct heap_region *pRegion)
{
    struct heap_node *align_addr;
    size_t align_size;
    struct heap_node *pPriv_node = NULL;

    pRoot->total_size = 0;
    pRoot->pEnd = NULL;
    pRoot->pStart = NULL;

    for (; pRegion->addr != NULL; pRegion++) {
        align_addr = (struct heap_node *)mmheap_align_up((size_t)pRegion->addr, MEM_MANAGE_ALIGNMENT_BYTE_DEFAULT); //计算内存块对齐后的地址
        if ((uint8_t *)align_addr > pRegion->mem_size + (uint8_t *)pRegion->addr)                                   //对齐消耗的内存超过内存区
            continue;
        align_size = pRegion->mem_size - ((uint8_t *)align_addr - (uint8_t *)pRegion->addr); //计算对齐后剩下的内存大小
        if (align_size < MEM_MANAGE_MINUM_MEM_SIZE + MEM_MANAGE_MEM_STRUCT_SIZE)             //对齐剩下的内存太小
            continue;
        align_size -= MEM_MANAGE_MEM_STRUCT_SIZE; //求除去掉表头后内存块的大小
        align_addr->mem_size = align_size;
        align_addr->next_node = NULL;
        if (pRoot->pStart == NULL) {                                                                      //如果是初始化
            pRoot->pStart = align_addr;                                                                   //将当前内存块地址记为start
            if (align_size >= MEM_MANAGE_MINUM_MEM_SIZE + MEM_MANAGE_MEM_STRUCT_SIZE) {                   //若剩下的块足够大
                align_size -= MEM_MANAGE_MEM_STRUCT_SIZE;                                                 //去掉下一个块的表头剩下的内存大小
                align_addr = (struct heap_node *)((uint8_t *)pRoot->pStart + MEM_MANAGE_MEM_STRUCT_SIZE); //下一个块的表头地址
                align_addr->mem_size = align_size;
                align_addr->next_node = NULL;
                pRoot->pStart->mem_size = 0;
                pRoot->pStart->next_node = align_addr;
                pRoot->total_size = align_addr->mem_size;
            } else { //内存太小了，将当前内存块地址记为start
                pRoot->total_size = 0;
                pRoot->pStart->mem_size = 0;
            }
        } else {
            pPriv_node->next_node = align_addr; //更新上一节点的next_node
            pRoot->total_size += align_size;
        }
        pPriv_node = align_addr;
    }
    //此时，pPriv_node为最后一个块，接下来在块尾放置表尾end
    //求出放置end块的地址,end块仅是方便遍历使用，因此尽量小，分配为MEM_MANAGE_MEM_STRUCT_SIZE
    align_addr = (struct heap_node *)mmheap_align_down(
        (size_t)mmheap_addr_add(pPriv_node) + pPriv_node->mem_size - MEM_MANAGE_MEM_STRUCT_SIZE, MEM_MANAGE_ALIGNMENT_BYTE_DEFAULT);
    align_size = (uint8_t *)align_addr - (uint8_t *)mmheap_addr_add(pPriv_node); //求出分配出end块后，前一个块剩余大小
    if (align_size >= MEM_MANAGE_MINUM_MEM_SIZE) {                               //若剩下的块足够大
        pRoot->total_size -= pPriv_node->mem_size - align_size;                  //去掉分配end块消耗的内存
        pRoot->pEnd = align_addr;                                                //更新表尾的地址
        pPriv_node->next_node = align_addr;
        pPriv_node->mem_size = align_size;
        align_addr->next_node = NULL;
        align_addr->mem_size = 0; //end块不参与内存分配，因此直接为0就可以
    } else {                      //最后一个块太小了，直接作为end块
        pRoot->pEnd = pPriv_node;
        pRoot->total_size -= pPriv_node->mem_size;
    }
    MMHEAP_ASSERT(pRoot->pStart != NULL);
    MMHEAP_ASSERT(pRoot->pEnd != NULL);
}
