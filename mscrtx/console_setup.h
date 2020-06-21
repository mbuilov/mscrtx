#ifndef CONSOLE_SETUP_H_INCLUDED
#define CONSOLE_SETUP_H_INCLUDED

/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* console_setup.h */

/* Check if given 'fd' is a file descriptor of console device.
  If it is, change file mode to _O_U16TEXT, so fwprintf() functions will work
   correctly.
  Returns previous file descriptor mode, which may be used to restore file
   mode by calling _setmode().
  Returns -1 if 'fd' is not a console descriptor (file mode wasn't changed) */
int console_set_wide(int fd);

#endif /* CONSOLE_SETUP_H_INCLUDED */
