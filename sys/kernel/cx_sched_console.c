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
#include "arch_context.h"
#include "cx_sched.h"

/************************************************************************************
 * Defines
 */
#define THREAD_GETID()          ( (i32)((PCB_t *)current_pcb - (PCB_t *)pcblist) )

/************************************************************************************
 * Prototypes
 */
static i32 do_ps(i32 argc, char **argv);
static i32 do_kill(i32 argc, char **argv);
static i32 do_pdump(i32 argc, char **argv);
static i32 do_sigtest(i32 argc, char **argv);

/************************************************************************************
 * Globals
 */

static const struct console_fnc g_console_sched_fncs[] = {
    { "ps", do_ps },
    { "top", do_ps },
    { "kill", do_kill },
    { "pdump", do_pdump },
    { "sigtest", do_sigtest },
};

static struct console_fnc_list g_console_sched_fnclist;

/************************************************************************************
 * Functions
 */

void cx_sched_console_init(void) {
    /*
     * Register console
     */
    CX_CONSOLE_CREATE(g_console_sched_fncs, g_console_sched_fnclist);
}

static i32 do_ps(i32 argc _UNUSED_, char **argv _UNUSED_) {
    i32 pid;
    PCB_t *pcb;

    printf("PID NAME STATE\n");
    for (pid = 0; pid < ARCH_MAX_THREADS; pid++) {
        pcb = cx_get_pcb(pid);
        if (!CX_SCHED_IS_PCB_DEAD(pcb)) {
            printf("%d %s ", pid, pcb->th_name);
            if (TH_RUNNING == (pcb->th_state & TH_RUNNING))
                printf("%c", 'R');
            if (TH_SLEEPING == (pcb->th_state & TH_SLEEPING))
                printf("%c", 'S');
            if (TH_SUSPENDED == (pcb->th_state & TH_SUSPENDED))
                printf("%c", 'W');
            if (TH_SEMWAIT == (pcb->th_state & TH_SEMWAIT))
                printf("%c", 'M');
            if (TH_HALTED == (pcb->th_state & TH_HALTED))
                printf("%c", 'H');
            printf("\n");
        }
    }
    return (0);
}


static i32 do_kill(i32 argc, char **argv) {
    u32 sig;
    i32 pid;

    if (argc < 3) {
        printf("kill [-C|D|W|I] <pid pid...>\n");
        return (-1);
    }

    if ('-' == argv[1][0]) {
        if ('I' == argv[1][1])
            sig = SIGINT;
        else if ('C' == argv[1][1])
            sig = SIGCONT;
        else if ('D' == argv[1][1])
            sig = SIGTERM;
        else if ('W' == argv[1][1])
            sig = SIGSTOP;
        else {
            printf("Unknown switch: %s\n", argv[1]);
            return (-1);
        }

        argv++;
        argc--;
    } else {
        printf("kill [-C|D|W|I] <pid pid...>\n");
        return (-1);

    }

    for (; argc > 1; argc--, argv++) {
        pid = atoi(argv[1]);
        printf("Sent signal %u to pid %u. Result = %d\n",
               sig, pid, cx_kill(pid, sig));
    }

    return (0);
}

static i32 do_pdump(i32 argc, char **argv) {
    i32 pid;
    PCB_t *pcb;
    i32 sval = 0;

    if ((argc < 2) || ('-' == argv[1][0])) {
        printf("pdump <pid>\n");
        return (-1);
    }

    pid = atoi(argv[1]);
    pcb = cx_get_pcb(pid);
    if (NULL != pcb) {
        printf("Pdump %s - PID %u\n\n", pcb->th_name, pid);
        printf("&Ctx = 0x%X size %u\n", (u32) & pcb->ctx,
               sizeof(struct context));
        arch_context_print(&pcb->ctx);

        printf("\n"
               "Fnc = 0x%X ( arg:%d )\n",
               (u32) pcb->entry_info.fnc, pcb->entry_info.arg);
        printf("Stk = 0x%X size:%u\n",
               pcb->stack_info.stack, pcb->stack_info.stack_size);
        printf("Attr = 0x%X\n", pcb->th_attr);
        printf("NTR  = %u\n", pcb->num_times_run);
        printf("Wait Value = %u\n", pcb->wait_val);
        printf("UI = %u\n", pcb->uistream);

        (void) sem_getvalue(&pcb->th_port.sem_send, &sval);
        printf("Msgs = %d\n", sval * (-1) + 1);

        printf("State = ");
        if (CX_SCHED_IS_PCB_DEAD(pcb))
            printf("Dead\n");
        else {
            if (TH_RUNNING == (pcb->th_state & TH_RUNNING))
                printf("%c", 'R');
            if (TH_SLEEPING == (pcb->th_state & TH_SLEEPING))
                printf("%c", 'S');
            if (TH_SUSPENDED == (pcb->th_state & TH_SUSPENDED))
                printf("%c", 'W');
            if (TH_SEMWAIT == (pcb->th_state & TH_SEMWAIT))
                printf("%c", 'M');
            if (TH_HALTED == (pcb->th_state & TH_HALTED))
                printf("%c", 'H');
            printf("\n");
        }
    } else {
        printf("PID %u not found\n", pid);
    }

    return (0);
}

static i32 do_sigtest(i32 argc, char **argv) {

    i32 sig;
    i32 pid;

    if (argc < 3) {
        printf("%s <pid> <signal>\n", argv[0]);
        return (-1);
    }

    pid = atoi(argv[1]);
    sig = atoi(argv[2]);

    cx_event_send(pid, sig);
    return (0);

}
