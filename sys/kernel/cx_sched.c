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
#include "arch_context.h"
#include "cx_sched.h"
#include "cx_sched_console.h"

/************************************************************************************
 * Defines
 */
#define THREAD_GETID()          ( (i32)((PCB_t *)current_pcb - (PCB_t *)pcblist) )

/************************************************************************************
 * Prototypes
 */
static i32 cx_thread_alloc(void);
static void cx_set_current_pcb(PCB_t * pcb);
static PCB_t *cx_sched_get_next_thread(void);
static void dummy_handler(i32 val);

/************************************************************************************
 * Globals
 */
static PCB_t pcblist[ARCH_MAX_THREADS];
static PCB_t *current_pcb;
static PCB_t sched_pcb;

/************************************************************************************
 * Functions
 */

 /**
  * @defgroup cxgrp_thread Thread API
  *
  *     These set of function can be used to change or get
  *     thread state.
  *
  */

 /**
  * @defgroup cxgrp_kernel_only Thread API for Kernel Only
  *
  *     These set of functions are available for the kernel only.
  *     They are not available to applications.
  *
  *     @ingroup cxgrp_thread
  *
  */

/**
 * @defgroup cxgrp_os_start Systerm Startup Sequence
 *
 *      These functions are used to initialize the operating
 *      system.
 */

 /**
  *     Get process id for the current running process
  *
  * @ingroup cxgrp_thread
  *
  * @retval -1
  *     Kernel is not up yet, so there is not thread running
  *
  * @return Process ID as i32
  *
  * @note
  *      - Call will not block
  */
i32 cx_getpid(void) {
    if (NULL == current_pcb) {
        return (-1);
    } else {
        return (THREAD_GETID());
    }
}

/**
 *      End process referred by process id @p pid.  Process
 *      state will be changed to dead.
 *
 * @ingroup cxgrp_thread
 *
 * @param[in] pid
 *      Process id of process to end.
 *
 * @retval 0
 *      Success
 * @retval -ERANGE
 *      PID is out of range
 *
 * @note
 *      - Call will not block
 *
 * @sa cx_findproc()
 *
 * @bug
 *      - What will happen it is in a semaphore queue?
 */
i32 cx_thread_end(i32 pid) {
    PCB_t *pcb;

    /*
     * Check argument is in range
     */
    if ((0 > pid) || (pid >= ARCH_MAX_THREADS)) {
        return (-ERANGE);
    }

    /*
     * Get PCB
     */
    pcb = cx_get_pcb(pid);

    /*
     * BAM!
     */
    pcb->th_state = TH_DEAD;

    /*
     * Check the attributes to see if we need to free
     * the stack
     */
    if (TH_STACK_ALLOC == (TH_STACK_ALLOC & pcb->th_attr))
        (void) cx_kfree(pcb->stack_info.stack);

    /*
     * Return
     */
    return (0);
}

/**
 *      Return the process id with the specified process name.
 *
 * @ingroup cxgrp_thread
 *
 * @param[in] name
 *      Points to the name of the process
 *
 * @retval -ENOENT
 *      Name was not found in running processes
 *
 * @return
 *      Process ID matching the name provided
 *
 */
i32 cx_findproc(const char *name) {
    i32 pid;
    PCB_t *pcb;

    /*
     * Search for the name through the list of PCBs.
     * Return as soon as we find the first one.
     */
    for (pid = 0; pid < ARCH_MAX_THREADS; pid++) {
        pcb = cx_get_pcb(pid);
        if (!CX_SCHED_IS_PCB_DEAD(pcb)) {
            if (0 == strncmp(pcb->th_name, name, ARCH_MAX_THREAD_NAME))
                return (pid);
        }
    }

    /*
     * Name not found
     */
    return (-ENOENT);
}

/**
 *      Return the name of the process specified by its
 *      process id.
 *
 * @ingroup cxgrp_thread
 *
 * @param[in] pid
 *      The process id of the thread
 * @param[out] name
 *      Points a memory location where to store
 *      the name of the thread
 * @param[in] size
 *      Size of the memory pointed by @p name.
 *
 * @retval -EINVAL
 *      Value of @p name is @p NULL.
 * @retval -ERANGE
 *      Value of @p pid is out of range.
 * @retval -ESRCH
 *      Name was not found in running processes
 *
 * @return
 *      Process ID matching the name provided
 *
 */
i32 cx_findname(i32 pid, char *name, u32 size) {
    PCB_t *pcb;

    /*
     * Check parameters
     */
    if (NULL == name) {
        return (-EINVAL);
    }
    if ((0 > pid) || (pid >= ARCH_MAX_THREADS)) {
        return (-ERANGE);
    }

    /*
     * Get the name of the thread
     */
    pcb = cx_get_pcb(pid);
    if (!CX_SCHED_IS_PCB_DEAD(pcb)) {
        strncpy(name, cx_get_pcb(pid)->th_name, size);
        return (0);
    } else {
        /*
         * Pid specified is not running
         */
        name[0] = '\0';
        return (-ESRCH);
    }
}

/**
 *      Return the address of the PCB for the thread currently
 *      running.
 *
 * @ingroup cxgrp_kernel_only
 *
 * @return
 *      (PCB_t *) to current thread PCB
 *
 */
PCB_t *cx_get_current_pcb(void) {
    return (current_pcb);
}

/**
 *      Return the address of the PCB for the scheduler.
 *
 * @ingroup cxgrp_kernel_only
 *
 * @return
 *      (PCB_t *) to scheduler thread PCB
 *
 */
PCB_t *cx_get_sched_pcb(void) {
    return (&sched_pcb);
}

/**
 *      Return the PCB address for the specified process id.
 *
 * @ingroup cxgrp_kernel_only
 *
 * @param[in] pid
 *      The process id of the thread
 *
 * @retval NULL
 *      Name was not found in running processes
 * @return
 *      (PCB_t *) to PCB for the process specified
 *
 */
PCB_t *cx_get_pcb(i32 pid) {
    if ((0 <= pid) && (pid < ARCH_MAX_THREADS)) {
        return (&pcblist[pid]);
    } else {
        return (NULL);
    }
}

/**
 *  Initializes the scheduler.  This function also initializes
 *  the architecture specific context function (which start
 *  with 'arch').  After initialization, it registers the console
 *  commands with the Console API.
 *
 *  @ingroup cxgrp_os_start
 *
 *
 */
void cx_sched_init(void) {
    /*
     * Initalize Arch Context
     */
    arch_context_init();

    /*
     * Initialize Sched thread
     */
    arch_context_create_sched(&sched_pcb);

    /*
     * Clear current pointer to show that we are just starting up
     */
    cx_set_current_pcb(NULL);

    /*
     * Register console
     */
    cx_sched_console_init();

}

/* ------------------------------------------------------------ */
i32 cx_thread_set_state(i32 pid, u32 new_state) {
    PCB_t *pcb;

    /*
     * Get the PCB
     */
    pcb = cx_get_pcb(pid);

    if (NULL == pcb) {
        errno = ESRCH;
        return (-1);
    }

    return (cx_thread_set_state_pcb(pcb, new_state));
}

i32 cx_thread_set_state_pcb(PCB_t * pcb, u32 new_state) {

    if ((NULL != pcb) && (!CX_SCHED_IS_PCB_DEAD(pcb))) {
        switch (new_state) {
        case TH_RUNNING:
        case TH_SUSPENDED:
        case TH_SEMWAIT:
        case TH_SLEEPING:
        case TH_MSG_REPLY:
            pcb->th_state = (pcb->th_state & 0xff00) | new_state;
            break;
        }

        return (0);
    }

    errno = EINVAL;
    return (-1);
}


i32 cx_thread_halt(i32 pid) {
    PCB_t *pcb;

    pcb = cx_get_pcb(pid);

    if (NULL == pcb) {
        errno = ESRCH;
        return (-1);
    }

    pcb->th_state |= TH_HALTED;
    return (0);
}

/* ------------------------------------------------------------ */
i32
cx_thread_start(char *name,
                u8 * stack, u32 stacksize, void (*fnc)(i32 arg), i32 arg) {

    PCB_t *pcb;
    i32 pid;

    /*
     * Check params
     */
    if ((NULL == name) || (NULL == fnc) || (0 == stacksize)) {
        errno = EINVAL;
        return (-1);
    }

    /*
     * Allocate a PCB
     */
    pid = cx_thread_alloc();
    if (0 > pid) {
        errno = ENOSPC;
        return (-1);
    }

    /*
     * Clear thread attributes
     */
    pcb = cx_get_pcb(pid);
    memset(pcb, 0x0, sizeof(PCB_t));
    pcb->th_attr = TH_ATTR_NONE;

    /*
     * Allocate a stack if it has not been given
     */
    if (NULL == stack) {
    /************* FIXME ********** USE cx_hal_getinfo() */
        if (ARCH_MIN_STACK_SIZE > stacksize)
            stacksize = ARCH_MIN_STACK_SIZE;
        stack = (u8 *) cx_kmalloc(stacksize, KM_NOCXEEP);
        if (NULL == stack) {
            errno = ENOMEM;
            return (-1);
        }

        /*
         * Set the attribute so that we free the memory when
         * the thread has finished
         */
        pcb->th_attr |= TH_STACK_ALLOC;
    }

    /*
     * Initialize PCB information
     */
    strncpy(pcb->th_name, name, ARCH_MAX_THREAD_NAME);
    pcb->th_state = TH_RUNNING;
    pcb->stack_info.stack = stack;
    pcb->stack_info.stack_size = stacksize;
    pcb->entry_info.fnc = fnc;
    pcb->entry_info.arg = arg;
    pcb->num_times_run = 0;
    pcb->eventhandler_info.eventhandler = dummy_handler;

    /*
     * Initialize UI
     */
    if (0 > cx_getpid())
        pcb->uistream = llstdout;
    else
        pcb->uistream = stdout;

    /*
     * Initialize port
     */
    sem_init(&pcb->th_port.sem_recv, 0);
    sem_init(&pcb->th_port.sem_send, 1);

    /*
     * Initialize architecture process context
     */
    arch_context_set(pcb);

    return (pid);
}


/* ------------------------------------------------------------ */
void cx_sched_schedule(void) {
    PCB_t *next;

    /*
     * Get next PCB
     *
     * Keep checking the threads.  Maybe they are all sleeping
     * and we need to wake them up.
     */
    do {
        next = cx_sched_get_next_thread();
    } while (NULL == next);

    /*
     * Change States
     */
    next->num_times_run++;
    cx_set_current_pcb(next);
    arch_context_switch(&sched_pcb.ctx, &next->ctx);
}

/* ------------------------------------------------------------ */
void cx_sched_start(void) {
    arch_context_switch_start();
}

i32 cx_in_signal_context(void) {
    return (TH_SIG_CONTEXT == current_pcb->th_state);
}

/* ------------------------------------------------------------ */
static i32 cx_thread_signal_service(void) {

    /*
     * Clear the SIGNALED flag first in case we get
     * signaled again
     */
    current_pcb->th_attr &= ~TH_ASYNC_EVENT_PEND;

    /*
     * Check if the current thread has a registered signal handler
     */
    if (NULL != current_pcb->eventhandler_info.eventhandler) {
        /*
         * Set the state to show that we are in signal context
         *
         * XXXXXXXXXXXXXX FIX THIS - if we set this here we will not be scheduled again! XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
         */
        current_pcb->th_state = TH_SIG_CONTEXT;

        /*
         * Call signal handler
         */
        current_pcb->eventhandler_info.
            eventhandler(current_pcb->eventhandler_info.val);

        /*
         * We are now out of signal context
         */
        current_pcb->th_state = TH_RUNNING;
    }

    /*
     * Check if we got interrupted from sleep
     */
    if (0 != (TH_ASYNC_EVENT_INTR & current_pcb->th_attr)) {
        errno = EINTR;
        return (-1);
    }

    /*
     * Normal return
     */
    return (0);

}

/* ------------------------------------------------------------ */
i32 cx_yield(void) {
    /*
     * Yield the cpu
     */
    arch_yield();

    /*
     * Check if we have been signaled and we are not in signal context.
     *
     * To be faster, first check that we have not been signalled or that
     * we an in signal context.
     */
    if ((0 == (TH_ASYNC_EVENT_PEND & current_pcb->th_attr)) ||
        (cx_in_signal_context())) {
        return (0);
    } else {
        return (cx_thread_signal_service());
    }
}

/* ------------------------------------------------------------ */
i32 cx_msleep(u32 msecs) {
    i32 retval;

    current_pcb->sleep_time = (u64) msecs + arch_get_mtime();
    cx_thread_set_state_pcb(current_pcb, TH_SLEEPING);

    /*
     * We don't care if yield() comes back from a signal
     * handler.  The calculations will still work and errno
     * will be filled in by yield()
     */
    (void) cx_yield();

    /*
     * Calculate how much time we need to sleep
     */
    retval = (i32) (current_pcb->sleep_time - arch_get_mtime());
    if (0 >= retval) {
        return (0);
    } else {
        return (retval);
    }
}

    /** XXX MOVE TO ANOTHER FILE XXX */
i32 cx_usleep(u32 usecs _UNUSED_) {
    return (cx_yield());
}

/* ------------------------------------------------------------ */
i32 cx_set_uistream(fd_t fd) {
    if ((0 <= fd) && (fd < ARCH_MAX_DESCRIPTORS)) {
        cx_get_current_pcb()->uistream = fd;
        return (0);
    } else {
        errno = EINVAL;
        return (-1);
    }

}

/* ------------------------------------------------------------ */
fd_t cx_get_uistream(void) {
    return (cx_get_current_pcb()->uistream);
}

/************************************************************************************
 * Private Functions
 */

/* ------------------------------------------------------------ */
static i32 cx_thread_alloc(void) {
    i32 i;

    for (i = 0; i < ARCH_MAX_THREADS; i++) {
        if (CX_SCHED_IS_PCB_DEAD(&pcblist[i])) {
            return (i);
        }
    }

    /*
     * If we are here, then we were unable to find a free PCB
     */
    errno = ENOMEM;
    return (-1);
}

/* ------------------------------------------------------------ */
static void cx_set_current_pcb(PCB_t * pcb) {
    current_pcb = pcb;
}

/* ------------------------------------------------------------ */
/* XXXXXXXXXX MAYBE ADD THIS TO cx_sched_schedule XXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/
static PCB_t *cx_sched_get_next_thread(void) {
    i32 pid;
    i32 i;

    pid = cx_getpid();
    i = pid;

    /*
     * Search for the next available thread
     *
     * Note: TH_DEAD is mutual exclusive state
     *
     */
    do {
        if (++i >= ARCH_MAX_THREADS) {
            i = 0;
        }

        if (TH_DEAD == pcblist[i].th_state)
            continue;

        if (0 != (pcblist[i].th_state & TH_HALTED))
            continue;

        /*
         * Check if the alarm has gone off
         */
        if ((0 != pcblist[i].alarm_time)
            && (pcblist[i].alarm_time <= arch_get_mtime())) {
            pcblist[i].alarm_time = 0;
            cx_kill(i /* pid */ , SIGALRM);
        }

        /*
         * Check for asynchronous events
         */
        if (TH_ASYNC_EVENT_PEND ==
            (TH_ASYNC_EVENT_PEND & pcblist[i].th_attr)) {
            if (TH_RUNNING != pcblist[i].th_state) {
                /*
                 * We woke you up!  WAKE UP!
                 */
                pcblist[i].th_attr |= TH_ASYNC_EVENT_INTR;
            }

            pcblist[i].th_state = TH_RUNNING;
        }

        /*
         * Check to see if the thread needs to wake up
         */
        if (TH_SLEEPING == (TH_SLEEPING & pcblist[i].th_state)) {
            if (pcblist[i].sleep_time <= arch_get_mtime()) {
                pcblist[i].th_state = TH_RUNNING;
            }
        }

        if (TH_RUNNING == pcblist[i].th_state) {
            return (&pcblist[i]);
        }

    } while (i != pid);

    /*
     * Unable to find a running process
     */
    return (NULL);
}

static void dummy_handler(i32 val) {
    printf("---* %d: GOT EVENT %d *---\n", cx_getpid(), val);
}
