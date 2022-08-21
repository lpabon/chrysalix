/*-
 * Copyright (c) 2022 Isabella Pabon <isabella@chrysalix.org>
 * Copyright (c) 2022 Luis Pabon <lpabon@chrysalix.org>
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

#include <chrysalix.h>

// Defines
#define STACK_SIZE (16*1024)

// Prototypes
void add(i32 arg);

void test_threading(void) {
    test_sync();
}

// Global test value
i32 total = 0;
struct waitgroup test_sync_wait;
struct mutex protect_total;

void test_sync(void) {
    printf("test_sync...");
    total = 10;
    waitgroup_init(&test_sync_wait, 2);
    mutex_init(&protect_total);

    cx_thread_start("test_sync1", NULL, STACK_SIZE, add, 10);
    cx_thread_start("test_sync2", NULL, STACK_SIZE, add, 20);

    // Wait until other threads done
    waitgroup_wait(&test_sync_wait);



    // To pass, show that total is 40
    if (total != 40) {
        printf("FAILED, value of total is %d\n", total);
    } else {
        printf("OK\n");
    }
}

void add(i32 arg) {

    mutex_lock(&protect_total);
    total += arg;
    mutex_unlock(&protect_total);

    // Tell the test program to go ahead and finish
    waitgroup_done(&test_sync_wait);
}
