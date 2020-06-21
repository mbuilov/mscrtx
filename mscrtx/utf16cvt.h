#ifndef UTF16CVT_H_INCLUDED
#define UTF16CVT_H_INCLUDED

/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* utf16cvt.h */

/* Convert utf8->utf16, malloc'ate and return new buffer if necessary */

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(1)
A_Nonnull_arg(4)
A_Nonnull_arg(5)
A_At(str, A_In_z)
A_When(buf, A_At(buf, A_Pre_writable_size(buf_sz)))
A_At(sz, A_Inout A_Out_range(>,0))
A_At(u8sz, A_Out A_Out_range(>,0))
A_Success(return)
A_Ret_z
A_Ret_writes(*sz)
#endif
wchar_t *cvt_utf8_to_16_z_reserve(const char str[], wchar_t buf[]/*NULL?*/,
	const size_t buf_sz, size_t *const sz/*in,out*/, size_t *const u8sz/*out*/);

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(1)
A_Nonnull_arg(4)
A_At(str, A_In_z)
A_When(buf, A_At(buf, A_Pre_writable_size(buf_sz)))
A_At(sz, A_Out A_Out_range(>,0))
A_Success(return)
A_Ret_z
A_Ret_writes(*sz)
#endif
wchar_t *cvt_utf8_to_16_z_sz(const char str[], wchar_t buf[]/*NULL?*/,
	const size_t buf_sz, size_t *const sz/*out*/);

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(1)
A_At(str, A_In_z)
A_When(buf, A_At(buf, A_Pre_writable_size(buf_sz)))
A_Success(return)
A_Ret_z
#endif
wchar_t *cvt_utf8_to_16_z(const char str[], wchar_t buf[]/*NULL?*/, const size_t buf_sz);

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(1)
A_At(str, A_Pre_readable_size(*len))
A_At(len, A_Inout A_In_range(>,0))
A_When(buf, A_At(buf, A_Pre_writable_size(buf_sz)))
A_Success(return)
A_Ret_writes(*len)
#endif
wchar_t *cvt_utf8_to_16(const char str[], size_t *const len/*>0,in,out*/,
	wchar_t buf[]/*NULL?*/, const size_t buf_sz);

#define CVT_UTF8_TO_16_Z_RESERVE(str, buf, sz/*in,out*/, u8sz/*out*/) \
	cvt_utf8_to_16_z_reserve(str, buf, sizeof(buf)/sizeof(buf[0]), sz, u8sz)

#define CVT_UTF8_TO_16_Z_SZ(str, buf, sz/*out*/) \
	cvt_utf8_to_16_z_sz(str, buf, sizeof(buf)/sizeof(buf[0]), sz)

#define CVT_UTF8_TO_16_Z(str, buf) \
	cvt_utf8_to_16_z(str, buf, sizeof(buf)/sizeof(buf[0]))

#define CVT_UTF8_TO_16(str, len/*>0,in,out*/, buf) \
	cvt_utf8_to_16(str, len, buf, sizeof(buf)/sizeof(buf[0]))

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(1)
A_Nonnull_arg(4)
A_At(str, A_In_z)
A_When(buf, A_At(buf, A_Pre_writable_size(buf_sz)))
A_At(sz, A_Out A_Out_range(>,0))
A_Success(return)
A_Ret_z
A_Ret_writes(*sz)
#endif
wchar_t *cvt_utf32_to_16_z_sz(const unsigned str[], wchar_t buf[]/*NULL?*/,
	const size_t buf_sz, size_t *const sz/*out*/);

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(1)
A_At(str, A_In_z)
A_When(buf, A_At(buf, A_Pre_writable_size(buf_sz)))
A_Success(return)
A_Ret_z
#endif
wchar_t *cvt_utf32_to_16_z(const unsigned str[], wchar_t buf[]/*NULL?*/, const size_t buf_sz);

#define CVT_UTF32_TO_16_Z_SZ(str, buf, sz/*out*/) \
	cvt_utf32_to_16_z_sz(str, buf, sizeof(buf)/sizeof(buf[0]), sz)

#define CVT_UTF32_TO_16_Z(str, buf) \
	cvt_utf32_to_16_z(str, buf, sizeof(buf)/sizeof(buf[0]))

#endif /* UTF16CVT_H_INCLUDED */
