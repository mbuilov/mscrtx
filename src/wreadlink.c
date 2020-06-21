/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* wreadlink.c */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winioctl.h>
#include <errno.h>

#include "mscrtx/localerpl.h"
#include "mscrtx/xstat.h" /* xpathwc */
#include "mscrtx/wreadlink.h"

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#endif

#ifndef INT_MAX
#define INT_MAX ((unsigned)-1/2)
#endif

/* Ntifs.h may be not available in application WDK,
   define structure according to
   https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_reparse_data_buffer */
typedef struct reparse_buf {
	ULONG  ReparseTag;
	USHORT ReparseDataLength;
	USHORT Reserved;
	union {
		struct {
			USHORT SubstituteNameOffset;
			USHORT SubstituteNameLength;
			USHORT PrintNameOffset;
			USHORT PrintNameLength;
			ULONG  Flags;
			WCHAR  PathBuffer[1];
		} SymbolicLinkReparseBuffer;
		struct {
			USHORT SubstituteNameOffset;
			USHORT SubstituteNameLength;
			USHORT PrintNameOffset;
			USHORT PrintNameLength;
			WCHAR  PathBuffer[1];
		} MountPointReparseBuffer;
		struct {
			UCHAR DataBuffer[1];
		} GenericReparseBuffer;
	} u;
} REPARSE_DATA_BUFFER;

/* Note: caller must free(*dyn_rdb) on _any_ return */
static REPARSE_DATA_BUFFER *do_ioctl(const HANDLE h, REPARSE_DATA_BUFFER *rdb,
	DWORD rdb_size, REPARSE_DATA_BUFFER **const dyn_rdb, DWORD *filled)
{
	/* Assume path is less than 32768 wide characters.  */
	const DWORD add_by = 32768*sizeof(WCHAR);
	while (!DeviceIoControl(h, FSCTL_GET_REPARSE_POINT,
		NULL, 0, rdb, rdb_size, filled, NULL))
	{
		const DWORD err = GetLastError();
		if (ERROR_MORE_DATA != err) {
			errno = EINVAL;
			return NULL;
		}
		if (rdb_size > (DWORD)-1 - add_by) {
			errno = ENOMEM;
			return NULL;
		}
		rdb_size += add_by;
		if (*dyn_rdb)
			free(*dyn_rdb);
		*dyn_rdb = (REPARSE_DATA_BUFFER*)malloc(rdb_size);
		if (!*dyn_rdb)
			return NULL;
		rdb = *dyn_rdb;
	}
	return rdb;
}

/* Note: caller must free(*dyn_rdb) on _any_ return */
static WCHAR *readlink_ioctl(const HANDLE h, REPARSE_DATA_BUFFER *rdb,
	DWORD rdb_size, REPARSE_DATA_BUFFER **const dyn_rdb, USHORT *const link_len)
{
	DWORD filled;

	rdb = do_ioctl(h, rdb, rdb_size, dyn_rdb, &filled);
	if (!rdb)
		return NULL;

	if (filled < 8)
		goto notsup;

	if (!IsReparseTagMicrosoft(rdb->ReparseTag))
		goto notsup;

	{
		WCHAR *pbuf;
		USHORT offs, plen;	/* in bytes */
		switch (rdb->ReparseTag) {
			case IO_REPARSE_TAG_SYMLINK:
				/* fields of rdb->SymbolicLinkReparseBuffer must be filled.  */
				if (rdb->ReparseDataLength < 12u)
					goto notsup;
				offs = rdb->u.SymbolicLinkReparseBuffer.SubstituteNameOffset;
				plen = rdb->u.SymbolicLinkReparseBuffer.SubstituteNameLength;
				if (rdb->ReparseDataLength < 12u + offs + plen + sizeof(L'\0'))
					goto notsup;
				pbuf = rdb->u.SymbolicLinkReparseBuffer.PathBuffer;
				break;
			case IO_REPARSE_TAG_MOUNT_POINT:
				/* fields of rdb->MountPointReparseBuffer must be filled.  */
				if (rdb->ReparseDataLength < 8u)
					goto notsup;
				offs = rdb->u.MountPointReparseBuffer.SubstituteNameOffset;
				plen = rdb->u.MountPointReparseBuffer.SubstituteNameLength;
				if (rdb->ReparseDataLength < 8u + offs + plen + sizeof(L'\0'))
					goto notsup;
				pbuf = rdb->u.MountPointReparseBuffer.PathBuffer;
				break;
			default:
				goto notsup;
		}

		*link_len = (USHORT)(plen/sizeof(WCHAR));
		return &pbuf[offs/sizeof(WCHAR)];
	}

notsup:
	errno = ENOTSUP;
	return NULL;
}

A_Use_decl_annotations
int wreadlinkfd(const void *h, wchar_t buf[], const size_t bufsz)
{
	int ret = -1;
	union {
		REPARSE_DATA_BUFFER buf;
		char mem[sizeof(REPARSE_DATA_BUFFER) + MAX_PATH*sizeof(WCHAR)];
	} u;
	USHORT link_len;
	REPARSE_DATA_BUFFER *dyn_rdb = NULL;
	const WCHAR *link = readlink_ioctl((HANDLE)h, &u.buf, sizeof(u), &dyn_rdb, &link_len);
	if (link) {
		if (link_len <= bufsz) {
			memcpy(buf, link, link_len*sizeof(*link));
			/* Ensure link_len <= INT_MAX.  */
			(void)sizeof(int[1-2*((USHORT)-1 > INT_MAX)]);
			ret = (int)link_len;
		}
		else
			errno = ERANGE;
	}
	if (dyn_rdb)
		free(dyn_rdb);
	return ret;
}

A_Use_decl_annotations
int readlinkfd(const void *h, char buf[], const size_t bufsz)
{
	int ret = -1;
	union {
		REPARSE_DATA_BUFFER buf;
		char mem[sizeof(REPARSE_DATA_BUFFER) + MAX_PATH*sizeof(wchar_t)];
	} u;
	USHORT link_len;
	REPARSE_DATA_BUFFER *dyn_rdb = NULL;
	const WCHAR *link = readlink_ioctl((HANDLE)h, &u.buf, sizeof(u), &dyn_rdb, &link_len);
	if (link) {
		/* Convert wide-character path to multibyte string.  */
		const size_t converted = wcstombs(buf, link, bufsz);
		if ((size_t)-1 != converted) {
			/* Make sure that conversion wasn't stopped due to small buffer.  */
			if (bufsz < 6 || converted > bufsz - 6/*MB_CUR_MAX*/) {
				const size_t n = wcstombs(NULL, link, 0);
				if (n != converted) {
					errno = (size_t)-1 == n ? EILSEQ : ERANGE;
					goto err;
				}
			}
			if (converted <= INT_MAX)
				ret = (int)converted;
			else
				errno = ENAMETOOLONG;
		}
		else
			errno = EILSEQ;
	}
err:
	if (dyn_rdb)
		free(dyn_rdb);
	return ret;
}

A_Use_decl_annotations
int wreadlink(const wchar_t path[], wchar_t buf[], const size_t bufsz)
{
	const HANDLE h = xstat_open(path, /*dont_follow:*/1);
	if (INVALID_HANDLE_VALUE != h) {
		BY_HANDLE_FILE_INFORMATION info;
		if (GetFileInformationByHandle(h, &info)) {
			if (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
				const int r = wreadlinkfd(h, buf, bufsz);
				CloseHandle(h);
				return r;
			}
			errno = EINVAL;
		}
		else
			errno = EACCES;
		CloseHandle(h);
	}
	else {
		const DWORD err = GetLastError();
		errno = ERROR_ACCESS_DENIED == err ? EACCES : ENOENT;
	}
	return -1;
}

A_Use_decl_annotations
int readlink(const char path[], char buf[], const size_t bufsz)
{
	wchar_t wpath_buf[MAX_PATH];
	wchar_t *const wp = xpathwc(path, wpath_buf, sizeof(wpath_buf)/sizeof(wpath_buf[0]));
	if (wp) {
		const HANDLE h = xstat_open(wp, /*dont_follow:*/1);
		if (wp != wpath_buf)
			free(wp);
		if (INVALID_HANDLE_VALUE != h) {
			BY_HANDLE_FILE_INFORMATION info;
			if (GetFileInformationByHandle(h, &info)) {
				if (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
					const int r = readlinkfd(h, buf, bufsz);
					CloseHandle(h);
					return r;
				}
				errno = EINVAL;
			}
			else
				errno = EACCES;
			CloseHandle(h);
		}
		else {
			const DWORD err = GetLastError();
			errno = ERROR_ACCESS_DENIED == err ? EACCES : ENOENT;
		}
	}
	return -1;
}
