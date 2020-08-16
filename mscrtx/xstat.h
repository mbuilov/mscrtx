#ifndef XSTAT_H_INCLUDED
#define XSTAT_H_INCLUDED

/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* xstat.h */

/* Emulation of stat(2)/lstat(2) for Windows:
  - unlike CRT's stat, set 64-bit inode values,
  - unlike CRT's stat, set real number of links,
  - xlstat do not follows re-parse points.  */

typedef unsigned short     xdev_t;
typedef unsigned long long xino_t;
typedef long long          xtime_t;

struct xstat {
	xdev_t             st_dev;
	/* combination of
	  type bits:
	    _S_IFREG  = 0x8000 (1000000000000000)
	    _S_IFDIR  = 0x4000 (0100000000000000)
	    _S_IFCHR  = 0x2000 (0010000000000000)
	    _S_IFIFO  = 0x1000 (0001000000000000)
	  mode bits:
	    _S_IREAD  = 0x0100 (0000000100100100) (repeated)
	    _S_IWRITE = 0x0080 (0000000010010010) (repeated)
	    _S_IEXEC  = 0x0040 (0000000001001001) (repeated)
	*/
	unsigned short     st_mode;
	/* Number of links to this file.
	   Not useful for directories.  */
	unsigned int       st_nlink;
	xino_t             st_ino;
	unsigned long long st_size;
	xtime_t            st_atime;
	xtime_t            st_mtime;
	xtime_t            st_ctime;
};

/* Note: not a typedef - to check if xstat.h is included by #ifdef xstat_t */
#define xstat_t struct xstat

/* This is non-standard for Windows.  */
#define S_IFLNK    (_S_IFREG | _S_IFCHR)
#define	S_ISLNK(m) (((m) & _S_IFMT) == S_IFLNK)

/* Open file or directory for xfstat.  */
#define xstat_open(wpath, dont_follow) \
	CreateFileW(wpath, FILE_READ_ATTRIBUTES, \
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, \
		NULL, OPEN_EXISTING, \
		(DWORD) (((dont_follow) ? FILE_FLAG_OPEN_REPARSE_POINT : 0) | \
			FILE_FLAG_BACKUP_SEMANTICS), \
		NULL)

/* Get stat info by file handle.
   Returns 0 on success, -1 if failed.
   If 'path' is NULL, then
    - st_dev will be set to 0,
    - st_mode will not have _S_IEXEC bits set.
   On failure errno can be set to:
   EBADF        - handle is not a file/directory/pipe/character device handle,
   EACCES       - access to directory or file is denied,
   ENOTSUP      - path points to a volume device, like "\\.\C:"  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(3)
A_At(path, A_In_opt_z)
A_At(buf, A_Out)
A_Success(!return)
#endif
int xfstat(void *h, const wchar_t path[]/*NULL?*/, struct xstat *const buf);

/* Get stat info by file path.
   Returns 0 on success, -1 if failed.
   On failure errno can be set to:
   ENOENT       - path does not point to directory or file,
   EBADF        - path points not to a file/directory/pipe/character device,
   EACCES       - access to directory or file is denied,
   ENOTSUP      - path points to a volume device, like "\\.\C:"  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(path, A_In_z)
A_At(buf, A_Out)
A_Success(!return)
#endif
int xwstat(const wchar_t path[], struct xstat *buf, int dont_follow);

#if defined(__cplusplus) && defined(__GNUC__) && __GNUC__ >= 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow" /* 'int xstat(const char*, xstat*)' hides constructor for 'struct xstat' */
#endif

/* Get stat info by file path.
   Returns 0 on success, -1 if failed.
   On failure errno can be set to:
   EILSEQ       - failed to convert path to wide-character string,
   ENAMETOOLONG - resulting path exceeds the limit of 32767 wide characters,
   ENOMEM       - memory allocation failure,
   ENOENT       - path does not point to directory or file,
   EBADF        - path points not to a file/directory/pipe/character device,
   EACCES       - access to directory or file is denied,
   ENOTSUP      - path points to a volume device, like "\\.\C:"  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(path, A_In_z)
A_At(buf, A_Out)
A_Success(!return)
#endif
int xstat(const char path[], struct xstat *buf);

#if defined(__cplusplus) && defined(__GNUC__) && __GNUC__ >= 6
#pragma GCC diagnostic pop
#endif

/* Same as xstat, but do not follow a symbolic link.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(path, A_In_z)
A_At(buf, A_Out)
A_Success(!return)
#endif
int xlstat(const char path[], struct xstat *buf);

/* Try to stat a root drive directory, like "C:\".
   Returns 0 on success, -1 if failed.
   On failure errno can be set to:
   ENOENT       - path does not point to a root drive directory,
   ENOTSUP      - path points to a volume device, like "\\.\C:"  */
/* Note: xstat/xlstat/xwstat correctly stat root drive directories,
   this function may be usable if there is a need to use xfstat and
   xstat_open() fails - path may point to a root drive directory, but
   the drive may be not ready.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(path, A_In_z)
A_At(buf, A_Out)
A_Success(!return)
#endif
int xstat_root(const wchar_t path[], struct xstat *const buf);

/* Convert given path to L'\0'-terminated wide-character string.
   May return buf if it's big enough, else returns dynamically allocated
    buffer, which must be free()'d after use.
   Assume buf_size is the buf size in wide-characters.
   Do not allocate buffer exceeding the lim wide-characters, including terminating L'\0'.
   Returns NULL on error, setting errno to one of:
   EILSEQ       - failed to convert path to wide-character string,
   ENAMETOOLONG - resulting path exceeds the limit of lim wide characters,
   ENOMEM       - memory allocation failure.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(1)
A_Nonnull_arg(5)
A_At(path, A_In_z)
A_At(buf, A_Pre_writable_size(buf_size))
A_At(alloc, A_Notnull)
A_Ret_z
A_Success(return)
#endif
wchar_t *xpathwc_alloc(const char path[], wchar_t buf[],
	const size_t buf_size, const size_t lim, void *(*alloc)(size_t sz));

/* Path should not exceed the limit of 32767 wide characters. */
#define xpathwc(path, buf, buf_size) \
	xpathwc_alloc(path, buf, buf_size, 32767, malloc)

/* Convert borken-down UTC time to number of seconds since Epoch (1970/1/1).
   Compute: yday - days since January 1.
   Returns -1 if input argument(s) are invalid */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_At(yday, A_Out_opt A_Out_range(0,365))
A_Success(return != -1)
#endif
xtime_t xtimegm(
	const unsigned year/*since 1900,>70*/,
	const unsigned month/*0..11*/,
	const unsigned day/*1..31*/,
	const unsigned hour/*0..23*/,
	const unsigned minute/*0..59*/,
	const unsigned second/*0..60*/,
	unsigned *const yday/*NULL?,out:0..365*/);

/* Get day-of-week (days since Sunday - [0,6]) for the given xtime_t (number of seconds since Epoch (1970/1/1)) */
#define xweekday(t) ((unsigned)(((t)/(24*60*60) + 4/* 1970/1/1 was Thursday */) % 7))

#endif /* XSTAT_H_INCLUDED */
