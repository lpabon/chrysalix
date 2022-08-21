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
#include <cx_init.h>
#include "cx_arch.h"
#include "arch_context.h"
#include "cx_sched.h"

/**
 *      Initialize the kernel and libraries.
 */
i32 cx_init(void) {
    /*
     * Initialize the console before anyone else so that other
     * programs can register their functions
     */
    cx_console_init();
    cx_drv_init();
    cx_mem_init();

    /*
     * Initialize utils
     */
    cx_utils_init();

    /*
     * Initialize the scheduler
     */
    cx_sched_init();
    cx_msg_init();

    /*
     * Load device drivers
     */
    arch_drivers_load();

    /*
     * Initialize test infrastructure
     */
    //cx_test_init();

    /*
     * DEBUG
     */
    CXDEBUG(cx_llprintf(__CX_OS__ " Initialized\n\r"););

    return (0);
}

/**
 *      Start the kernel -- GO!
 */
void cx_start(void) {
    CXDEBUG(cx_llprintf(__CX_OS__ " kernel starting...\n\r"););
    cx_sched_start();

    /* NEVER REACHED */

}
