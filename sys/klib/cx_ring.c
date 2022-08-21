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

#include <chrysalix.h>

#define  RING_END(ring)          ((ring)->ring_buf_size)

/**
 * Initialize the memory
 */
i32 cx_ring_init(struct ring *ring, u8 * buf, u32 buflen) {
    if ((NULL == ring) || (NULL == buf) || (0 == buflen)) {
        return (-EINVAL);
    }

    ring->reader = 0;
    ring->writer = 0;
    ring->ring_buf = buf;
    ring->ring_buf_size = buflen;
    ring->ring_buf_state = CX_RING_STATE_WRITE;

    memset((void *) ring->ring_buf, 0x0, ring->ring_buf_size);

    return (0);
}

/**
 * Write into the circular buffer
 *
 * @param ring
 *      Circular buffer structure pointer
 * @param buf
 *      Buffer to copy to circular buffer
 * @param buflen
 *      Number of bytes to copy from buf to ring
 *
 * @return
 *      Number of bytes written, or
 *      -1 for failure.
 */
i32 cx_ring_write(struct ring *ring, u8 * buf, u32 buflen) {

    i32 ret;
    u32 end;
    u32 s = cx_intsoff();

    /*
     * Check variables
     */
    if ((NULL == ring) || (NULL == buf)) {
        cx_intson(s);
        return (-1);
    }

    /*
     * Get end
     */
    if (cx_ring_empty(ring) || (ring->reader < ring->writer)) {
        end = RING_END(ring);
    } else if (cx_ring_full(ring)) {
        cx_intson(s);
        return (0);
    } else {
        end = ring->reader;
    }

    /*
     * Check if we have enough space to write to the buffer.
     */
    if ((ring->writer + buflen) <= end) {
        memcpy(&ring->ring_buf[ring->writer], buf, buflen);
        ring->writer += buflen;
        if (ring->writer >= RING_END(ring)) {
            ring->writer = 0;
        }

        ret = buflen;
    } else {
        /*
         * We do not have enough space, let's write to the end,
         * then wrap around
         */
        ret = cx_ring_write(ring, buf, end - ring->writer);
        if (0 > ret) {
            cx_intson(s);
            return (ret);
        }
        /*
         * Now write the rest
         */
        ret += cx_ring_write(ring, &buf[ret], buflen - ret);

    }

    /*
     * Change state in case we catch up with the reader
     */
    ring->ring_buf_state = CX_RING_STATE_PEND;

    /*
     * Return
     */
    cx_intson(s);
    return (ret);
}

/**
 * Write into the circular buffer
 *
 * @param cb
 *      Circular buffer structure pointer
 * @param buf
 *      Buffer to copy from circular buffer
 * @param buflen
 *      Number of bytes to read from cb and copy to cb
 *
 * @return
 *      Number of bytes read, or
 *      -1 for failure.
 */
i32 cx_ring_read(struct ring *ring, u8 * buf, u32 buflen) {

    i32 ret;
    u32 end;
    u32 s = cx_intsoff();

    /*
     * Check variables
     */
    if ((NULL == ring) || (NULL == buf)) {
        cx_intson(s);
        return (-EINVAL);
    }

    /*
     * Check if we have no bytes to read.  If there
     * are bytes to read, check the end.
     *
     *         R
     * B-------W-----E
     *
     *
     * B---R---W-----E
     *
     * B---W---R-----E
     *
     *
     */
    if (cx_ring_full(ring)) {
        end = RING_END(ring);
    } else if (cx_ring_empty(ring)) {
        cx_intson(s);
        return (0);
    } else if (ring->reader < ring->writer) {
        end = ring->writer;
    } else {
        end = RING_END(ring);
    }

    /*
     * Check if we have enough space to write to the buffer.
     */
    if ((ring->reader + buflen) <= end) {
        memcpy(buf, &ring->ring_buf[ring->reader], buflen);
        ring->reader += buflen;
        if (ring->reader >= RING_END(ring)) {
            ring->reader = 0;
        }

        ret = buflen;
    } else {
        /*
         * We do not have enough space, let's read to the end,
         * then wrap around
         */
        ret = cx_ring_read(ring, buf, end - ring->reader);
        if (0 > ret) {
            cx_intson(s);
            return (ret);
        }
        /*
         * Now read the rest
         */
        ret += cx_ring_read(ring, &buf[ret], buflen - ret);

    }

    /*
     * Change the state in case we catch up with the writer
     */
    ring->ring_buf_state = CX_RING_STATE_WRITE;

    /*
     * Return
     */
    cx_intson(s);
    return (ret);

}
