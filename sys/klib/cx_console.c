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

/*****************************************************************
 * Includes
 */
#include <chrysalix.h>

/*****************************************************************
 * Defines
 */
#define TITLE       "Welcome to ChrysalixOS "  CX_VERSION " (" __ARCH__ ")\n    compiled " __DATE__" " __TIME__
#define PROMPT      "\n[chrysalix] # "

/*****************************************************************
 * Prototypes
 */
static i32 do_version(i32 argc, char **argv);
static i32 do_help(i32 argc, char **argv);
static i32 do_cmdtest(i32 argc, char **argv);
static i32 console_run_cmd(i32 argc, char **argv);
static i32 console_scan_commandline(i32 * argc, char **argv);

/*****************************************************************
 * Globals
 */
    /** Linked list of command line programs */
static struct list fnc_list;
static char commandline[ARCH_CMDLINE_SIZE];

    /** ARGV and ARGC for programs to call */
static char *argv[ARCH_CMDLINE_SIZE / 2];
static i32 argc;

static const struct console_fnc g_console_fncs[] = {
    { "version", do_version },
    { "cmdtest", do_cmdtest },
    { "ls", do_help },
    { "help", do_help }
};

static struct console_fnc_list g_console_fnclist;

/*****************************************************************
 * Functions
 */

/* -------------------------------------- */
i32 cx_console_register(struct console_fnc_list *cmds) {
    list_insert(&fnc_list, &cmds->fnc_link);
    return (0);
}

/* -------------------------------------- */
i32 cx_console_deregister(struct console_fnc_list *cmds) {
    list_remove(&cmds->fnc_link);
    return (0);
}

/* -------------------------------------- */
i32 cx_console_init(void) {
    /*
     * Initialize the linked list
     */
    list_init(&fnc_list);

    /*
     * Register function list
     */
    CX_CONSOLE_CREATE(g_console_fncs, g_console_fnclist);

    return (0);

}

/*****************************************************************
 * Private Functions
 */

static i32 console_run_cmd(i32 argc, char **argv) {
    u32 i;
    struct console_fnc_list *console_fncs;
    struct list *node;

    /*
     * Search every registered function list
     */
    for (node = list_next(&fnc_list);
         node != &fnc_list; node = list_next(node)) {
        /*
         * In each function list search each of the names
         */
        console_fncs = list_entry(node, struct console_fnc_list, fnc_link);
        for (i = 0; i < console_fncs->num_fncs; i++) {
            /*
             * Check if this is the command the user is asking for
             */
            if (0 ==
                strncmp(argv[0], console_fncs->fnc_list[i].cmd_name, 16)) {
                /*
                 * Execute command
                 */
                return (console_fncs->fnc_list[i].cmd_fnc(argc, argv));
            }
        }
    }

    /*
     * If we are here, then we were unable to find
     * the program the user is looking for
     */
    printf("%s: Command not found\n", argv[0]);

    return (-1);
}

/* -------------------------------------- */
static i32 console_scan_commandline(i32 * argc, char **argv) {
    char *c;

    /*
     * Initialize argc
     */
    *argc = 0;

    /*
     * argv[0] points to the beginning of the command line
     */
    argv[(*argc)++] = commandline;

    /*
     * Go through the command line until we reach either
     * a newline or a NULL character
     */
    for (c = commandline; (*c != '\n') && (*c != '\0'); c++) {
        /*
         * If we see a space, then add the next
         * position to argv[] and change the 'space'
         * character to a NULL character.
         */
        if (' ' == *c) {
            while (' ' == *c)
                *(c++) = '\0';
            if ('\0' != *c)
                argv[(*argc)++] = c;
        }
    }

    return (0);
}

/* -------------------------------------- */
static i32 do_cmdtest(i32 argc, char **argv) {
    i32 i;

    for (i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    return (0);
}

/* -------------------------------------- */
static i32 do_version(i32 argc _UNUSED_, char **argv _UNUSED_) {
    printf("\n" TITLE "\n\n");
    return (0);
}

/* -------------------------------------- */
static i32 do_help(i32 argc _UNUSED_, char **argv _UNUSED_) {
    u32 i;
    struct console_fnc_list *console_fncs;
    struct list *node;

    /*
     * For each of the function lists, show all
     * functions.
     */
    for (node = list_next(&fnc_list);
         node != &fnc_list; node = list_next(node)) {
        console_fncs = list_entry(node, struct console_fnc_list, fnc_link);
        for (i = 0; i < console_fncs->num_fncs; i++) {
            printf("%s ", console_fncs->fnc_list[i].cmd_name);
            if (7 == i % 8)
                printf("\n");
        }
        printf("\n");
    }

    return (0);
}

/* -------------------------------------- */
void console_thread_start(i32 uistream) {
    i32 retval;
    char *buf;
    u32 bufsize;

    /*
     * Initialize values
     */
    buf = commandline;
    bufsize = ARCH_CMDLINE_SIZE;
    if (0 > cx_set_uistream((fd_t) uistream))
        return;

    /*
     * Print title screen and prompt
     */
    do_version(0, argv);
    printf(PROMPT);

    /*
     * Wait for input
     */
    while (1) {
        cx_yield();
        retval = cx_readtm((fd_t) uistream, (void *) buf, bufsize, 0);
        if ('\n' == buf[retval - 1]) {
            /*
             * Take out the the new line
             */
            buf[retval - 1] = '\0';

            /*
             * Do something now that we got the command line
             *
             * Get the argv and argc
             *
             * Call the command handler
             */
            if ('\0' != commandline[0]) {
                console_scan_commandline(&argc, argv);
                console_run_cmd(argc, argv);
            }

            /*
             * Reset
             */
            buf = commandline;
            bufsize = ARCH_CMDLINE_SIZE;
            printf(PROMPT);
        } else if (0 < retval) {
            /*
             * Adjust pointer, since the user has not pressed <ENTER> yet
             */
            buf += retval;
            bufsize -= retval;
        }
    }
}
