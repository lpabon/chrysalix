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
 
#ifndef _CX_SCHED_H
#define _CX_SCHED_H

/*****************************************************************
 * Defines
 */


/** 
 * Thread States
 * @{
 */
#define TH_DEAD         (0)
#define TH_RUNNING      (1 << 0)
#define TH_SLEEPING     (1 << 1)
#define TH_SUSPENDED    (1 << 2)
#define TH_SEMWAIT      (1 << 3)
#define TH_MSG_REPLY    (1 << 4)

#define TH_HALTED       (1 << 8)
#define TH_SIG_CONTEXT  (1 << 9)
/** @} */

#define CX_SCHED_IS_PCB_DEAD(pcb)       (TH_DEAD == (pcb)->th_state)

/*****************************************************************
 * Structures
 */
#define     TH_ATTR_NONE            0x0
#define     TH_STACK_ALLOC          0x1
#define     TH_EVENT_PEND           0x2
#define     TH_ASYNC_EVENT_PEND     0x4
#define     TH_ASYNC_EVENT_INTR     0x8

typedef struct
{
    void (*fnc) (i32 arg);
    i32 arg;
} ThreadFncEntry_t;

typedef struct
{
    u8 *stack;
    u32 stack_size;

} ThreadStack_t;

typedef struct
{
    void (*eventhandler) (i32 val);
    i32 val;
} ThreadEventHandler_t;

typedef struct
{
    struct semaphore  sem_recv;
    struct semaphore   sem_send;
    struct msg   *sent_msg;
} ThreadPort_t;

typedef struct
{
    ThreadFncEntry_t        entry_info;
    ThreadStack_t           stack_info;
    ThreadEventHandler_t    eventhandler_info;
    ThreadPort_t            th_port;
    struct context              ctx;
    struct queue                 sem_link;
    u32                  th_attr;
    u32                  th_state;
    char                    th_name[ARCH_MAX_THREAD_NAME + 1];
    u32                  avg_time;
    u32                  num_times_run;
    u64                  sleep_time;
    u64                  alarm_time;
    u32                  wait_val;
    fd_t                    uistream;

} PCB_t;



/*****************************************************************
 * Prototypes
 */
void  cx_sched_init(void);
void  cx_sched_schedule(void);
void  cx_sched_start(void);
i32   cx_in_signal_context( void );
i32   cx_thread_set_state( i32      pid, u32  new_state);
i32   cx_thread_set_state_pcb( PCB_t      *pcb, u32 new_state);

PCB_t *cx_get_current_pcb(void);
PCB_t *cx_get_sched_pcb(void);
PCB_t *cx_get_pcb(i32 pid);

/*****************************************************************
 * Arch Dep functions
 */
void arch_context_init(void);
void arch_context_set(PCB_t * pcb);
void arch_context_create_sched(PCB_t * sched_pcb);
void arch_context_switch(struct context * prev, struct context * next);
void arch_context_switch_start(void);
void arch_context_save(struct context * ctx, int excls);
void arch_context_restore(struct context * ctx, void *regs);
void arch_context_print(struct context *ctx);
void arch_yield(void);
u64 arch_get_mtime(void);
i32 arch_drivers_load( void );

#endif /* _CX_SCHED_H */
