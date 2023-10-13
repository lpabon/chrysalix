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
 
#ifndef _CX_STRING_H
#define _CX_STRING_H


/*****************************************************************
 * Prototypes
 */
char *cx_strncpy(char *dest, const char *src, i32 count);
int  cx_strncmp(const char *src, const char *tgt, i32 count);
i32  cx_strnlen(const char *str, i32 count);
void *cx_memcpy(void *dest, const void *src, i32 count);
void *cx_memset(void *dest, int ch, i32 count);
void cx_llprintf(const char *fmt, ...);
void cx_printf(const char *fmt, ...);
void cx_sprintf(char *buf, const char *fmt, ...);
void cx_fprintf(fd_t stream, const char *fmt, ...);
void cx_vfprintf(fd_t stream, const char *fmt, va_list args);
i32 cx_vsprintf(char *buf, const char *fmt, va_list args);
int prex_atoi(const char *s);


/*****************************************************************
 * Defines
 */
#define strncpy cx_strncpy
#define strncmp cx_strncmp
#define strlen cx_strnlen
#define memcpy cx_memcpy
#define memset cx_memset
#define llprintf cx_llprintf
#define printf cx_printf
#define sprintf cx_sprintf
#define fprintf cx_fprintf
#define vsprintf cx_vsprintf
#define atoi prex_atoi

#endif /* _CX_STRING_H */
