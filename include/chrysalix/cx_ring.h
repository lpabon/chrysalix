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
 
#ifndef _CX_RING_H
#define _CX_RING_H

enum ring_state
{
     CX_RING_STATE_WRITE = 0,
     CX_RING_STATE_PEND  = 1

};

/**
 * This structure defines the circular buffer structure, useless comment
 */
struct ring
{
    u32          writer;
    u32          reader;
    u8          *ring_buf;
    u32          ring_buf_size;
    enum ring_state ring_buf_state;
};


/**************** API **************************/
/* Lock functions */
#define cx_ring_full(ring)                 ( (CX_RING_STATE_PEND == (ring)->ring_buf_state) && ((ring)->reader == (ring)->writer) )
#define cx_ring_empty(ring)                ( (CX_RING_STATE_WRITE == (ring)->ring_buf_state) && ((ring)->reader == (ring)->writer) )
#define cx_ring_restore(ring, amount)      { \
            if ( (ring)->reader < (amount) ) \
                (ring)->reader = (ring)->ring_buf_size - ( (amount) - (ring)->reader ); \
            else \
                (ring)->reader -= (amount); \
            }
#define cx_ring_delete(ring, amount)       { \
            (ring)->reader += (amount); \
            if ( (ring)->reader >= (ring)->ring_buf_size )\
                (ring)->reader -= (ring)->ring_buf_size;\
            }

i32
cx_ring_init( struct ring *ring, u8 *buf, u32 buflen);

i32
cx_ring_read( struct ring *ring, u8 *buf,  u32 buflen);

i32
cx_ring_write( struct ring *ring, u8 *buf, u32 buflen);


#endif /* _CX_RING_H */

