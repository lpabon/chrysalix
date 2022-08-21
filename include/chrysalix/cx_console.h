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
 
#ifndef _CX_CONSOLE_H
#define _CX_CONSOLE_H

/*****************************************************************
 * Defines
 */
#define CONSOLE_STKSZ       1024

    /** Create and register the function list with the console */
#define CX_CONSOLE_CREATE(fncs, flist) \
        do \
        { \
            (flist).fnc_list = (fncs);      \
            (flist).num_fncs = ( sizeof(fncs) / sizeof(struct console_fnc) ); \
            LIST_INIT((flist).fnc_link); \
            cx_console_register( &(flist) );\
        } while ( 0 );


/*****************************************************************
 * Structures
 */
struct console_fnc
{
    const char      *cmd_name;
    i32         (*cmd_fnc)( i32     argc,   char    **argv );
};

struct console_fnc_list
{
    const struct console_fnc   *fnc_list;
    u32                num_fncs;
    struct list               fnc_link;
};


/*****************************************************************
 * Defines
 */
void console_thread_start( i32    uistream );
i32 cx_console_register( struct console_fnc_list *cmds );
i32 cx_console_deregister( struct console_fnc_list *cmds );

#endif /* _CX_CONSOLE_H */
