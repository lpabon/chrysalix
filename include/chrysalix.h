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
 
#ifndef _CHRYSALIX_H
#define _CHRYSALIX_H


/*
 * Debug commands
 */
#ifdef DEBUG
#    define CXDEBUG(debugcode)      do { debugcode } while(0)
#    define ASSERT( condition ) \
        do { \
            if (!(condition)) \
            {\
                cx_panic("ASSERT: " #condition );\
            }\
        } while (0)

#else /* DEBUG */

#   define CXDEBUG(test)            do{} while (0)
#   define ASSERT(condition)        do{} while (0)

#endif /* DEBUG */

/*
 * Define NULL if it not defined yet
 */
#  ifndef NULL
#    define NULL    0
#  endif

/*
 * Define GNU GCC attribute
 */
#  ifndef _UNUSED_
#    define _UNUSED_ __attribute__ ((unused))
#  endif
       /* !_UNUSED_ */

    /*
     * For UNIT8/u16..etc 
     */
#  include <stdarg.h>
#  include <arch_types.h>
#  include <arch_lib.h>

    /*
     * CX 
     */
#  include <cx_arch.h>
#  include <chrysalix/queue.h>
#  include <chrysalix/list.h>
#  include <chrysalix/cx_err.h>
#  include <chrysalix/cx.h>
#  include <chrysalix/cx_isr.h>
#  include <chrysalix/cx_utils.h>
#  include <chrysalix/cx_msg.h>
#  include <chrysalix/cx_io.h>
#  include <chrysalix/cx_drv.h>
#  include <chrysalix/cx_stdio.h>
#  include <chrysalix/cx_string.h>
#  include <chrysalix/cx_sync.h>
#  include <chrysalix/cx_event.h>
#  include <chrysalix/cx_console.h>
#  include <chrysalix/cx_mem.h>
#  include <chrysalix/cx_signal.h>
#  include <chrysalix/cx_proc.h>
#  include <chrysalix/cx_time.h>
#  include <chrysalix/cx_ring.h>

/*****************************************************************
 * Compatible Macros
 */

/*****************************************************************
 * Globals
 */
extern i32       errno;


#endif /* _CHRYSALIX_H */
