/*-
 * Copyright (c) 2006 Luis Pabon, Jr. <lpabon@chrysalix.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


/************************************************************************************
 * Includes
 */
#include <chrysalix.h>

/************************************************************************************
 * Defines
 */
    /** Max heap size in blocks */
#define MAXHEAPSIZE(heapsize)       (((heapsize) - sizeof(struct mem)) / sizeof(struct mem))
#define cx_get_heap(HeapType)       (&heaplist[ (HeapType) ])

/************************************************************************************
 * Prototypes
 */
static u32 cx_heap_bytes_free(struct heap *heap);

static i32 do_mem(i32 argc _UNUSED_, char **argv _UNUSED_);
static i32 do_free(i32 argc _UNUSED_, char **argv _UNUSED_);

/************************************************************************************
 * Globals
 */
static struct heap heaplist[HEAP_NUM];

static const struct console_fnc g_console_fncs[] = {
    { "free", do_free },
    { "mem", do_mem }
};

static struct console_fnc_list g_console_fnclist;

void cx_mem_init(void) {
    /*
     * Register function list
     */
    CX_CONSOLE_CREATE(g_console_fncs, g_console_fnclist);

}

i32 cx_heap_init(u8 * mem, u32 size, enum heap_type heaptype) {
    struct mem *first;
    struct heap *heap;

    /*
     * Check arguments
     */
    if ((NULL == mem) || (0 == size)) {
        errno = EINVAL;
        return (-1);
    }

    if (heaptype >= HEAP_NUM) {
        errno = ERANGE;
        return (-1);
    }

    /*
     * Clear it
     */
    memset((void *) mem, 0x0, size);

    /*
     * Point to the heap
     */
    heap = cx_get_heap(heaptype);
    heap->heap_start = (struct mem *) mem;
    heap->heap_size = size;
    first = heap->heap_start + 1;

    /*
     * Initialize the (*heap) of the heap
     */
    heap->heap_start->size = 0;
    heap->heap_start->next = first;

    /*
     * Setup the first header
     */
    first->next = heap->heap_start;
    first->size = MAXHEAPSIZE(size);

    /*
     * Initialize semaphore
     */
    sem_init(&heap->sem_wait_for_mem, 0);

    /*
     * Return
     */
    return (0);

}

static u32 cx_heap_bytes_free(struct heap *heap) {
    u32 count = 0;
    struct mem *p = heap->heap_start;

    do {
        count += p->size * sizeof(struct mem);
        p = p->next;
    }
    while (p != heap->heap_start);

    return count;

}


void *cx_heap_malloc(u32 nbytes, enum cx_mem_attr attr,
                     enum heap_type heaptype) {

    struct mem *p;
    struct mem *prev;
    struct heap *heap;
    u32 nunits;

    /*
     * Check parameters
     */
    if (heaptype >= HEAP_NUM) {
        errno = ERANGE;
        return (NULL);
    }

    /*
     * Check if the user has asked for more data than available
     */
    heap = cx_get_heap(heaptype);
    if (nbytes >= heap->heap_size)
        return (NULL);

    nunits = (nbytes + sizeof(struct mem) - 1) / sizeof(struct mem) + 1;
    prev = heap->heap_start;
    p = prev->next;
    while (1) {
        if (p->size >= nunits) {
            if (p->size == nunits) {
                //an exact fit!
                prev->next = p->next;
            } else {
                //block is bigger than we need, split it
                p->size -= nunits;

                p += p->size;
                p->size = nunits;

            }
            return (void *) (p + 1);

        }

        /*
         * wrapped around - no block found
         */
        if (p == heap->heap_start) {
            if (KM_CXEEP == attr)
                sem_wait(&heap->sem_wait_for_mem);
            else
                return NULL;
        }

        prev = p;
        p = p->next;
    }

}

void cx_heap_free(void *mem, enum heap_type heaptype) {

    struct mem *blk_hdr;
    struct mem *prev;
    struct mem *next;
    struct heap *heap;

    i32 sem_value;

    /*
     * Check parameters
     */
    if ((heaptype >= HEAP_NUM) || (NULL == mem))
        return;

    blk_hdr = (struct mem *) mem - 1;

    heap = cx_get_heap(heaptype);
    prev = heap->heap_start;
    next = heap->heap_start->next;
    do {

        if (next > blk_hdr)
            break;              //next is now pointing at the next free block after blk_hdr
        prev = next;
        next = next->next;

    }
    while (next != heap->heap_start);


    blk_hdr->next = next;
    prev->next = blk_hdr;
    //block is now back in the list


    //next we see if we can merge any adjacent blocks with this one

    if (blk_hdr + blk_hdr->size == next) {
        blk_hdr->size += blk_hdr->next->size;
        blk_hdr->next = blk_hdr->next->next;

    }

    if ((prev != heap->heap_start) && (prev + prev->size == blk_hdr)) {

        prev->size += blk_hdr->size;
        prev->next = blk_hdr->next;

    }

    /*
     * Check semaphore
     */
    sem_getvalue(&heap->sem_wait_for_mem, &sem_value);
    if (sem_value < 0)
        sem_post(&heap->sem_wait_for_mem);


}

static i32 do_free(i32 argc _UNUSED_, char **argv _UNUSED_) {
    u32 i;
    struct heap *heap;

    for (i = 0; i < HEAP_NUM; i++) {
        heap = cx_get_heap(i);
        printf("Mem[%u]: Total=%u  Free=%u\n",
               i, heap->heap_size, cx_heap_bytes_free(heap));
    }
    return (0);
}

static i32 do_mem(i32 argc _UNUSED_, char **argv _UNUSED_) {
    struct mem *p;
    u32 i;

    for (i = 0; i < HEAP_NUM; i++) {
        p = cx_get_heap(i)->heap_start;
        printf("Memory %u\n", i);
        do {
            printf("p 0x%X - z %d\n", (u32) p,
                   p->size * sizeof(struct mem));
            p = p->next;

        } while (p != cx_get_heap(i)->heap_start);
    }

    return (0);
}
