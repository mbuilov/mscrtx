#ifndef WREADDIR_H_INCLUDED
#define WREADDIR_H_INCLUDED

/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* wreaddir.h */

/* Emulation of open(2)/close(2) for directories, fchdir(2), opendir(3), readdir(3)
   using native Windows API.  */

/* Directory descriptor.  */
typedef struct dir_fd_ {
	void *handle;
	wchar_t abs_path[1];
} *dir_fd_t;

/* Open directory or file.
   If path is NULL, open current directory.
   Returns 0 on success, -1 if failed.
   On failure errno can be set to:
   EILSEQ       - can't convert path to wide-character string,
   ENOMEM       - memory allocation failure,
   ENAMETOOLONG - path is too long,
   EAGAIN       - can't make path absolute,
   ENOENT       - path does not point to directory or file,
   EACCES       - access to directory or file is denied.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(path, A_In_opt_z)
A_At(dfd, A_Outptr)
A_Success(!return)
#endif
int opendirfd(const char path[]/*NULL?*/, dir_fd_t *const dfd/*out*/);

/* Open current directory.
   Returns 0 on success, -1 if failed.
   On failure errno can be set to:
   ENOMEM       - memory allocation failure,
   ENAMETOOLONG - path to current directory is too long,
   EAGAIN       - can't get path to current directory,
   ENOENT       - path does not point to directory or file,
   EACCES       - access to directory or file is denied.  */
#define opencurdirfd(dfd) opendirfd(NULL, dfd)

/* Close directory.
   Returns 0 on success, -1 if failed.
   On failure errno can be set to;
   EINVAL       - directory descriptor is not valid.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dfd, A_In A_Post_ptr_invalid)
A_Success(!return)
#endif
int closedirfd(struct dir_fd_ *dfd);

/* Change current directory.
   Returns 0 on success, -1 if failed.
   On failure errno can be set to:
   ENOENT       - path does not point to directory or file,
   EACCES       - access to directory or file is denied.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(dfd, A_In)
A_Success(!return)
#endif
int fchdir(const struct dir_fd_ *dfd);

/* declared in win_find.h */
struct win_find;

/* Open directory to enumerate files/directories in it.
   Returns NULL if failed.
   On failure errno can be set to:
   EILSEQ       - can't convert path to wide-character string,
   ENOMEM       - memory allocation failure,
   ENAMETOOLONG - path is too long,
   EAGAIN       - can't make path absolute,
   ENOENT       - path does not point to directory or file,
   EACCES       - access to directory or file is denied,
   EFAULT       - search failed.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(path, A_In_z)
A_Success(return)
#endif
struct win_find *opendir(const char path[]);

/* Close directory opened for the search.
   Returns 0 on success, -1 if failed.
   On failure errno can be set to:
   EINVAL       - directory search structure is not valid.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dirp, A_In A_Post_ptr_invalid)
A_Success(!return)
#endif
int closedir(struct win_find *const dirp);

struct _WIN32_FIND_DATAW;

/* Get next entry of the directory.
   Returns NULL if no more entries or on failure.
   Returned entry must be released via dp_release().
   To be able to detect a failure, set errno to 0 before the call.
   On failure errno can be set to:
   EACCES       - access to directory is denied.  */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(dirp, A_Inout)
A_Ret_opt_valid
#endif
struct _WIN32_FIND_DATAW *readdir(struct win_find *const dirp);

/* Release entry returned by readdir().  */
#define dp_release(dp) \
	/* mark that there is no cached entry */ \
	((void)((dp)->dwFileAttributes = ~(DWORD)0))

#endif /* WREADDIR_H_INCLUDED */
