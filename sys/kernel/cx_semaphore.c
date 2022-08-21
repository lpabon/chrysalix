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

/***************************************************************************
**
** File Name:   cx_semaphore.c
**
**
**
** Purpose:
**              OS Semaphores
**
****************************************************************************/

/************************************************************************************
 * Includes
 */
#include <chrysalix.h>
#include "cx_arch.h"
#include "arch_context.h"
#include "cx_sched.h"

/************************************************************************************
 * Structures
 */
enum sem_wait {
    CX_SEM_WAIT,
    CX_SEM_TRYWAIT
};

/************************************************************************************
 * Prototypes
 */
static i32 cx_sem_get(struct semaphore *sem, enum sem_wait waittype);

/************************************************************************************
 * Functions
 */
i32 sem_init(struct semaphore *sem, i32 value) {
    if ((NULL == sem) || (SEM_VALUE_MAX < value)) {
        errno = EINVAL;
        return (-1);
    }

    /*
     * Check that we are not initializing a semaphore that
     * has already been initialized
     */
    if (NULL != queue_next(&sem->sem_q)) {
        errno = EACCES;
        return (-1);
    }

    /*
     * Initialize semaphore
     */
    queue_init(&sem->sem_q);
    sem->value = value;

    return (0);
}

i32 sem_wait(struct semaphore *sem) {
    return (cx_sem_get(sem, CX_SEM_WAIT));
}

i32 sem_trywait(struct semaphore *sem) {
    return (cx_sem_get(sem, CX_SEM_TRYWAIT));
}

i32 sem_post(struct semaphore *sem) {
    i32 s;
    PCB_t *pcb;
    struct queue *q_elem;

    if (NULL == sem) {
        errno = EINVAL;
        return (-1);
    }

    s = cx_intsoff();

    if (sem->value < SEM_VALUE_MAX) {
        if (++sem->value <= 0) {
            q_elem = dequeue(&sem->sem_q);
            if (NULL != q_elem) {
                pcb = queue_entry(q_elem, PCB_t, sem_link);
                (void) cx_thread_set_state_pcb(pcb, TH_RUNNING);
            }
        }

        cx_intson(s);
        return (0);
    }

    /*
     * Out of range
     */
    cx_intson(s);
    errno = ERANGE;
    return (-1);
}


i32 sem_getvalue(struct semaphore *sem, i32 * sval) {

    if ((NULL == sem) || (NULL == sval)) {
        errno = EINVAL;
        return (-1);
    }

    *sval = sem->value;

    return (0);
}

i32 sem_destroy(struct semaphore *sem) {
    if (NULL == sem) {
        errno = EINVAL;
        return (-1);
    }

    errno = ENOTSUP;
    return (-1);
}

/************************************************************************************
 * Private Functions
 */

static i32 cx_sem_get(struct semaphore *sem, enum sem_wait waittype) {
    i32 s;
    PCB_t *current_pcb;

    if (NULL == sem) {
        errno = EINVAL;
        return (-1);
    }

    s = cx_intsoff();

    sem->value--;
    if (sem->value < 0) {
        if (CX_SEM_WAIT == waittype) {
            current_pcb = cx_get_current_pcb();
            cx_thread_set_state_pcb(current_pcb, TH_SEMWAIT);
            enqueue(&sem->sem_q, &current_pcb->sem_link);

            /*
             * sem_wait always returns 0 and cannot be interrupted.
             */
            while (0 > cx_yield());
        } else {
            sem->value++;       /* Put it back */
            cx_intson(s);
            errno = EAGAIN;
            return (-1);
        }
    }


    cx_intson(s);
    return (0);
}
