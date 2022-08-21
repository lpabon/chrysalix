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
#include <drivers/cx_tty_drv.h>
#include "linux_console.h"

/************************************************************************************
 * Prototypes
 */
static i32 tty_read(fd_t fd, void *buf, i32 size, i32 timeout);
static i32 tty_write(fd_t fd, void *buf, i32 size, i32 timeout);

/*******************************************************
 * Globals
 */
static const Driver_t tty_drv_entry = {
    .d_name = "tty",
    .d_writetm = tty_write,
    .d_readtm = tty_read
};

/**
 * Initializes debug port
 *
 * @param dev
 *    Device to initialize - debug port
 *
 * @return
 *    0 for success, or -ENODEV when device requested does not exist
 */
i32 tty_init(drv_t minor) {

    /*
     * Make stdin nonblock
     */
    lc_init();

    /*
     * Register driver
     */
    cx_drv_reg(minor, &tty_drv_entry);
    return (0);
}

/**
 * Reads from standard in
 *
 * @param fd
 *    Device to write to - debug port
 * @param buf
 *    Buffer to copy to debug port
 * @param buflen
 *    Number of bytes to copy from buf to debug port
 * @param timeout
 *    Amount of time to wait before exiting function
 *
 * @return
 *   Number of bytes written, or
 *      -1 for failure.
 */
static i32 tty_read(fd_t fd, void *buf, i32 size, i32 timeout) {
    return (lc_read(buf, size));
}

/**
 * Writes to standard out
 *
 * @param fd
 *    Device to write to - debug port
 * @param buf
 *    Buffer to copy to debug port
 * @param buflen
 *    Number of bytes to copy from buf to debug port
 * @param timeout
 *    Amount of time to wait before exiting function
 *
 * @return
 *   Number of bytes written, or
 *      -1 for failure.
 */
static i32 tty_write(fd_t fd, void *buf, i32 size, i32 timeout) {
    return (lc_write(buf, size));
}
