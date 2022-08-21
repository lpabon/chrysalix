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
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission
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

#ifndef NULL
#define NULL 0
#endif

    /*
     * For UNIT8/u16..etc
     */
#include <stdarg.h>
#include <arch_types.h>
//#  include <arch_isr.h>
#include <arch_lib.h>

    /*
     * CX
     *
     * Cannot include <chrysalix.h> because it has conflicts with
     * stdlib and stdio
     */
#include <cx_arch.h>
#include <chrysalix/queue.h>
#include <chrysalix/list.h>
#include <chrysalix/cx_err.h>
#include <chrysalix/cx.h>
#include <chrysalix/cx_utils.h>
#include <chrysalix/cx_msg.h>
#include <chrysalix/cx_io.h>
#include <chrysalix/cx_drv.h>
#include <chrysalix/cx_stdio.h>
#include <chrysalix/cx_string.h>
#include <chrysalix/cx_sync.h>
#include <chrysalix/cx_event.h>
#include <chrysalix/cx_console.h>
#include <chrysalix/cx_mem.h>
#include <chrysalix/cx_proc.h>

#include "arch_context.h"
#include "cx_sched.h"

/****************************************************************
 * Globals
 */
#define SCHEDSTKSZ  (30*1024)
static u8 sched_stack[SCHEDSTKSZ];
static struct context blah;

/* ------------------------------------------------------------ */
static void linux_entry_point(void *arg) {
    PCB_t *pcb;

    (void) arg;
    linux_entry_point_setup();

    pcb = cx_get_current_pcb();
    if (NULL == pcb) {
        pcb = cx_get_sched_pcb();
    }
    pcb->entry_info.fnc(pcb->entry_info.arg);
    cx_thread_end(cx_getpid());

}

/* ------------------------------------------------------------ */
static void sched_schedule_thread(i32 arg) {
    while (1) {
        cx_sched_schedule();
    }
}

/**
 * Initialize architecture context library
 */
void arch_context_init(void) {
    getcontext(&blah.uctx);
}

/*
 * Initialize specified context.
 * All thread will start at the trap return routine - trap_ret().
 * In this time, the interrupt flag is enabled and I/O access
 * is disabled.
 *
 * @ctx: context id (pointer)
 * @kstack: kernel stack for the context
 */
void arch_context_set(PCB_t * pcb) {
    PCB_t *sched_pcb;
    struct context *ctx;
    ucontext_t *uc;

    sched_pcb = cx_get_sched_pcb();
    ctx = &pcb->ctx;
    uc = &ctx->uctx;

    getcontext(uc);
    uc->uc_stack.ss_sp = (void *) pcb->stack_info.stack;
    uc->uc_stack.ss_size = (size_t) pcb->stack_info.stack_size;
    uc->uc_stack.ss_flags = 0;

    if (pcb != sched_pcb) {
        uc->uc_link = &sched_pcb->ctx.uctx;
    } else {
        uc->uc_link = &blah.uctx;
    }
    makecontext(uc, linux_entry_point, 1, linux_entry_point);
}

/*
 * Switch to new context
 */
void arch_context_switch(struct context *prev, struct context *next) {
    //pth_uctx_switch((pth_uctx_t) prev->uctx, (pth_uctx_t) next->uctx);
    swapcontext(&prev->uctx, &next->uctx);
}

void arch_context_switch_start(void) {
    arch_context_switch(&blah, &(cx_get_sched_pcb())->ctx);
}

void arch_context_create_sched(PCB_t * sched_pcb) {
    strncpy(sched_pcb->th_name, "sched", ARCH_MAX_THREAD_NAME);
    sched_pcb->th_state = TH_RUNNING;
    sched_pcb->stack_info.stack = sched_stack;
    sched_pcb->stack_info.stack_size = SCHEDSTKSZ;
    sched_pcb->entry_info.fnc = sched_schedule_thread;
    sched_pcb->entry_info.arg = 0;
    arch_context_set(sched_pcb);
}

/*
 * Save user mode context to handle exceptions.
 *
 * @exc: exception code passed to the exception handler
 *
 * Copy current user mode registers in the kernel stack to the user
 * mode stack. The user stack pointer is adjusted for this area.
 * So that the exception handler can get the register state of
 * the target thread.
 *
 * It builds arguments for the exception handler in the following
 * format.
 *
 *   void exception_handler(int exc, void *regs);
 */
void arch_context_save(struct context *ctx, int exc) {
    /* TODO: need to handle ARM exception */
}

/*
 * Restore register context to return from the exception handler.
 *
 * @regs: pointer to user mode register context.
 */
void arch_context_restore(struct context *ctx, void *regs) {

}

void arch_context_print(struct context *ctx) {
    printf("LinuxUcontextStuffHere\n");
}

/**
 * Yield !
 */
void arch_yield(void) {
    PCB_t *sched, *pcb;

    pcb = cx_get_current_pcb();
    sched = cx_get_sched_pcb();
    arch_context_switch(&pcb->ctx, &sched->ctx);

}

u64 arch_get_mtime(void) {
    return (linux_get_mtime());
}
