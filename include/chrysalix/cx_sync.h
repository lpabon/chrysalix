/*-
 * Copyright (c) 2006 Luis Pabon, Jr. <lpabon@chrysalix.org>
 * Copyright (c) 2022 Isabella Pabon <isabella@chrysalix.org>
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
 
#ifndef _CX_SYNC_H
#define _CX_SYNC_H

/*****************************************************************
 * Defines
 */
#define SEM_VALUE_MAX       ((i32)((~0u)>>1))

/*****************************************************************
 * Structures
 */
/*
 * Semaphore interface
 */
struct semaphore
{
    i32          value;
    struct queue        sem_q;
};

struct mutex
{
    struct semaphore s;
};

struct waitgroup
{
    struct semaphore s;
    int wait_count;
};


/*****************************************************************
 * Prototypes
 */
i32 sem_init(struct semaphore * sem, i32 value);
i32 sem_wait(struct semaphore * sem);
i32 sem_trywait(struct semaphore * sem);
i32 sem_post(struct semaphore * sem);
i32 sem_getvalue(struct semaphore * sem, i32 * sval);
i32 sem_destroy(struct semaphore * sem);

i32 mutex_lock(struct mutex *m);
i32 mutex_unlock(struct mutex *m);
i32 mutex_init(struct mutex *m);
i32 mutex_destroy(struct mutex *m);

i32 waitgroup_init(struct waitgroup *w, int number_of_threads);
i32 waitgroup_wait(struct waitgroup *w);
i32 waitgroup_done(struct waitgroup *w);


#endif /* _CX_SYNC_H */
