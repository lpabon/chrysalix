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
 
#ifndef _CX_ARCH_H
#  define _CX_ARCH_H

/*****************************************************************
 * Defines
 */
    /** Maximum thread name */
#define ARCH_MAX_THREAD_NAME     8

    /** Maximum number of threads allowed */
#define ARCH_MAX_THREADS         64

    /** Console command line size */
#define ARCH_CMDLINE_SIZE   128

    /** Minimum stack for this architecture */
#define ARCH_MIN_STACK_SIZE         (16*1024)

    /** 
     * Maximum number of global file descriptors allowed in 
     * the system
     */
#define ARCH_MAX_DESCRIPTORS     64

    /** 
     * Maximum number of device drivers allowed in 
     * the system
     */
#define ARCH_MAX_DRIVERS     10

    /**
     * Maximum size for the device driver name
     */
#define ARCH_MAX_DRIVER_NAME            3

    /** 
     * Maximum size for the number of characters used to represent the minor 
     * number in the driver name 
     */
#define ARCH_MAX_DRVNAME_MINORNUMBER        2

/*****************************************************************
 * Structures
 */

enum heap_type
 {
    HEAP_OS,        /* <-- MUST HAVE! */
        
    /* Number of heaps in the system */
    HEAP_NUM
 };

#endif /* _CX_ARCH_H */
