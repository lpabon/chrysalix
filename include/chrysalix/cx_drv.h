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
 
#ifndef _CX_DRV_H
#define _CX_DRV_H

/*****************************************************************
 * Defines
 */
    /** Maximum device driver name */
#define DRVNAME_MAX         (ARCH_MAX_DRIVER_NAME + ARCH_MAX_DRVNAME_MINORNUMBER)
    /** Maximum number of file descriptors allowed */
#define FD_MAX              (ARCH_MAX_DESCRIPTORS)
    /** Maximum registered drivers allowed */ 
#define DRIVER_MAX          (ARCH_MAX_DRIVERS)

    /** Low-level stdout file descriptor */
#define LLSTDOUT_FILENO     (fd_t)0
#define LLSTDIN_FILENO      LLSTDOUT_FILENO
#define LLSTDERR_FILENO     LLSTDOUT_FILENO
#define llstdout            LLSTDOUT_FILENO
#define llstdin             LLSTDIN_FILENO
#define llstderr            LLSTDERR_FILENO

    /** Standard out */
#define stdout          cx_get_uistream()
#define stdin           stdout
#define stderr          stdout
#define STDOUT_FILENO   stdout
#define STDIN_FILENO    stdin
#define STDERR_FILENO   stderr

/**
 * Driver entry
 */
typedef struct
{
    const char *d_name;
    i32(*d_open)(fd_t dev, u32 flags, u32 mode);
    i32(*d_close)(fd_t dev);
    i32(*d_writetm) (fd_t dev, void *buf, i32 size, i32 timeout);
    i32(*d_readtm) (fd_t dev, void *buf, i32 size, i32 timeout);
    i32(*d_ioctl) (fd_t dev, i32 func, void *buf);

} Driver_t;


/**
 * Driver table entries
 */
struct device_init_table
{
    drv_t      minor;
    i32 (*init)( drv_t  minor );

};

/**
 * Driver information. 
 */
struct driver_entry
{
    char                d_name[ DRVNAME_MAX + 1];
    drv_t               d_minor;
    const Driver_t  *d_tbl;
};

/**
 *  Used for the global file descriptors
 */
struct file_entry
{
    void               *f_data;
    struct driver_entry *f_driver;
};
    

/*****************************************************************
 * Prototypes
 */
i32 cx_drivers_load( const struct device_init_table *device_table, u32 num_devices );
i32 cx_drv_reg(drv_t minor, const Driver_t *drv_entry);
i32 cx_drv_state_set(fd_t   dev, void    *state);
void  *cx_drv_state_get(fd_t   dev);
drv_t cx_drv_minor(fd_t   dev);
struct file_entry *cx_drv_get_fileentry( fd_t fd );
struct driver_entry *cx_drv_get_driverentry( fd_t fd );

#endif /* _CX_DRV_H */
