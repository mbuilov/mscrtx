#ifndef CONSOLE_SETUP_H_INCLUDED
#define CONSOLE_SETUP_H_INCLUDED

/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* console_setup.h */

/* Check if given 'fd' is a file descriptor of console device.
  If it is, change file mode to _O_BINARY, to disable internal text encoding
   conversion (from locale code page to console code page) in write()/fwrite()
   functions.
  Returns previous file descriptor mode (likely _O_TEXT), which must be used to
   restore file mode by calling turn_off_console_fd(fd, old_mode).
  Returns -1 if 'fd' is not a console descriptor (file mode wasn't changed) */
int turn_on_console_fd(int fd);

/* Restore file descriptor mode changed by previous call to turn_on_console_fd() */
#define turn_off_console_fd(fd, old_mode) _setmode(fd, old_mode)

#endif /* CONSOLE_SETUP_H_INCLUDED */
