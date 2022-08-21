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

/************************************************************************************
 * Functions
 */
i32 cx_alarm(i32 msec) {
    PCB_t *current_pcb;

    current_pcb = cx_get_current_pcb();
    current_pcb->alarm_time = (u64) (msec) + arch_get_mtime();
    return (0);
}

i32 cx_kill(i32 pid, i32 sig) {
    switch (sig) {
    case SIGTERM:
        return (cx_thread_end(pid));

    case SIGSTOP:
        return (cx_thread_set_state(pid, TH_SUSPENDED));

    case SIGCONT:
        {
            PCB_t *pcb;

            pcb = cx_get_pcb(pid);
            if (NULL == pcb) {
                errno = EINVAL;
                return (-1);
            }
            if (TH_SUSPENDED != pcb->th_state) {
                errno = EACCES;
                return (-1);
            }
            return (cx_thread_set_state(pid, TH_RUNNING));
        }

    default:
        return (cx_event_send(pid, sig));
    }

    return (0);

}
