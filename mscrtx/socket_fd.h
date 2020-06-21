#ifndef SOCKET_FD_H_INCLUDED
#define SOCKET_FD_H_INCLUDED

/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* socket_fd.h */

/* emulation of file descriptors over socket handles */

/* close (2) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Success(!return)
#endif
int socket_fd_close(const int fd);

/* get socket by its descriptor number */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
SOCKET socket_fd_get(const int fd);

/* dup (2) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Success(return >= 0)
#endif
int socket_fd_dup(const int oldfd);

/* Allocate new socket descriptor number and associate given socket pointer with it.
   Returns new socket or -1 on error */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Success(return >= 0)
#endif
int socket_fd_attach(SOCKET s);

/* Delete internal socket descriptors table.
   Note: all socket descriptors must be closed (via socket_fd_close). */
void socket_fd_free_tab(void);

#endif /* SOCKET_FD_H_INCLUDED */
