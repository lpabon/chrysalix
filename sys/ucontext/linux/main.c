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

/**
 * @file main.c
 *      Sets up Chrysalix OS to run on top of GNU Pth
 */

/************************************************************************************
 * Includes
 */
#include <chrysalix.h>
#include <cx_init.h>
#include "main.h"

/************************************************************************************
 * Packages
 */
#include <helloworld/helloworld.h>
#include <tests/tests.h>

/************************************************************************************
 * Defines
 */

/************************************************************************************
 * Globals
 */
static u8 osheap[HEAPSZ];

/************************************************************************************
 * Functions
 */
int main(void) {
    /*
     * Clear my mem
     */
    myGLOBAL = 0;
    memset((void *) &sem, 0x0, sizeof(struct semaphore));

    /*
     * Initialize OS heap
     */
    cx_heap_init(osheap, HEAPSZ, HEAP_OS);

    /*
     * Initialize Chrysalix
     */
    cx_init();

    /*
     * Register some threads
     */
    //cx_thread_start("cx_test", NULL, STKSZ, main_cx_test, 0);
    cx_thread_start("console", NULL, STKSZ,
                    console_thread_start, llstdout);
    /*
     * Start some package
     */
    helloworld_console_register();
    tests_console_register();

    /*
     * Start OS
     */
    cx_start();

    /*
     * Never reached
     */
    return (123);
}

unsigned long cx_intsoff(void) {
    return 0;
}

void cx_intson(unsigned long s) {
};
