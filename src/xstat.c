/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* xstat.c */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <errno.h>

#include "mscrtx/localerpl.h"
#include "mscrtx/xstat.h"

/* not defined under MinGW.org */
#ifndef ENOTSUP
#define ENOTSUP 129
#endif

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#endif

static int is_wslash(const wchar_t w)
{
	return L'\\' == w || L'/' == w;
}

static wchar_t is_walpha(const wchar_t w)
{
	if (L'A' <= w && w <= L'Z')
		return L'A';
	if (L'a' <= w && w <= L'z')
		return L'a';
	return L'\0';
}

A_Use_decl_annotations
wchar_t *xpathwc_alloc(const char path[], wchar_t buf[],
	const size_t buf_size, const size_t lim, void *(*alloc)(size_t sz))
{
	if (buf_size > 2/*red zone*/) {
		const size_t n = mbstowcs(buf, path, buf_size);
		if ((size_t)-1 == n) {
			errno = EILSEQ;
			return NULL;
		}
		if (n <= buf_size - 2/*red zone*/)
			return buf;
		/* Red zone is used.  Assume too small buffer.  */
	}
	{
		/* Determine length of needed buffer.  */
		const size_t n = mbstowcs(NULL, path, 0);
		if (n == (size_t)-1) {
			errno = EILSEQ;
			return NULL;
		}
		/* Path should not exceed the limit of lim wide characters,
		   including terminating L'\0'.  */
		if (n >= lim) {
			errno = ENAMETOOLONG;
			return NULL;
		}
		buf = (wchar_t*)(*alloc)(sizeof(*buf)*(n + 1));
		if (buf)
			(void)!mbstowcs(buf, path, n + 1);
		return buf;
	}
}

static wchar_t *get_full_path(const wchar_t path[], wchar_t abs_buf[], const size_t abs_buf_size)
{
	wchar_t *const ap = _wfullpath(abs_buf, path, abs_buf_size);
	if (ap)
		return ap;
	return _wfullpath(NULL, path, 0);
}

static int is_unc_root(const wchar_t *ap)
{
	if (!is_wslash(ap[0]) || !is_wslash(ap[1]) || is_wslash(ap[2]))
		return 0;

	ap += 2;
	do {
		if (L'\0' == *ap)
			return 0; /* "//" or "//abc" */
	} while (!is_wslash(*++ap));

	if (L'\0' == *++ap || is_wslash(*ap))
		return 0; /* "//x/" or "//x//" */

	do {
		if (L'\0' == *++ap)
			return 1; /* "//x/y" */
	} while (!is_wslash(*ap));

	if (L'\0' == *++ap)
		return 1; /* "//x/y/" */

	return 0; /* "//x/y/?" */
}

static int is_root_path(const wchar_t wp[])
{
	/* Check for a path to the root drive directory, like "C:\".  */
	wchar_t abs_buf[MAX_PATH];
	wchar_t *const ap = get_full_path(wp, abs_buf, sizeof(abs_buf)/sizeof(abs_buf[0]));

	if (ap == NULL)
		return 0;

	/* If longer than "C:\", check for a UNC root like "\\host\share\" */
	if (ap[0] && ap[1] && ap[2] && ap[3] && !is_unc_root(ap)) {
		if (ap != abs_buf)
			free(ap);
		return 0;
	}

	{
		const UINT type = GetDriveTypeW(ap);
		if (ap != abs_buf)
			free(ap);
		if (DRIVE_UNKNOWN == type ||
			DRIVE_NO_ROOT_DIR == type)
			return 0;
	}

	return 1;
}

static int wpath_get_dev(const wchar_t *wp, unsigned short *const dev)
{
	/* Skip long path "\\?\" or "\\.\" prefixes.  */
	if (wp[0] == L'\\' && wp[1] == L'\\' &&
		(wp[2] == L'?' || wp[2] == L'.') && wp[3] == L'\\')
		wp += 4;

	{
		const wchar_t r = is_walpha(wp[0]);
		if (r != L'\0' && wp[1] == L':') {
			/* Paths like "C:" are used to access devices,
			   not files/directories.  */
			if (wp[2] == L'\0')
				return -1;
			/* Path like "C:abc.txt" is a path to file "abc.txt"
			   relative to the current directory on disk "C:".  */
			*dev = (unsigned short)(wp[0] - r);
			return 0;
		}
	}

	{
		/* Note: drive will be 0 if current path is a UNC path.  */
		const unsigned short drive = (unsigned short)_getdrive();
		*dev = (unsigned short)(drive - 1);
	}

	return 0;
}

static unsigned long long hinfo_get_inode(const BY_HANDLE_FILE_INFORMATION *const info)
{
	unsigned long long inode = info->nFileIndexHigh;
	inode <<= 32;
	inode += info->nFileIndexLow;
	return inode;
}

static unsigned long long hinfo_get_size(const BY_HANDLE_FILE_INFORMATION *const info)
{
	unsigned long long size = info->nFileSizeHigh;
	size <<= 32;
	size += info->nFileSizeLow;
	return size;
}

/* https://en.wikipedia.org/wiki/Leap_year */
static int is_leap_year(const unsigned y/*since 1900*/)
{
	if (y % 4)
		return 0;
	if (y % 100)
		return 1;
	return !(((y + 300)/*year since 1600*/) % 400);
}

static unsigned leap_years_after_1900(const unsigned cy/*since 1900*/)
{
	/* Don't count current year.  */
	const unsigned y = cy - 1;
	return
		y/4              /* Every 4-th year */
		- y/100          /* Except every 100-th year */
		+ (y + 300)/400; /* But plus every 400-th year since 1600 */
}

A_Use_decl_annotations
xtime_t xtimegm(
	const unsigned year/*since 1900,>70*/,
	const unsigned month/*0..11*/,
	const unsigned day/*1..31*/,
	const unsigned hour/*0..23*/,
	const unsigned minute/*0..59*/,
	const unsigned second/*0..60*/,
	unsigned *const yday/*NULL?,out:0..365*/)
{
	/* days since the beginning of the year */
	unsigned yd = day - 1;
	if (yd > 30)
		return -1; /* invalid day */
	switch (month) {
		case 0:  yd += 0; break;
		case 1:  yd += 0 + 31; break;
		case 2:  yd += 0 + 31 + 28; break;
		case 3:  yd += 0 + 31 + 28 + 31; break;
		case 4:  yd += 0 + 31 + 28 + 31 + 30; break;
		case 5:  yd += 0 + 31 + 28 + 31 + 30 + 31; break;
		case 6:  yd += 0 + 31 + 28 + 31 + 30 + 31 + 30; break;
		case 7:  yd += 0 + 31 + 28 + 31 + 30 + 31 + 30 + 31; break;
		case 8:  yd += 0 + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31; break;
		case 9:  yd += 0 + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30; break;
		case 10: yd += 0 + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31; break;
		case 11: yd += 0 + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30; break;
		default:
			return -1; /* invalid month */
	}
	if (year < 70)
		return -1; /* invalid year */
	yd += month > 1 && is_leap_year(year);
	{
		/* add days between Epoch 1970/1/1 and the year */
		unsigned long long t = yd + 365ull*(year - 70) + leap_years_after_1900(year) - leap_years_after_1900(70);
		if (hour > 23)
			return -1; /* invalid hour */
		if (minute > 59)
			return -1; /* invalid minute */
		if (second > 60)
			return -1; /* invalid second */
		t = ((t*24 + hour)*60 + minute)*60 + second;
		if (yday)
			*yday = yd;
		return (xtime_t)t;
	}
}

static xtime_t hinfo_convert_time(const FILETIME file_time, const xtime_t fallback)
{
	SYSTEMTIME system_time;

	if (file_time.dwLowDateTime == 0 && file_time.dwHighDateTime == 0)
		return fallback; /* Not supported by the FS, use fallback.  */

	if (!FileTimeToSystemTime(&file_time, &system_time))
		return -1; /* xtimegm() also returns -1 if fails */

	return xtimegm(
		(unsigned)system_time.wYear - 1900,
		(unsigned)system_time.wMonth - 1,
		(unsigned)system_time.wDay,
		(unsigned)system_time.wHour,
		(unsigned)system_time.wMinute,
		(unsigned)system_time.wSecond,
		NULL);
}

static unsigned short dup_mode_bits(unsigned short mode)
{
	mode = (unsigned short)(mode | ((mode & 0700) >> 3)); /* group permissions */
	mode = (unsigned short)(mode | ((mode & 0700) >> 6)); /* other permissions */
	return mode;
}

A_Use_decl_annotations
int xstat_root(const wchar_t wp[], struct xstat *const buf)
{
	if (!is_root_path(wp)) {
		errno = ENOENT;
		return -1;
	}

	if (wpath_get_dev(wp, &buf->st_dev)) {
		errno = ENOTSUP;
		return -1;
	}

	buf->st_mode = dup_mode_bits(_S_IFDIR | _S_IEXEC | _S_IREAD | _S_IWRITE);

	buf->st_nlink = 1;
	buf->st_ino   = 0;
	buf->st_size  = 0;

	buf->st_mtime = xtimegm(80/*1980*/, 0, 1, 0, 0, 0, NULL);
	buf->st_atime = buf->st_mtime;
	buf->st_ctime = buf->st_mtime;

	return 0;
}

static int is_exe(const wchar_t wp[])
{
	const wchar_t *const dot = wcsrchr(wp, L'.');
	if (dot == NULL)
		return 0;

#define CASE_LET(a)	case a: case L'A' + (a - L'a')
#define CMP_DOT(i,a)	(dot[i] == a || dot[i] == L'A' + (a - L'a'))
	switch (dot[1]) {
		CASE_LET(L'b'):
			return
				CMP_DOT(2, L'a') &&
				CMP_DOT(3, L't') &&
				dot[4] == L'\0';
		CASE_LET(L'e'):
			return
				CMP_DOT(2, L'x') &&
				CMP_DOT(3, L'e') &&
				dot[4] == L'\0';
		CASE_LET(L'c'):
			return
				(
					(CMP_DOT(2, L'm') &&
					CMP_DOT(3, L'd'))
					||
					(CMP_DOT(2, L'o') &&
					CMP_DOT(3, L'm'))
				)
				&& dot[4] == L'\0';
		default:
			return 0;
	}
#undef CMP_DOT
#undef CASE_LET
}

static int xstat_file(HANDLE h, const wchar_t wp[]/*NULL?*/, struct xstat *const buf)
{
	BY_HANDLE_FILE_INFORMATION info;

	if (!GetFileInformationByHandle(h, &info)) {
		errno = EACCES;
		return -1;
	}

	if (!wp)
		buf->st_dev = 0;
	else if (wpath_get_dev(wp, &buf->st_dev)) {
		errno = ENOTSUP;
		return -1;
	}

	if (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
		buf->st_mode = S_IFLNK | _S_IREAD | _S_IWRITE | _S_IEXEC;
	else {
		buf->st_mode = (unsigned short)(
			(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			? _S_IFDIR | _S_IEXEC : wp && is_exe(wp)
			? _S_IFREG | _S_IEXEC : _S_IFREG);

		buf->st_mode = (unsigned short)(buf->st_mode |
			((info.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
			? _S_IREAD : _S_IREAD | _S_IWRITE));
	}

	buf->st_mode = dup_mode_bits(buf->st_mode);

	buf->st_nlink = info.nNumberOfLinks;
	buf->st_ino   = hinfo_get_inode(&info);
	buf->st_size  = hinfo_get_size(&info);
	buf->st_mtime = hinfo_convert_time(info.ftLastWriteTime,  0);
	buf->st_atime = hinfo_convert_time(info.ftLastAccessTime, buf->st_mtime);
	buf->st_ctime = hinfo_convert_time(info.ftCreationTime,   buf->st_mtime);

	return 0;
}

A_Use_decl_annotations
int xfstat(void *h, const wchar_t wp[]/*NULL?*/, struct xstat *const buf)
{
	const DWORD file_type = GetFileType((HANDLE)h);

	if (file_type == FILE_TYPE_DISK)
		return xstat_file((HANDLE)h, wp, buf);

	if (file_type == FILE_TYPE_CHAR) {
		buf->st_dev   = (unsigned short)-1;
		buf->st_mode  = _S_IFCHR;
		buf->st_nlink = 1;
		buf->st_ino   = 0;
		buf->st_size  = 0;
		buf->st_atime = 0;
		buf->st_mtime = 0;
		buf->st_ctime = 0;
		return 0;
	}

	if (file_type == FILE_TYPE_PIPE) {
		DWORD avail;
		buf->st_dev   = (unsigned short)-1;
		buf->st_mode  = _S_IFIFO;
		buf->st_nlink = 1;
		buf->st_ino   = 0;
		buf->st_size  = PeekNamedPipe(h, NULL, 0, NULL, &avail, NULL) ? avail : 0;
		buf->st_atime = 0;
		buf->st_mtime = 0;
		buf->st_ctime = 0;
		return 0;
	}

	errno = EBADF;
	return -1;
}

A_Use_decl_annotations
int xwstat(const wchar_t path[], struct xstat *buf, int dont_follow)
{
	const HANDLE h = xstat_open(path, dont_follow);
	if (INVALID_HANDLE_VALUE != h) {
		const int r = xfstat(h, path, buf);
		CloseHandle(h);
		return r;
	}
	{
		const DWORD open_err = GetLastError();
		const int r = xstat_root(path, buf);
		if (r && errno == ENOENT) {
			if (ERROR_ACCESS_DENIED == open_err)
				errno = EACCES;
		}
		return r;
	}
}

static int xstat_internal(const char path[], struct xstat *const buf, const int dont_follow)
{
	/* Empty path is not allowed.  */
	if (*path) {
		wchar_t wpath_buf[MAX_PATH];
		wchar_t *const wp = xpathwc(path, wpath_buf, sizeof(wpath_buf)/sizeof(wpath_buf[0]));
		if (wp) {
			const int r = xwstat(wp, buf, dont_follow);
			if (wp != wpath_buf)
				free(wp);
			return r;
		}
		return -1;
	}
	errno = ENOENT;
	return -1;
}

A_Use_decl_annotations
int xstat(const char path[], struct xstat *buf)
{
	return xstat_internal(path, buf, /*folow:*/0);
}

A_Use_decl_annotations
int xlstat(const char path[], struct xstat *buf)
{
	return xstat_internal(path, buf, /*folow:*/1);
}
