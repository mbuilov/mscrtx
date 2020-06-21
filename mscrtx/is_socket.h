#ifndef IS_SOCKET_H_INCLUDED
#define IS_SOCKET_H_INCLUDED

/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* is_socket.h */

/*
 * Microsoft's CRT library do not supports socket FDs, but because number of
 * file FDs is limited, use most significant bit of positive integer as a flag
 * denoting a socket descriptor.
 */
#define SOCKET_FD_BIT (((unsigned int)-1 >> 2) + 1)

/* Check if a file descriptor is a socket descriptor.  */
#define is_socket(fd) (((unsigned int)(fd) & ~((unsigned int)-1 >> 2)) == SOCKET_FD_BIT)

#endif /* IS_SOCKET_H_INCLUDED */
