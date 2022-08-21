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

/************************************************************************************
 * Defines
 */
#define DRV(fdentry)    ( fdlist[ (fdentry ) ].f_driver )

/************************************************************************************
 * Prototypes
 */
static i32 do_drivers(i32 argc _UNUSED_, char **argv _UNUSED_);
static i32 do_fds(i32 argc _UNUSED_, char **argv _UNUSED_);

/************************************************************************************
 * Globals
 */
static struct driver_entry ddlist[DRIVER_MAX];
static struct file_entry fdlist[FD_MAX];
i32 errno;
fd_t stdout;

static const struct console_fnc g_console_fncs[] = {
    { "drivers", do_drivers },
    { "fds", do_fds }
};

static struct console_fnc_list g_console_fnclist;

/************************************************************************************
 * Functions
 */

/* ------------------------------------------------------------ */
i32 cx_drv_init(void) {
    memset((void *) ddlist, 0x0, sizeof(struct driver_entry) * DRIVER_MAX);
    memset((void *) fdlist, 0x0, sizeof(struct file_entry) * FD_MAX);

    /*
     * Register function list
     */
    CX_CONSOLE_CREATE(g_console_fncs, g_console_fnclist);

    return (0);
}


/**
 * This is the device driver initialization table.  It gets loaded when
 * cx_drivers_load() is called at the initialization of the system.
 *
 * @note
 *      The first entry of the table is the Low Level driver.  This driver
 *      is the one used to show errors/info/etc as the system comes up.
 *      If this driver is unable to be loaded, the cx_drivers_load will call
 *      cx_die() to halt the system.
 *
 * @important
 *      Order is important.  Layered drivers are supposed to be initialized
 *      after their lower level drivers.
 */
i32
cx_drivers_load(const struct device_init_table *device_table,
                u32 num_devices) {
    u32 i;
    i32 retval;

    /*
     * Load low level driver
     */
    retval = -1;
    if (NULL != device_table[0].init)
        retval = device_table[0].init(device_table[0].minor);

    /*
     * If device_table[0].init is NULL or device_table[0].init failed, then
     * die here
     */
    if (0 > retval)
        cx_die();

    /*
     * Have it use the first file descriptor.  This way we reserve file
     * descripor zero(0) for the use of the low level driver (llstdout)
     */
    if (0 > cx_open(ddlist[0].d_name, 0, 0))
        cx_die();

    /*
     * Load other drivers
     */
    for (i = 1; i < num_devices; i++) {
        if (NULL != device_table[i].init) {
            retval = device_table[i].init(device_table[i].minor);
            if (0 > retval)
                return (retval);
        }
    }

    return (0);
}

/* ------------------------------------------------------------ */
i32 cx_drv_reg(drv_t minor, const Driver_t * drv_entry) {

    drv_t drv;
    char minor_str[10];
    struct driver_entry *dd = NULL;

    /*
     * Check arguments
     */
    if (NULL == drv_entry) {
        return (-EINVAL);
    }

    /*
     * Allocate a descriptor
     */
    for (drv = 0; drv < DRIVER_MAX; drv++) {
        if (NULL == ddlist[drv].d_tbl) {
            /* Found */
            break;
        }
    }

    /*
     * Check if we were not able to find an empty spot.
     */
    if (DRIVER_MAX == drv) {
        return (-ENOMEM);
    }

    /*
     * Point to allocate descriptor
     */
    dd = &ddlist[drv];

    /*
     * Save minor number
     */
    dd->d_minor = minor;

    /*
     * Concatenate the name
     */
    strncpy(dd->d_name, drv_entry->d_name, ARCH_MAX_DRIVER_NAME);
    dd->d_name[ARCH_MAX_DRIVER_NAME] = '\0';
    sprintf(minor_str, "%d", minor);
    strncat(dd->d_name, minor_str, ARCH_MAX_DRVNAME_MINORNUMBER);

    /*
     * Save driver functions
     */
    dd->d_tbl = drv_entry;

    /*
     * Print out
     */
    CXDEBUG(if (llstdout < drv) {
            cx_llprintf("%s ok\n\r", dd->d_name);}
    );

    return (0);
}

/* ------------------------------------------------------------ */
i32 cx_drv_state_set(fd_t fd, void *state) {
    ASSERT((0 <= fd) && (fd < FD_MAX) && (NULL != state));
    fdlist[fd].f_data = state;
    return (0);
}

/* ------------------------------------------------------------ */
void *cx_drv_state_get(fd_t fd) {
    ASSERT((0 <= fd) && (fd < FD_MAX));
    return (fdlist[fd].f_data);
}

/* ------------------------------------------------------------ */
drv_t cx_drv_minor(fd_t fd) {
    ASSERT((0 <= fd) && (fd < FD_MAX));
    return (DRV(fd)->d_minor);
}

/* ------------------------------------------------------------ */
struct driver_entry *cx_drv_get_driverentry(fd_t fd) {
    ASSERT((0 <= fd) && (fd < FD_MAX));
    return (DRV(fd));
}

/* ------------------------------------------------------------ */
struct file_entry *cx_drv_get_fileentry(fd_t fd) {
    ASSERT((0 <= fd) && (fd < FD_MAX));
    return (&fdlist[fd]);
}

/* ------------------------------------------------------------ */
fd_t cx_open(const char *path, i32 flags, i32 mode) {
    i32 dev;
    fd_t fd;
    i32 retval;

    /*
     * Check arguments
     */
    if (NULL == path)
        return (-EINVAL);

    /*
     * Look for an open file descriptor
     */
    for (fd = 0; fd < FD_MAX; fd++) {
        if (NULL == DRV(fd)) {
            break;
        }
    }

    /*
     * Check if we found a file descriptor
     */
    if (FD_MAX == fd)
        return (-ENFILE);

    /*
     * Find driver
     */
    for (dev = 0; dev < DRIVER_MAX; dev++) {
        if (0 == strncmp(path, ddlist[dev].d_name, DRVNAME_MAX)) {
            break;
        }
    }

    /*
     * Driver not found
     */
    if (DRIVER_MAX == dev)
        return (-ENODEV);

    /*
     * Save the driver in the descriptor
     */
    DRV(fd) = &ddlist[dev];

    /*
     * Call driver open call
     */
    if (NULL != ddlist[dev].d_tbl->d_open) {
        retval = ddlist[dev].d_tbl->d_open(fd, flags, mode);
        if (0 > retval) {
            /*
             * Deallocate the file descriptor
             */
            memset((void *) &fdlist[fd], 0x0, sizeof(struct file_entry));

            /*
             * Return open failure to caller
             */
            return (retval);
        }
    }

    /*
     * Return file descriptor
     */
    return (fd);

}


/* ------------------------------------------------------------ */
i32 cx_writetm(fd_t fd, void *buf, i32 size, i32 timeout) {
    ASSERT((0 <= fd) && (fd < FD_MAX));
    ASSERT(NULL != DRV(fd)->d_tbl);

    if (NULL != DRV(fd)->d_tbl->d_writetm)
        return (DRV(fd)->d_tbl->d_writetm(fd, buf, size, timeout));
    else {
        errno = ENOTSUP;
        return (-1);
    }

}

/* ------------------------------------------------------------ */
i32 cx_readtm(fd_t fd, void *buf, i32 size, i32 timeout) {
    ASSERT((0 <= fd) && (fd < FD_MAX));
    ASSERT(NULL != DRV(fd)->d_tbl);

    if (NULL != DRV(fd)->d_tbl->d_readtm)
        return (DRV(fd)->d_tbl->d_readtm(fd, buf, size, timeout));
    else {
        errno = ENOTSUP;
        return (-1);
    }

}

/* ------------------------------------------------------------ */
i32 cx_ioctl(fd_t fd, i32 func, void *buf) {
    ASSERT((0 <= fd) && (fd < FD_MAX));
    ASSERT(NULL != DRV(fd)->d_tbl);

    if (NULL != DRV(fd)->d_tbl->d_ioctl)
        return (DRV(fd)->d_tbl->d_ioctl(fd, func, buf));
    else {
        errno = ENOTSUP;
        return (-1);
    }
}

/* ------------------------------------------------------------ */
i32 cx_close(fd_t fd) {
    i32 retval;

    ASSERT((0 <= fd) && (fd < FD_MAX));
    ASSERT(NULL != DRV(fd)->d_tbl);

    if (NULL != DRV(fd)->d_tbl->d_close) {
        retval = DRV(fd)->d_tbl->d_close(fd);
        if (0 > retval)
            return (retval);
    }

    /*
     * Free fd
     */
    memset((void *) &fdlist[fd], 0x0, sizeof(struct file_entry));

    return (0);
}

/* ------------------------------------------------------------ */
i32 cx_lseek(fd_t fd, i32 offset, i32 whence) {
    return (-ENOTSUP);
}

/* ------------------------------------------------------------ */
static i32 do_drivers(i32 argc _UNUSED_, char **argv _UNUSED_) {
    drv_t drv;

    /*
     * Print header
     */
    printf("DRV# Name Table\n" "============\n");
    /*
     * Print Driver
     */
    for (drv = 0; drv < DRIVER_MAX; drv++) {
        if (NULL != ddlist[drv].d_tbl) {
            printf("%d %s 0x%X\n",
                   drv, ddlist[drv].d_name, ddlist[drv].d_tbl);
        }
    }

    return (0);
}

static i32 do_fds(i32 argc _UNUSED_, char **argv _UNUSED_) {
    drv_t fd;

    /*
     * Print header
     */
    printf("FD# Name Data\n" "============\n");
    /*
     * Print Driver
     */
    for (fd = 0; fd < FD_MAX; fd++) {
        if (NULL != fdlist[fd].f_driver) {
            printf("%d %s 0x%X\n", fd, DRV(fd)->d_name, fdlist[fd].f_data);
        }
    }
    return (0);
}
