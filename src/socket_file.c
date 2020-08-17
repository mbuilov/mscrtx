/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* socket_file.c */

#include <stdio.h>
#include <errno.h>
#include <winsock2.h>

#include "mscrtx/socket_file.h"
#include "mscrtx/socket_fd.h"

#ifndef SOCKET_FILE_BUFFER_SIZE
#define SOCKET_FILE_BUFFER_SIZE 4096
#endif

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#endif

A_Use_decl_annotations
socket_file_t *socket_file_alloc(void)
{
	struct socket_file *const sf = (struct socket_file*)malloc(sizeof(*sf));
	if (sf) {
		sf->buf = malloc(SOCKET_FILE_BUFFER_SIZE);
		if (sf->buf) {
			sf->buf_size = SOCKET_FILE_BUFFER_SIZE;
			sf->filled = 0;
			return sf;
		}
		free(sf);
	}
	return NULL;
}

A_Use_decl_annotations
void socket_file_free(socket_file_t *const sf)
{
	free(sf->buf);
	free(sf);
}

A_Use_decl_annotations
int socket_file_fflush(socket_file_t *const sf, const int fd)
{
	if (!sf->filled)
		return 0;
	if (sf->filled < 0) {
		errno = EIO;
		return EOF;
	}
	{
		const SOCKET s = socket_fd_get(fd);
		const int ret = send(s, (const char*)sf->buf, sf->filled, 0);
		if (SOCKET_ERROR == ret) {
			errno = EIO;
			sf->filled = 0 - sf->filled - 1;
			return EOF;
		}
	}
	sf->filled = 0;
	return 0;
}

A_Use_decl_annotations
size_t socket_file_fwrite(socket_file_t *const sf, const int fd,
	const void *ptr, const size_t size, const size_t nmemb)
{
	int ret;
	SOCKET s;
	size_t n, result, to_send, rem;
	unsigned space;
	if (nmemb == 0 || size == 0 || sf->filled < 0)
		return 0;
	s = socket_fd_get(fd);
	n = nmemb;
	space = sf->buf_size - (unsigned)sf->filled;
	if (size == 1) {
		to_send = n;
		if (to_send > space) {
			n = to_send - space;
			to_send = space;
		}
		rem = 0;
	}
	else {
		to_send = size;
		while (--n && to_send < space)
			to_send += size;
		if (to_send > space) {
			rem = to_send - space;
			to_send = space;
		}
		else
			rem = 0;
	}
	if (sf->filled || (unsigned)to_send != space) {
		char *dest = (char*) sf->buf + sf->filled;
		unsigned filled = (unsigned)((char*)memcpy(dest, ptr, to_send)
			- (char*)sf->buf) + (unsigned)to_send;
		if (filled != sf->buf_size) {
			sf->filled = (int)filled;
			return nmemb;
		}
		ptr = (const char*)ptr + (unsigned)to_send;
		ret = send(s, (const char*)sf->buf, (int)filled, 0);
		if (ret == SOCKET_ERROR) {
			errno = EIO;
			sf->filled = 0 - (int)(filled - (unsigned)(size - rem)) - 1;
			return nmemb - n - 1;
		}
		space = (unsigned)filled;
		goto block_sent;
	}
	for (result = 0;;) {
		ret = send(s, (const char*)ptr, (int)space, 0);
		if (ret == SOCKET_ERROR) {
			errno = EIO;
			sf->filled = -1;
			return result;
		}
		ptr = (const char*)ptr + space;
block_sent:
		result = nmemb - n - !!rem;
		if (size == 1) {
			to_send = n;
			if (to_send > space) {
				n = to_send - space;
				to_send = space;
			}
			else if (to_send != space)
				break;
		}
		else {
			for (to_send = rem; n && to_send < space; n--)
				to_send += size;
			if (to_send > space) {
				rem = to_send - space;
				to_send = space;
			}
			else if (to_send != space)
				break;
			else
				rem = 0;
		}
	}
	sf->filled = (int)to_send;
	if (to_send)
		memcpy(sf->buf, ptr, to_send);
	return nmemb;
}

A_Use_decl_annotations
void socket_file_clearerr(socket_file_t *const sf)
{
	if (sf->filled < 0)
		sf->filled = 0 - (sf->filled + 1);
}
