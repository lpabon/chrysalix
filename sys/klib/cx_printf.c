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
#include <stdarg.h>
#include <chrysalix.h>

/************************************************************************************
 * Globals
 */
static char cx_print_buf[CX_PRINT_BUF_SIZE];

void cx_llprintf(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    cx_vfprintf(llstdout, fmt, args);
    va_end(args);

}


void cx_printf(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    cx_vfprintf(stdout, fmt, args);
    va_end(args);

}

void cx_sprintf(char *buf, const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    cx_vsprintf(buf, fmt, args);
    va_end(args);
}

void cx_fprintf(fd_t stream, const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    cx_vfprintf(stream, fmt, args);
    va_end(args);
}

void cx_vfprintf(fd_t stream, const char *fmt, va_list args) {
    i32 retval;

    retval = cx_vsprintf(cx_print_buf, fmt, args);
    if (0 < retval)
        cx_writetm(stream, cx_print_buf, retval, -1);
}

/*-
 * Copyright (c) 2005, Kohsuke Ohtani
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
 *
 *
 * -- FROM PREX OS --
 *
 * Print formatted output - scaled down version
 *
 * Identifers:
 *  %d - Decimal signed int
 *  %x - Hex integer
 *  %u - Unsigned integer
 *  %c - Character
 *  %s - String
 *
 * Flags:
 *   0 - Zero pad
 */

static int prex_divide(long *n, int base) {
    int res;

    /*
     * Note: Optimized for ARM processor which does not support
     * prex_divide instructions.
     *
     * res = ((unsigned long)*n) % (unsigned int)base;
     * *n = ((unsigned long)*n) / (unsigned int)base;
     */

    if (base == 10) {
        res = ((unsigned long) *n) % 10U;
        *n = ((unsigned long) *n) / 10U;
    } else {
        res = ((unsigned long) *n) % 16U;
        *n = ((unsigned long) *n) / 16U;
    }
    return res;
}

int prex_atoi(const char *s) {
    int i = 0;
    while (isdigit((int) *s))
        i = i * 10 + *(s++) - '0';
    return i;
}

i32 cx_vsprintf(char *buf, const char *fmt, va_list args) {
    char *p, *str;
    const char *digits = "0123456789abcdef";
    char pad, tmp[16];
    i32 width, base, sign, i;
    long num;

    for (p = buf; *fmt; fmt++) {
        if (*fmt != '%') {
            *p++ = *fmt;
            continue;
        }
        /*
         * get flags
         */
        ++fmt;
        pad = ' ';
        if (*fmt == '0') {
            pad = '0';
            fmt++;
        }
        /*
         * get width
         */
        width = -1;
        if (isdigit(*fmt)) {
            width = prex_atoi(fmt);
        }
        base = 10;
        sign = 0;
        switch (*fmt) {
        case 'c':
            *p++ = (unsigned char) va_arg(args, int);
            continue;
        case 's':
            str = va_arg(args, char *);
            if (str == NULL)
                str = "<NULL>";
            for (; *str; str++) {
                *p++ = *str;
            }
            continue;
        case 'X':
        case 'x':
            base = 16;
            break;
        case 'd':
            sign = 1;
            break;
        case 'u':
            break;
        default:
            continue;
        }
        num = va_arg(args, long);
        if (sign && num < 0) {
            num = -num;
            *p++ = '-';
            width--;
        }
        i = 0;
        if (num == 0)
            tmp[i++] = '0';
        else
            while (num != 0)
                tmp[i++] = digits[prex_divide(&num, base)];
        width -= i;
        while (width-- > 0)
            *p++ = pad;
        while (i-- > 0)
            *p++ = tmp[i];
    }
    *p = '\0';
    return (p - buf);
}
