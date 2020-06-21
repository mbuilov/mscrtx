#ifndef WREADLINK_H_INCLUDED
#define WREADLINK_H_INCLUDED

/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* wreadlink.h */

/* Emulation of readlink(2) using native Windows API.  */

/* Get the path symbolic link points to by file handle.
   File must exist, must be opened with FILE_FLAG_OPEN_REPARSE_POINT flag.
   Assume bufsiz is the buf size in wide-characters.
   Returns number of wide characters placed in buf, -1 if failed.
   On failure errno can be set to:
   EINVAL       - ReparsePoint IOCTL on file handle failed,
   ENOMEM       - memory allocation failure,
   ENOTSUP      - ReparsePoint file has unsupported format,
   ERANGE       - output buffer size is too small.  */
/* Note: on success, buf is not L'\0'-terminated.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(buf, A_Pre_writable_size(bufsz) A_Post_readable_size(return))
A_Success(return >= 0)
#endif
int wreadlinkfd(const void *h, wchar_t buf[], const size_t bufsz);

/* Get the path symbolic link points to.
   Assume bufsiz is the buf size in wide-characters.
   Returns number of wide characters placed in buf, -1 if failed.
   On failure errno can be set to:
   EACCES       - access to directory or file is denied,
   ENOENT       - path does not point to directory or file,
   EINVAL       - path does not point to ReparsePoint file,
   ENOMEM       - memory allocation failure,
   ENOTSUP      - ReparsePoint file has unsupported format,
   ERANGE       - output buffer size is too small.  */
/* Note: on success, buf is not L'\0'-terminated.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(path, A_In_z)
A_At(buf, A_Pre_writable_size(bufsz) A_Post_readable_size(return))
A_Success(return >= 0)
#endif
int wreadlink(const wchar_t path[], wchar_t buf[], const size_t bufsz);

/* Get the path symbolic link points to by file handle.
   File must exist, must be opened with FILE_FLAG_OPEN_REPARSE_POINT flag.
   Returns number of bytes placed in buf, -1 if failed.
   On failure errno can be set to:
   EINVAL       - ReparsePoint IOCTL on file handle failed,
   ENOMEM       - memory allocation failure,
   ENOTSUP      - ReparsePoint file has unsupported format,
   EILSEQ       - can't convert resulting path to wide-character string,
   ERANGE       - output buffer size is too small,
   ENAMETOOLONG - resulting path length > INT_MAX.  */
/* Note: on success, buf is not '\0'-terminated.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(buf, A_Pre_writable_size(bufsz) A_Post_readable_size(return))
A_Success(return >= 0)
#endif
int readlinkfd(const void *h, char buf[], const size_t bufsz);

/* Get the path symbolic link points to.
   Returns number of bytes placed in buf, -1 if failed.
   On failure errno can be set to:
   EACCES       - access to directory or file is denied,
   ENOENT       - path does not point to directory or file,
   EINVAL       - path does not point to ReparsePoint file,
   ENOMEM       - memory allocation failure,
   ENOTSUP      - ReparsePoint file has unsupported format,
   EILSEQ       - failed to convert input path to wide-character string,
   EILSEQ       - can't convert resulting path to wide-character string,
   ERANGE       - output buffer size is too small,
   ENAMETOOLONG - input path exceeds the limit of 32767 wide characters,
   ENAMETOOLONG - resulting path length > INT_MAX.  */
/* Note: on success, buf is not '\0'-terminated.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(path, A_In_z)
A_At(buf, A_Pre_writable_size(bufsz) A_Post_readable_size(return))
A_Success(return >= 0)
#endif
int readlink(const char path[], char buf[], const size_t bufsz);

#endif /* WREADLINK_H_INCLUDED */
