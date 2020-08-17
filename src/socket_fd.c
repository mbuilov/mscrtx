/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* socket_fd.c */

#include <assert.h>
#include <winsock2.h>

#include "mscrtx/socket_fd.h"
#include "mscrtx/is_socket.h"

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#endif

#ifndef SOCKET_TAB_MAX_ROWS
#define SOCKET_TAB_MAX_ROWS  128
#endif

/* SOCKET_TAB_MAX_ROWS must be non-zero */
typedef int socket_tab_max_rows_is_zero_[1 - 2*!SOCKET_TAB_MAX_ROWS];

#ifndef SOCKET_ARRAY_SIZE
#define SOCKET_ARRAY_SIZE    64
#endif

/* SOCKET_ARRAY_SIZE must be non-zero */
typedef int socket_array_size_is_zero_[1 - 2*!SOCKET_ARRAY_SIZE];

/* SOCKET_ARRAY_SIZE must be power of the two */
typedef int socket_array_size_is_not_power_of_two_[1 -
	2*!!((SOCKET_ARRAY_SIZE - 1) & SOCKET_ARRAY_SIZE)];

typedef SOCKET socket_array_t[SOCKET_ARRAY_SIZE];
static socket_array_t **socket_tab = NULL;
static socket_array_t *socket_arr_cached = NULL;
static size_t socket_tab_size = 0;
static size_t socket_tab_capacity = 0;

static socket_array_t *new_socket_array(void)
{
	socket_array_t *const arr = (socket_array_t*)malloc(sizeof(*arr));
	if (arr) {
		size_t j = 0;
		for (; j < SOCKET_ARRAY_SIZE; j++)
			(*arr)[j] = INVALID_SOCKET;
	}
	return arr;
}

static int alloc_socket_fd(void)
{
	/* find free cell */
	size_t i, j;
	for (i = 0; i < socket_tab_size; i++) {
		for (j = 0; j < SOCKET_ARRAY_SIZE; j++) {
			if ((*socket_tab[i])[j] == INVALID_SOCKET)
				return (int)(i*SOCKET_ARRAY_SIZE + j);
		}
	}

	/* no free cells */
	if (socket_tab_size == SOCKET_TAB_MAX_ROWS)
		return -1;

	/* add new row */
	if (socket_tab_size == socket_tab_capacity) {
		socket_array_t **const new_tab = (socket_array_t**)realloc(
			socket_tab, sizeof(*socket_tab)*(socket_tab_capacity + 1));
		if (!new_tab)
			return -1;
		socket_tab = new_tab;
		socket_tab_capacity++;
	}
	if (!socket_arr_cached) {
		socket_arr_cached = new_socket_array();
		if (!socket_arr_cached)
			return -1;
	}
	socket_tab[socket_tab_size++] = socket_arr_cached;
	socket_arr_cached = NULL;
	return (int)((socket_tab_size - 1)*SOCKET_ARRAY_SIZE);
}

A_Use_decl_annotations
int socket_fd_close(const int fd)
{
	int r = 0;
	const unsigned ufd = (unsigned)fd & ~SOCKET_FD_BIT;
	const size_t row = ufd/SOCKET_ARRAY_SIZE;
	const size_t idx = ufd%SOCKET_ARRAY_SIZE;
	SOCKET s = (*socket_tab[row])[idx];
	size_t dup_row;

	/* check if socket is not dup'ed */
	for (dup_row = 0; dup_row < socket_tab_size; dup_row++) {
		size_t dup_idx;
		for (dup_idx = 0; dup_idx < SOCKET_ARRAY_SIZE; dup_idx++) {
			if ((*socket_tab[dup_row])[dup_idx] == s &&
					(dup_row != row || dup_idx != idx))
				goto found_dup;
		}
	}
found_dup:

	/* don't close if socket was dup'ed */
	if (dup_row == socket_tab_size)
		r = closesocket(s);

	(*socket_tab[row])[idx] = INVALID_SOCKET;
	if (socket_tab_size - 1 != row || dup_row == row)
		return r;

	/* try to free tail rows */
	do {
		size_t j = 0;
		for (; j < SOCKET_ARRAY_SIZE; j++) {
			if ((*socket_tab[row])[j] != INVALID_SOCKET)
				return r;
		}
		if (socket_arr_cached)
			free(socket_arr_cached);
		socket_arr_cached = socket_tab[--socket_tab_size];
	} while (socket_tab_size);
	return r;
}

static SOCKET *socket_fd_get_p(const int fd)
{
	const unsigned ufd = (unsigned)fd & ~SOCKET_FD_BIT;
	const size_t row = ufd/SOCKET_ARRAY_SIZE;
	const size_t idx = ufd%SOCKET_ARRAY_SIZE;
	return &(*socket_tab[row])[idx];
}

A_Use_decl_annotations
SOCKET socket_fd_get(const int fd)
{
	return *socket_fd_get_p(fd);
}

void socket_fd_free_tab(void)
{
	/* all FDs must be closed */
	assert(socket_tab_size == 0);
	if (socket_arr_cached) {
		free(socket_arr_cached);
		socket_arr_cached = NULL;
	}
	if (socket_tab) {
		free(socket_tab);
		socket_tab = NULL;
	}
	socket_tab_capacity = 0;
}

A_Use_decl_annotations
int socket_fd_dup(const int oldfd)
{
	assert(is_socket(oldfd));
	{
		const int fd = alloc_socket_fd();
		if (fd != -1) {
			SOCKET s = socket_fd_get(oldfd);
			*socket_fd_get_p(fd) = s;
			return (int)((unsigned)fd | SOCKET_FD_BIT);
		}
	}
	return -1;
}

A_Use_decl_annotations
int socket_fd_attach(SOCKET s)
{
	const int fd = alloc_socket_fd();
	if (fd != -1) {
		*socket_fd_get_p(fd) = s;
		return (int)((unsigned)fd | SOCKET_FD_BIT);
	}
	return -1;
}
