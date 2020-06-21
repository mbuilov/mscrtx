#ifndef UTF8ENV_H_INCLUDED
#define UTF8ENV_H_INCLUDED

/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* utf8env.h */

/* UTF-8 wrappers around wide-character environment access API.  */

/* delete shadow utf8 environment variables table */
void utf8_env_shadow_reset(void);

/* delete both the real and shadow environment tables */
int utf8_clearenv(void);

/* these functions will create shadow utf8 environment variables table
   by the first call - if failed, utf8_env_fatal() callback is called */

/* environ (5) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Ret_never_null
A_Ret_valid
#endif
char **utf8_environ(void);

/* getenv (3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(name, A_In_z)
A_Ret_maybenull_z
#endif
char *utf8_getenv(const char name[]);

/* setenv (3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(name, A_In_z)
A_At(value, A_In_z)
A_Success(!return)
#endif
int utf8_setenv(const char name[], const char value[], int overwrite);

/* unsetenv (3) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(name, A_In_z)
A_Success(!return)
#endif
int utf8_unsetenv(const char name[]);

/* Annotate function that does not return (exits the program).  */
#ifndef ATTRIBUTE_NORETURN
# ifdef _MSC_VER
#  define ATTRIBUTE_NORETURN __declspec(noreturn)
# else
#  define ATTRIBUTE_NORETURN __attribute__ ((__noreturn__))
# endif
#endif

/* external callback
  - called if failed to create shadow utf8 environment variables table */
/* NOTE: must be implemented outside of this library */
ATTRIBUTE_NORETURN extern void utf8_env_fatal(void);

#endif /* UTF8ENV_H_INCLUDED */
