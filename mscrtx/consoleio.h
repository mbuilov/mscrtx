#ifndef CONSOLEIO_H_INCLUDED
#define CONSOLEIO_H_INCLUDED

/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* consoleio.h */

/* returns -1 if failed, 0 on EOF, else - may return < count */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(count, A_In_range(1, INT_MAX))
A_At(buf, A_Notnull)
A_At(buf, A_Pre_writable_byte_size(count))
A_At(buf, A_Post_readable_byte_size(return))
A_Success(return >= 0)
#endif
int read_console(int fd, void *buf, unsigned count/*[1..INT_MAX]*/);

/* returns -1 if failed, 0 on EOF, else - may return < count */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(count, A_In_range(1, INT_MAX))
A_At(buf, A_Notnull)
A_At(buf, A_Pre_writable_size(count))
A_At(buf, A_Post_readable_size(return))
A_Success(return >= 0)
#endif
int read_console_w(int fd, wchar_t buf[], unsigned count/*[1..INT_MAX]*/);

/* returns (size_t)-1 if failed, 0 on EOF, else - may return < count */
/* if 'until_nl' is non-zero, then reading stops after '\n' is put into the 'buf' */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(count, A_In_range(>,0))
A_At(buf, A_Notnull)
A_At(buf, A_Pre_writable_byte_size(count))
A_At(buf, A_Post_readable_byte_size(return))
A_At(stream, A_Inout)
A_Success(return != A_Size_t(-1))
#endif
size_t fread_console_(void *buf, size_t count/*[1..(size_t)-2]*/, FILE *stream, int until_nl);

#define fread_console(buf, count, stream)    fread_console_(buf, count, stream, /*until_nl:*/0)
#define fread_console_nl(buf, count, stream) fread_console_(buf, count, stream, /*until_nl:*/1)

/* returns (size_t)-1 if failed, 0 on EOF, else - may return < count */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(count, A_In_range(>,0))
A_At(buf, A_Notnull)
A_At(buf, A_Pre_writable_size(count))
A_At(buf, A_Post_readable_size(return))
A_At(stream, A_Inout)
A_Success(return != A_Size_t(-1))
#endif
size_t fread_console_w(wchar_t buf[], size_t count/*[1..(size_t)-2]*/, FILE *stream);

/* returns -1 if failed, 0 - if all data has been written */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(count, A_In_range(1, INT_MAX))
A_At(buf, A_Notnull)
A_At(buf, A_Pre_readable_byte_size(count))
A_Success(!return)
#endif
int write_console(int fd, const void *buf, unsigned count/*[1..INT_MAX]*/);

/* returns -1 if failed, 0 - if all data has been written */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(count, A_In_range(1, INT_MAX))
A_At(buf, A_Notnull)
A_At(buf, A_Pre_readable_size(count))
A_Success(!return)
#endif
int write_console_w(int fd, const wchar_t buf[], unsigned count/*[1..INT_MAX]*/);

/* returns -1 if failed, 0 - if all data has been written */
/* if 'append_nl' is non-zero, then additionally write '\n' at end */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(count, A_In_range(>,0))
A_At(buf, A_Notnull)
A_At(buf, A_Pre_readable_byte_size(count))
A_At(stream, A_Inout)
A_Success(!return)
#endif
int fwrite_console_(const void *buf, size_t count/*>0*/, FILE *stream, int append_nl);

#define fwrite_console(buf, count, stream)    fwrite_console_(buf, count, stream, /*append_nl:*/0)
#define fwrite_console_nl(buf, count, stream) fwrite_console_(buf, count, stream, /*append_nl:*/1)

/* returns -1 if failed, 0 - if all data has been written */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(count, A_In_range(>,0))
A_At(buf, A_Notnull)
A_At(buf, A_Pre_readable_size(count))
A_At(stream, A_Inout)
A_Success(!return)
#endif
int fwrite_console_w(const wchar_t buf[], size_t count/*>0*/, FILE *stream);

/* reset internal buffers */
void console_buffers_reset(void);

#endif /* CONSOLEIO_H_INCLUDED */
