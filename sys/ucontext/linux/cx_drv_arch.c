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
 * @file cx_drv_arch.c
 *      GNU Pth drivers port
 */

/************************************************************************************
 * Includes
 */
#include <chrysalix.h>

/************************************************************************************
 * Driver Includes
 */
#include <drivers/cx_tty_drv.h>

/************************************************************************************
 * Defines
 */
    /** Number of entries in the driver table */
#define NUM_DEV_TABLE_ENTRIES       ( sizeof(g_drv_table) / sizeof(struct device_init_table) )

/************************************************************************************
 * Device Table
 */

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
static const struct device_init_table g_drv_table[] = {

/*  { minor,   fnc  }, */
    { 0, tty_init },            /* Low Level Driver Entry! */
};

/* ------------------------------------------------------------ */
i32 arch_drivers_load(void) {
    return (cx_drivers_load(g_drv_table, NUM_DEV_TABLE_ENTRIES));
}
