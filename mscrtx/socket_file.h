#ifndef SOCKET_FILE_H_INCLUDED
#define SOCKET_FILE_H_INCLUDED

/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* socket_file.h */

/* Emulation of FILE api over a socket */

typedef struct socket_file {
	unsigned buf_size;
	int filled;
	void *buf;
} socket_file_t;

#define socket_file_ferror(sf)	((sf)->filled < 0)

/* Allocate socket file structure.
   Returns NULL on error */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Success(return)
#endif
socket_file_t *socket_file_alloc(void);

/* Free socket file structure allocated by socket_file_alloc() */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(sf, A_In A_Post_ptr_invalid)
#endif
void socket_file_free(socket_file_t *const sf);

/* fflush (3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(sf, A_Inout)
A_Success(!return)
#endif
int socket_file_fflush(socket_file_t *const sf, const int fd);

/* fwrite (3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(sf, A_Inout)
A_At(ptr, A_In_reads_bytes(size*nmemb))
#endif
size_t socket_file_fwrite(socket_file_t *const sf, const int fd,
	const void *ptr, const size_t size, const size_t nmemb);

/* clearerr(3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(sf, A_Inout)
#endif
void socket_file_clearerr(socket_file_t *const sf);

#endif /* SOCKET_FILE_H_INCLUDED */
