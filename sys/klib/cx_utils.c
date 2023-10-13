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

/*****************************************************************
 * Prototypes
 */
static i32 do_peek(i32 argc, char **argv);
static i32 do_poke(i32 argc, char **argv);
static i32 do_reset(i32 argc, char **argv);

/*****************************************************************
 * Globals
 */
static const struct console_fnc g_console_fncs[] = {
    { "peek", do_peek },
    { "poke", do_poke },
    { "reset", do_reset }

};

static struct console_fnc_list g_console_fnclist;

/*****************************************************************
 * Functions
 */

/* -------------------------------------- */
i32 cx_utils_init(void) {
    /*
     * Register function list
     */
    CX_CONSOLE_CREATE(g_console_fncs, g_console_fnclist);

    return (0);

}

/* -------------------------------------- */
void hexprint(unsigned char *b, int len) {
    int i;
    unsigned char *p;

    printf("\n0:");
    for (i = 0, p = b; i < len; i++, p++) {
        printf(" %2X", *p);
        if ((i % 4) == 3)
            printf("h   ");
        if ((i % 16) == 7)
            printf("       ");
        if ((i % 16) == 15)
            printf("\n%d:", i + 1);
    }
    printf("\n");
}

/*****************************************************************
 * Private Functions
 */


/* -------------------------------------- */
static i32 do_peek(i32 argc, char **argv) {
    u8 *address;
    i32 size;

    if (argc < 3) {
        printf("%s <address in hex> <size>\n", argv[0]);
        return (-1);
    }

    address = (u8 *) ((uintptr_t)atoi(argv[1]));
    size = atoi(argv[2]);

    printf("Memory at 0x%X size of %d\n", (uintptr_t) address, size);
    hexprint(address, size);

    return (0);
}

/* -------------------------------------- */
static i32 do_reset(i32 argc _UNUSED_, char **argv _UNUSED_) {
    arch_reset();
    return (0);
}

/* -------------------------------------- */
static i32 do_poke(i32 argc, char **argv) {
    u32 *address;
    u32 value;

    if (argc < 3) {
        printf("%s <address in hex> <value>\n", argv[0]);
        printf("  - 4 bytes at a time\n");
        return (-1);
    }

    address = (u32 *) ((uintptr_t)atoi(argv[1]));
    value = atoi(argv[2]);

    *address = value;

    return (0);

}
