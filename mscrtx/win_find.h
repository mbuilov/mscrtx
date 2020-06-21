#ifndef WIN_FIND_H_INCLUDED
#define WIN_FIND_H_INCLUDED

/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* win_find.h */

/* Desclaration of struct win_find used by opendir/readdir/closedir from wreaddir.h */

/* Directory search structure.  */
struct win_find {
	HANDLE search_handle;
	WIN32_FIND_DATAW ffd;
	struct dir_fd_ dfd;
};

/* Get directory descriptor from the directory search structure.  */
#define dirfd(dirp)	(&(dirp)->dfd)

#endif /* WIN_FIND_H_INCLUDED */
