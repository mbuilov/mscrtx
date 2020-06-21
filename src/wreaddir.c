/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* wreaddir.c */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>

#include "mscrtx/xstat.h" /* xpathwc */
#include "mscrtx/wreaddir.h"
#include "mscrtx/win_find.h"

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#endif

#ifndef OFFSETOF
# if defined _MSC_VER && !defined __cplusplus
#  define OFFSETOF(type, member) \
     ((size_t)((const char*)&((type*)NULL)->member - (const char*)NULL))
# else
#  define OFFSETOF(type, member) offsetof(type, member)
# endif
#endif

static struct dir_fd_ *alloc_dir_fd(const DWORD abs_sz)
{
	const size_t head = OFFSETOF(struct dir_fd_, abs_path);
	if (abs_sz <= ((size_t)-1 - head)/sizeof(wchar_t)) {
		const size_t sz = head + (size_t)abs_sz*sizeof(wchar_t);
		return (struct dir_fd_*)malloc(sz);
	}
	errno = ENAMETOOLONG;
	return NULL;
}

static void free_dir_fd(struct dir_fd_ *const dfd)
{
	free(dfd);
}

static int dir_fd_open_handle(struct dir_fd_ *const buf)
{
	buf->handle = CreateFileW(buf->abs_path,
		FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (INVALID_HANDLE_VALUE == buf->handle) {
		const DWORD err = GetLastError();
		errno = ERROR_FILE_NOT_FOUND == err ? ENOENT : EACCES;
		return -1;
	}
	return 0;
}

static struct dir_fd_ *opencurdirfd_(void)
{
	const DWORD abs_sz = GetCurrentDirectoryW(0, NULL);
	if (abs_sz) {
		struct dir_fd_ *const buf = alloc_dir_fd(abs_sz);
		if (buf) {
			const DWORD abs_len = GetCurrentDirectoryW(abs_sz, buf->abs_path);
			if (abs_sz - 1 != abs_len)
				errno = EAGAIN;
			else if (!dir_fd_open_handle(buf))
				return buf;
			free(buf);
		}
	}
	else
		errno = ENOMEM;
	return NULL;
}

static struct dir_fd_ *opendirfd_(
	const char path[], DWORD *const p_abs_len,
	struct dir_fd_ *(*const alloc)(const DWORD abs_sz),
	void (*const dealloc)(struct dir_fd_ *const buf))
{
	wchar_t wpath_buf[MAX_PATH];
	wchar_t *const wp = xpathwc(path, wpath_buf, sizeof(wpath_buf)/sizeof(wpath_buf[0]));
	if (wp) {
		const DWORD abs_sz = GetFullPathNameW(wp, 0, NULL, NULL);
		if (abs_sz) {
			struct dir_fd_ *const buf = (*alloc)(abs_sz);
			if (buf) {
				const DWORD abs_len = GetFullPathNameW(wp, abs_sz, buf->abs_path, NULL);
				if (wpath_buf != wp)
					free(wp);
				if (abs_sz - 1 != abs_len)
					errno = EAGAIN;
				else if (!dir_fd_open_handle(buf)) {
					*p_abs_len = abs_len;
					return buf;
				}
				(*dealloc)(buf);
				return NULL;
			}
		}
		else
			errno = ENOMEM;
		if (wpath_buf != wp)
			free(wp);
	}
	return NULL;
}

A_Use_decl_annotations
int opendirfd(const char path[]/*NULL?*/, dir_fd_t *const dfd/*out*/)
{
	DWORD abs_len;
	struct dir_fd_ *const buf = !path ? opencurdirfd_()
		: opendirfd_(path, &abs_len, alloc_dir_fd, free_dir_fd);
	*dfd = buf;
	return buf ? 0 : -1;
}

A_Use_decl_annotations
int closedirfd(struct dir_fd_ *dfd)
{
	const BOOL ok = CloseHandle(dfd->handle);
	free(dfd);
	if (ok)
		return 0;
	errno = EINVAL;
	return -1;
}

A_Use_decl_annotations
int fchdir(const struct dir_fd_ *dfd)
{
	if (!SetCurrentDirectoryW(dfd->abs_path)) {
		const DWORD err = GetLastError();
		errno = ERROR_FILE_NOT_FOUND == err ? ENOENT : EACCES;
		return -1;
	}
	return 0;
}

static struct dir_fd_ *alloc_win_find(const DWORD abs_sz)
{
	const size_t head = OFFSETOF(struct win_find, dfd.abs_path);
	/* Reserve 2 wide-characters for "\*".  */
	if (abs_sz <= ((size_t)-1 - head)/sizeof(wchar_t) - 2) {
		const size_t sz = head + ((size_t)abs_sz + 2)*sizeof(wchar_t);
		struct win_find *const f = (struct win_find*)malloc(sz);
		if (f)
			return &f->dfd;
	}
	else
		errno = ENAMETOOLONG;
	return NULL;
}

static struct win_find *win_find_from_dfd(struct dir_fd_ *const dfd)
{
	const size_t head = OFFSETOF(struct win_find, dfd);
	void *const p = (char*)dfd - head;
	return (struct win_find*)p;
}

static void free_win_find(struct dir_fd_ *const dfd)
{
	struct win_find *const f = win_find_from_dfd(dfd);
	free(f);
}

A_Use_decl_annotations
struct win_find *opendir(const char path[])
{
	DWORD abs_len;
	struct dir_fd_ *const dfd = opendirfd_(path,
		&abs_len, alloc_win_find, free_win_find);
	if (dfd) {
		struct win_find *const f = win_find_from_dfd(dfd);
		/* Fill reserved 2 wide-characters for "\*".  */
		if (!abs_len ||
			(f->dfd.abs_path[abs_len - 1] != L'\\' &&
			f->dfd.abs_path[abs_len - 1] != L'/'))
		{
			f->dfd.abs_path[abs_len++] = L'\\';
		}
		f->dfd.abs_path[abs_len++] = L'*';
		f->dfd.abs_path[abs_len] = L'\0';
		f->search_handle = FindFirstFileW(f->dfd.abs_path, &f->ffd);
		if (INVALID_HANDLE_VALUE != f->search_handle &&
			~(DWORD)0 != f->ffd.dwFileAttributes)
		{
			/* Trim the path back.  */
			f->dfd.abs_path[abs_len - 1] = L'\0';
			return f;
		}
		{
			const DWORD last_err = GetLastError();
			const int err = INVALID_HANDLE_VALUE != f->search_handle
				? EFAULT : ERROR_FILE_NOT_FOUND == last_err
				? ENOENT : EACCES;
			if (INVALID_HANDLE_VALUE != f->search_handle)
				(void)FindClose(f->search_handle);
			(void)CloseHandle(f->dfd.handle);
			free(f);
			errno = err;
		}
	}
	return NULL;
}

A_Use_decl_annotations
int closedir(struct win_find *const dirp)
{
	const BOOL ok1 = FindClose(dirp->search_handle);
	const BOOL ok2 = CloseHandle(dirp->dfd.handle);
	free(dirp);
	if (ok1 && ok2)
		return 0;
	errno = EINVAL;
	return -1;
}

A_Use_decl_annotations
WIN32_FIND_DATAW *readdir(struct win_find *const dirp)
{
	if (~(DWORD)0 == dirp->ffd.dwFileAttributes) {
		/* No cached entry.  */
		if (!FindNextFileW(dirp->search_handle, &dirp->ffd)) {
			const DWORD err = GetLastError();
			errno = ERROR_NO_MORE_FILES == err ? 0 : EACCES;
			return NULL;
		}
	}
	return &dirp->ffd;
}
