#ifndef ARG_PARSER_H_INCLUDED
#define ARG_PARSER_H_INCLUDED

/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* arg_parser.h */

struct wide_arg {
	struct wide_arg *next;
	wchar_t value[1];
};

/* Parse command line to NULL-terminated list of command-line arguments.
   Remove unescaped double-quotes, expand wildcards.
   Returns list of parsed arguments and their count: first argument is the program name.
   Returns NULL on failure. */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(argc, A_Out)
A_Success(return)
#endif
struct wide_arg *arg_parse_command_line(int *const argc/*out*/);

/* Free arguments list created by arg_parse_command_line() */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(list, A_In A_Post_ptr_invalid)
#endif
void arg_free_wide_args(struct wide_arg *list);

/* Get program module name.
   sz - is the buf size, in wide-chars, if 0 - the buf is not used.
   Returns buf or new malloc'ated buffer if buf is too small.
   Returns NULL on failure. */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_When(sz, A_At(buf, A_Pre_writable_size(sz)))
A_Ret_z
A_Success(return)
#endif
wchar_t *arg_get_module_name(wchar_t buf[/*sz*/]/*NULL?*/, unsigned sz/*0?*/);

struct arg_convert_err {
	unsigned number;    /* 0-based number of wide-character argument, -1 if not set */
	const wchar_t *arg; /* wide-character argument, NULL if not set */
};

/* Convert program arguments from wide-char to multibyte according to current locale.
   Returns NULL-terminated array of pointers to '\0'-terminated strings.
   Returns NULL on error and, if err != NULL, fills err:
  err->number and err->arg are will be set only if failed to convert an argument
  from wide-character string to multibyte-string. */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(2)
A_At(list, A_In)
A_At(err, A_On_failure(A_Out_opt))
A_Success(return)
#endif
char **arg_convert_wide_args(const unsigned argc, const struct wide_arg *list,
	struct arg_convert_err *err/*NULL?,out*/);

/* Free arguments array allocated by arg_convert_wide_args() */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(argv, A_In A_Post_ptr_invalid)
#endif
void arg_free_argv(char **const argv);

#endif /* ARG_PARSER_H_INCLUDED */
