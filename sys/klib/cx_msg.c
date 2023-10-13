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
#include "cx_arch.h"
#include "arch_context.h"
#include "cx_sched.h"

/************************************************************************************
 * Prototypes
 */
static i32 do_msg(i32 argc, char **argv);
static void msg_print_server(i32 arg);
/************************************************************************************
 * Globals
 */
static i32 msgprintpid;
static const struct console_fnc g_console_fncs[] = {
    { "msg", do_msg }
};

static struct console_fnc_list g_console_fnclist;

i32 cx_msg_init(void) {
    /*
     * Register function list
     */
    CX_CONSOLE_CREATE(g_console_fncs, g_console_fnclist);

    msgprintpid =
        cx_thread_start("msgprint", NULL, 512, msg_print_server, 0);

    return (0);
}

i32 cx_msg_send(ND_t nd, struct msg *msg, _UNUSED_ u32 flags, _UNUSED_ i32 tm) {
    PCB_t *srv_pcb;

    /*
     * Check if the message is to ourselves
     */
    msg->msg_source = cx_getpid();
    if ((i32) nd == msg->msg_source)
        return (-EFAULT);

    /*
     * Get PCB of the server
     */
    srv_pcb = cx_get_pcb((i32) nd);
    if (NULL == srv_pcb)
        return (-ESRCH);

    /*
     * Check if destination is alive
     */
    if (CX_SCHED_IS_PCB_DEAD(srv_pcb))
        return (-EHOSTDOWN);

    /*
     * Take semaphore
     */
    sem_wait(&srv_pcb->th_port.sem_send);

    /*
     * Place msg pointer in server's PCB
     */
    srv_pcb->th_port.sent_msg = msg;

    /*
     * Wake up server
     */
    sem_post(&srv_pcb->th_port.sem_recv);

    /*
     * Wait for response
     */
    cx_thread_set_state_pcb(cx_get_current_pcb(), TH_MSG_REPLY);
    cx_yield();

    /*
     * Return to client
     */
    return (0);
}

i32 cx_msg_recv(_UNUSED_ ND_t nd, struct msg *msg, _UNUSED_ u32 flags, _UNUSED_ i32 tm) {
    PCB_t *current_pcb;

    /*
     * Get current pcb
     */
    current_pcb = cx_get_current_pcb();

    /*
     * Wait for a message
     */
    sem_wait(&current_pcb->th_port.sem_recv);

    /*
     * Message available, give to the server
     */
    memcpy((void *) msg, (void *) current_pcb->th_port.sent_msg,
           sizeof(struct msg));

    /*
     * Return
     */
    return (0);

}


i32 cx_msg_reply(_UNUSED_ ND_t nd, struct msg *msg, _UNUSED_ u32 flags, _UNUSED_ i32 tm) {
    PCB_t *current_pcb;

    /*
     * Initialize variables
     */
    current_pcb = cx_get_current_pcb();

    /*
     * Place client into running state.  Data is already in the message
     * which they had sent.
     */
    cx_thread_set_state((i32) msg->msg_source, TH_RUNNING);

    /*
     * Update the send semaphore so others can place messages there
     */
    sem_post(&current_pcb->th_port.sem_send);

    /*
     * Yield here to cooperate with other servers
     */
    cx_yield();

    /*
     * Return to caller
     */
    return (0);
}


static i32 do_msg(i32 argc, char **argv) {
    struct msg msg;

    printf(" - - Msg test [WORKER] - - \n");
    for (argc--; argc >= 0; argc--) {
        msg.msg_source = cx_getpid();
        msg.msg = (void *) argv[argc];
        msg.msg_tag = 1;
        cx_msg_send(msgprintpid, &msg, 0, 0);
    }


    return (0);
}

static void msg_print_server(_UNUSED_ i32 arg) {
    struct msg msg;
    i32 pid;

    pid = cx_getpid();
    while (1) {
        if (0 <= cx_msg_recv(pid, &msg, 0, -1)) {
            printf("..%s..\n", (char *) msg.msg);
            cx_msg_reply(msg.msg_source, &msg, 0, 0);
        }

    }

}
