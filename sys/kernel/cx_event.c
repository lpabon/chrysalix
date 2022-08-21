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
#include <arch_context.h>
#include "cx_sched.h"

/**
 * Timeout in msecs
 */
i32 cx_event_wait(u32 val, i32 timeout) {
    PCB_t *current_pcb;
    i32 s;

    /*
     * Check arguments.
     *
     * The value cannot be zero because this is the initial value when
     * a thread is not waiting for an event.
     */
    if (0 == val) {
        errno = EINVAL;
        return (-1);
    }

    /*
     * Get pcb
     */
    current_pcb = cx_get_current_pcb();

    /*
     * Turn off interrupts so that we can check the flags
     */
    s = cx_intsoff();

    /*
     * Check if there is a pending event.  If there is
     * return
     */
    if ((current_pcb->wait_val == val) &&
        (TH_EVENT_PEND == (TH_EVENT_PEND & current_pcb->th_attr))) {
        /*
         * Clear the flag now that we have the event
         */
        current_pcb->th_attr &= ~TH_EVENT_PEND;
        cx_intson(s);
        return (0);
    }

    /*
     * Clear the event status
     */
    current_pcb->th_attr &= ~TH_EVENT_PEND;

    /*
     * Save the value
     */
    current_pcb->wait_val = val;

    /*
     * If we have a timeout, then set the state to CXEEPING,
     * else set the state just to SUSPENDED.
     */
    if (-1 < timeout) {
        current_pcb->sleep_time = (u64) timeout + arch_get_mtime();
        cx_thread_set_state_pcb(current_pcb, TH_SLEEPING);
    } else {
        current_pcb->th_state |= TH_SUSPENDED;
    }

    /*
     * Wait for the next event
     */
    if (0 > cx_yield())
        return (-1);

    /*
     * We came back, check if we timed out.  Interrupts are still off.
     */
    current_pcb->th_state &= ~TH_SUSPENDED;
    if (TH_EVENT_PEND != (TH_EVENT_PEND & current_pcb->th_attr)) {
        cx_intson(s);
        /*
         * We must have woken up because we timed out
         */
        //current_pcb->th_state &= ~TH_SLEEPING;
        errno = ETIMEDOUT;
        return (-1);
    }

    /*
     * Clear flag
     */
    current_pcb->th_attr &= ~TH_EVENT_PEND;

    /*
     * Put back interrupts
     */
    cx_intson(s);

    /*
     * Return to caller
     */
    return (0);

}

i32 cx_event_post(u32 val) {
    u32 pid;
    PCB_t *pcb;

    /*
     * Go through the list looking for the value.  Don't
     * care if they in a TH_SUSPENDED state or not.
     */
    for (pid = 0; pid < ARCH_MAX_THREADS; pid++) {
        pcb = cx_get_pcb(pid);
        if (pcb->wait_val == val) {
            if (0 <= cx_thread_set_state_pcb(pcb, TH_RUNNING))
                pcb->th_attr |= TH_EVENT_PEND;
        }
    }

    return (0);
}

i32 cx_event_register(void (*eventhandler)(i32 val)) {
    PCB_t *current_pcb;

    /*
     * Check arguments
     */
    if (NULL == eventhandler) {
        errno = EINVAL;
        return (-1);
    }

    /*
     * Save handler value
     */
    current_pcb = cx_get_current_pcb();
    current_pcb->eventhandler_info.eventhandler = eventhandler;

    /*
     * Normal return
     */
    return (0);
}

i32 cx_event_send(i32 pid, i32 val) {
    PCB_t *pcb;

    /*
     * Get PCB
     */
    pcb = cx_get_pcb(pid);

    /*
     * If the PCB exists, set that it received an
     * asynchronous event.
     */
    if (NULL != pcb) {
        /*
         * Only change the state if it was not dead.
         */
        if (!CX_SCHED_IS_PCB_DEAD(pcb)) {
            pcb->th_attr |= TH_ASYNC_EVENT_PEND;
            pcb->eventhandler_info.val = val;
            return (0);
        }

        /*
         * PCB is not allocated, so cannot send the event
         */
        errno = ESRCH;
        return (-1);
    } else {
        errno = EINVAL;
        return (-1);
    }
}
