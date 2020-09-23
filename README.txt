Description.
Library of replacement/missing functions of the Microsoft's CRT API.
- Provides a command line argument parsing function that supports escaping double quotes with two consecutive double quotes, as well as expanding the list of files specified by the wildcards.
- Adds UTF-8 locale encoding support (by wrapping common locale-specific CRT functions).
- Implements readlink(2) and analogs of open(2)/close(2) for directories, fchdir(2), opendir(3), readdir(3).

Author.
Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx

License.
GNU General Public License version 3 or any later version, see COPYING.

Dependencies:
https://github.com/mbuilov/libutf16
https://github.com/mbuilov/unicode_ctype

Optional dependencies.
"sal_defs.h" from https://github.com/mbuilov/cmn_headers


Building.
You need any c99 compiler,

for example MinGW gcc:
gcc -g -O2 -D_POSIX_C_SOURCE -I. -c -Wall -Wextra .\src\arg_parser.c
gcc -g -O2 -D_POSIX_C_SOURCE -I. -c -Wall -Wextra .\src\socket_fd.c
gcc -g -O2 -D_POSIX_C_SOURCE -I. -c -Wall -Wextra .\src\socket_file.c
gcc -g -O2 -D_POSIX_C_SOURCE -I. -c -Wall -Wextra .\src\wreaddir.c
gcc -g -O2 -D_POSIX_C_SOURCE -I. -c -Wall -Wextra .\src\wreadlink.c
gcc -g -O2 -D_POSIX_C_SOURCE -I. -c -Wall -Wextra .\src\xstat.c
gcc -g -O2 -D_POSIX_C_SOURCE -I. -c -Wall -Wextra .\src\locale_helpers.c
gcc -g -O2 -D_POSIX_C_SOURCE -I. -c -Wall -Wextra -I..\libutf16 .\src\utf16cvt.c
gcc -g -O2 -D_POSIX_C_SOURCE -I. -c -Wall -Wextra -I..\libutf16 .\src\consoleio.c
gcc -g -O2 -D_POSIX_C_SOURCE -I. -c -Wall -Wextra -I..\libutf16 -I..\unicode_ctype .\src\utf8env.c
gcc -g -O2 -D_POSIX_C_SOURCE -I. -c -Wall -Wextra -I..\libutf16 -I..\unicode_ctype .\src\localerpl.c
ar -crs mscrtx.a      ^
  .\arg_parser.o      ^
  .\socket_fd.o       ^
  .\socket_file.o     ^
  .\wreaddir.o        ^
  .\wreadlink.o       ^
  .\xstat.o           ^
  .\locale_helpers.o  ^
  .\utf16cvt.o        ^
  .\consoleio.o       ^
  .\utf8env.o         ^
  .\localerpl.o

or MSVC:
cl /wd4711 /wd4820 /wd5045 /D_CRT_SECURE_NO_WARNINGS /O2 /I. /c /Wall .\src\arg_parser.c
cl /wd4711 /wd4820 /wd5045 /D_CRT_SECURE_NO_WARNINGS /O2 /I. /c /Wall .\src\socket_fd.c
cl /wd4711 /wd4820 /wd5045 /D_CRT_SECURE_NO_WARNINGS /O2 /I. /c /Wall .\src\socket_file.c
cl /wd4711 /wd4820 /wd5045 /D_CRT_SECURE_NO_WARNINGS /O2 /I. /c /Wall .\src\wreaddir.c
cl /wd4711 /wd4820 /wd5045 /D_CRT_SECURE_NO_WARNINGS /O2 /I. /c /Wall .\src\wreadlink.c
cl /wd4711 /wd4820 /wd5045 /D_CRT_SECURE_NO_WARNINGS /O2 /I. /c /Wall .\src\xstat.c
cl /wd4711 /wd4820 /wd5045 /D_CRT_SECURE_NO_WARNINGS /O2 /I. /c /Wall .\src\locale_helpers.c
cl /wd4711 /wd4820 /wd5045 /D_CRT_SECURE_NO_WARNINGS /O2 /I. /c /Wall -I..\libutf16 .\src\utf16cvt.c
cl /wd4711 /wd4820 /wd5045 /D_CRT_SECURE_NO_WARNINGS /O2 /I. /c /Wall -I..\libutf16 .\src\consoleio.c
cl /wd4711 /wd4820 /wd5045 /D_CRT_SECURE_NO_WARNINGS /O2 /I. /c /Wall -I..\libutf16 -I..\unicode_ctype .\src\utf8env.c
cl /wd4711 /wd4820 /wd5045 /D_CRT_SECURE_NO_WARNINGS /O2 /I. /c /Wall -I..\libutf16 -I..\unicode_ctype .\src\localerpl.c
lib /out:mscrtx.a       ^
  .\arg_parser.obj      ^
  .\socket_fd.obj       ^
  .\socket_file.obj     ^
  .\wreaddir.obj        ^
  .\wreadlink.obj       ^
  .\xstat.obj           ^
  .\locale_helpers.obj  ^
  .\utf16cvt.obj        ^
  .\consoleio.obj       ^
  .\utf8env.obj         ^
  .\localerpl.obj



Also, the library can be built with source annotations (restricted pointers, non-null attributes, etc.)

MinGW gcc:
gcc -g -O2 -D_POSIX_C_SOURCE -include ../cmn_headers/sal_defs.h -I. -c -Wall -Wextra -fanalyzer .\src\arg_parser.c
gcc -g -O2 -D_POSIX_C_SOURCE -include ../cmn_headers/sal_defs.h -I. -c -Wall -Wextra -fanalyzer .\src\socket_fd.c
gcc -g -O2 -D_POSIX_C_SOURCE -include ../cmn_headers/sal_defs.h -I. -c -Wall -Wextra -fanalyzer .\src\socket_file.c
gcc -g -O2 -D_POSIX_C_SOURCE -include ../cmn_headers/sal_defs.h -I. -c -Wall -Wextra -fanalyzer .\src\wreaddir.c
gcc -g -O2 -D_POSIX_C_SOURCE -include ../cmn_headers/sal_defs.h -I. -c -Wall -Wextra -fanalyzer .\src\wreadlink.c
gcc -g -O2 -D_POSIX_C_SOURCE -include ../cmn_headers/sal_defs.h -I. -c -Wall -Wextra -fanalyzer .\src\xstat.c
gcc -g -O2 -D_POSIX_C_SOURCE -include ../cmn_headers/sal_defs.h -I. -c -Wall -Wextra -fanalyzer .\src\locale_helpers.c
gcc -g -O2 -D_POSIX_C_SOURCE -include ../cmn_headers/sal_defs.h -I. -c -Wall -Wextra -fanalyzer -I..\libutf16 .\src\utf16cvt.c
gcc -g -O2 -D_POSIX_C_SOURCE -include ../cmn_headers/sal_defs.h -I. -c -Wall -Wextra -fanalyzer -I..\libutf16 .\src\consoleio.c
gcc -g -O2 -D_POSIX_C_SOURCE -include ../cmn_headers/sal_defs.h -I. -c -Wall -Wextra -fanalyzer -I..\libutf16 -I..\unicode_ctype .\src\utf8env.c
gcc -g -O2 -D_POSIX_C_SOURCE -include ../cmn_headers/sal_defs.h -I. -c -Wall -Wextra -fanalyzer -I..\libutf16 -I..\unicode_ctype .\src\localerpl.c
ar -crs mscrtx.a      ^
  .\arg_parser.o      ^
  .\socket_fd.o       ^
  .\socket_file.o     ^
  .\wreaddir.o        ^
  .\wreadlink.o       ^
  .\xstat.o           ^
  .\locale_helpers.o  ^
  .\utf16cvt.o        ^
  .\consoleio.o       ^
  .\utf8env.o         ^
  .\localerpl.o

MSVC:
cl /wd4464 /wd4711 /wd4820 /wd5045 /analyze /D_CRT_SECURE_NO_WARNINGS /O2 /FI..\cmn_headers\sal_defs.h /I. /c /Wall .\src\arg_parser.c
cl /wd4464 /wd4711 /wd4820 /wd5045 /analyze /D_CRT_SECURE_NO_WARNINGS /O2 /FI..\cmn_headers\sal_defs.h /I. /c /Wall .\src\socket_fd.c
cl /wd4464 /wd4711 /wd4820 /wd5045 /analyze /D_CRT_SECURE_NO_WARNINGS /O2 /FI..\cmn_headers\sal_defs.h /I. /c /Wall .\src\socket_file.c
cl /wd4464 /wd4711 /wd4820 /wd5045 /analyze /D_CRT_SECURE_NO_WARNINGS /O2 /FI..\cmn_headers\sal_defs.h /I. /c /Wall .\src\wreaddir.c
cl /wd4464 /wd4711 /wd4820 /wd5045 /analyze /D_CRT_SECURE_NO_WARNINGS /O2 /FI..\cmn_headers\sal_defs.h /I. /c /Wall .\src\wreadlink.c
cl /wd4464 /wd4711 /wd4820 /wd5045 /analyze /D_CRT_SECURE_NO_WARNINGS /O2 /FI..\cmn_headers\sal_defs.h /I. /c /Wall .\src\xstat.c
cl /wd4464 /wd4711 /wd4820 /wd5045 /analyze /D_CRT_SECURE_NO_WARNINGS /O2 /FI..\cmn_headers\sal_defs.h /I. /c /Wall .\src\locale_helpers.c
cl /wd4464 /wd4711 /wd4820 /wd5045 /analyze /D_CRT_SECURE_NO_WARNINGS /O2 /FI..\cmn_headers\sal_defs.h /I. /c /Wall -I..\libutf16 .\src\utf16cvt.c
cl /wd4464 /wd4711 /wd4820 /wd5045 /analyze /D_CRT_SECURE_NO_WARNINGS /O2 /FI..\cmn_headers\sal_defs.h /I. /c /Wall -I..\libutf16 .\src\consoleio.c
cl /wd4464 /wd4711 /wd4820 /wd5045 /analyze /D_CRT_SECURE_NO_WARNINGS /O2 /FI..\cmn_headers\sal_defs.h /I. /c /Wall -I..\libutf16 -I..\unicode_ctype .\src\utf8env.c
cl /wd4464 /wd4711 /wd4820 /wd5045 /analyze /D_CRT_SECURE_NO_WARNINGS /O2 /FI..\cmn_headers\sal_defs.h /I. /c /Wall -I..\libutf16 -I..\unicode_ctype .\src\localerpl.c
lib /out:mscrtx.a       ^
  .\arg_parser.obj      ^
  .\socket_fd.obj       ^
  .\socket_file.obj     ^
  .\wreaddir.obj        ^
  .\wreadlink.obj       ^
  .\xstat.obj           ^
  .\locale_helpers.obj  ^
  .\utf16cvt.obj        ^
  .\consoleio.obj       ^
  .\utf8env.obj         ^
  .\localerpl.obj
