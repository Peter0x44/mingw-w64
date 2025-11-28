/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
/*
    This source code was extracted from the Q8 package created and
    placed in the PUBLIC DOMAIN by Doug Gwyn <gwyn@arl.mil>
    last edit:	1999/11/05	gwyn@arl.mil

	Implements subclause 7.8.2 of ISO/IEC 9899:1999 (E).

	This particular implementation requires the matching <inttypes.h>.
	It also assumes that character codes for A..Z and a..z are in
	contiguous ascending order; this is true for ASCII but not EBCDIC.
*/
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <inttypes.h>
#include <wchar.h>
#include <wctype.h>
#include <stdio.h>

/* Function prototypes */
intmax_t __cdecl __mingw_strtoimax(const char * __restrict__ nptr, char ** __restrict__ endptr, int base);
uintmax_t __cdecl __mingw_strtoumax(const char * __restrict__ nptr, char ** __restrict__ endptr, int base);
intmax_t __cdecl __mingw_wcstoimax(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr, int base);
uintmax_t __cdecl __mingw_wcstoumax(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr, int base);
long __cdecl __mingw_strtol(const char * __restrict__ nptr, char ** __restrict__ endptr, int base);
unsigned long __cdecl __mingw_strtoul(const char * __restrict__ nptr, char ** __restrict__ endptr, int base);
long long __cdecl __mingw_strtoll(const char * __restrict__ nptr, char ** __restrict__ endptr, int base);
unsigned long long __cdecl __mingw_strtoull(const char * __restrict__ nptr, char ** __restrict__ endptr, int base);
long __cdecl __mingw_wcstol(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr, int base);
unsigned long __cdecl __mingw_wcstoul(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr, int base);
long long __cdecl __mingw_wcstoll(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr, int base);
unsigned long long __cdecl __mingw_wcstoull(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr, int base);

/* Helper macros */

/* convert digit character to number, in any base */
#define ToNumber(c)	(isdigit(c) ? (c) - '0' : \
			 isupper(c) ? (c) - 'A' + 10 : \
			 islower(c) ? (c) - 'a' + 10 : \
			 -1		/* "invalid" flag */ \
			)
/* validate converted digit character for specific base */
#define valid(n, b)	((n) >= 0 && (n) < (b))

/* Wide char versions */
#define ToNumberW(c)	(iswdigit(c) ? (c) - L'0' : \
			 iswupper(c) ? (c) - L'A' + 10 : \
			 iswlower(c) ? (c) - L'a' + 10 : \
			 -1		/* "invalid" flag */ \
			)
#define validW(n, b)	((n) >= 0 && (n) < (b))

/* Generic strtox body macro */
#define STRTOX_BODY(signed_flag, type, utype, char_type, to_num, valid_func, isspace_func) \
	do { \
		if (sizeof(char_type) == sizeof(char)) \
			fprintf(stderr, "STRTOX_BODY called: input='%s' initial_base=%d\n", (const char *)nptr, base); \
		register utype accum; \
		register int n; \
		int minus = 0; \
		int toobig = 0; \
 \
		if (endptr != NULL) \
			*endptr = (char_type *)nptr; \
 \
		if (base < 0 || base == 1 || base > 36) { \
			errno = EDOM; \
			return 0; \
		} \
 \
		while (isspace_func(*nptr)) \
			++nptr; \
 \
		if (signed_flag) { \
			switch (*nptr) { \
			case '-': \
				minus = 1; \
				__attribute__((fallthrough)); \
			case '+': \
				++nptr; \
				break; \
			} \
		} \
 \
		if (base == 0 || base == 2) { \
			if (*nptr == '0') { \
				++nptr; \
				if (*nptr == 'b' || *nptr == 'B') { \
					++nptr; \
					if (base == 0) \
						base = 2; \
				} else { \
					--nptr; \
				} \
			} \
			if (sizeof(char_type) == sizeof(char)) \
				fprintf(stderr, "after binary-check: ptr='%s' base=%d\n", (const char *)nptr, base); \
		} \
 \
		if (base == 0) { \
			if (*nptr == '0') { \
				++nptr; \
				if (*nptr == 'x' || *nptr == 'X') { \
					base = 16; \
					++nptr; \
				} else { \
					base = 8; \
				} \
			} else { \
				base = 10; \
			} \
			if (sizeof(char_type) == sizeof(char)) \
				fprintf(stderr, "after hex/octal-check: ptr='%s' base=%d\n", (const char *)nptr, base); \
		} \
 \
		accum = 0; \
		for (;;) { \
			n = to_num(*nptr); \
			if (!valid_func(n, base)) \
				break; \
			if (signed_flag) { \
				if (accum > (utype)(~(type)0) / (utype)base) \
					toobig = 1; \
			} else { \
				if (accum > ~(utype)0 / (utype)base) \
					toobig = 1; \
			} \
			if (!toobig) \
				accum = accum * (utype)base + (utype)n; \
			++nptr; \
		} \
 \
		if (toobig) { \
			errno = ERANGE; \
			if (endptr != NULL) \
				*endptr = (char_type *)nptr; \
			if (signed_flag) \
				return minus ? (type)((utype)1 << (sizeof(type)*8 - 1)) : (type)~((utype)1 << (sizeof(type)*8 - 1)); \
			else \
				return ~(utype)0; \
		} \
 \
		if (endptr != NULL) \
			*endptr = (char_type *)nptr; \
 \
		if (signed_flag) \
			return minus ? -(type)accum : (type)accum; \
		else \
			return accum; \
	} while (0)

/* Implementations using the generic body */
intmax_t __cdecl __mingw_strtoimax(const char * __restrict__ nptr, char ** __restrict__ endptr, int base)
{
	STRTOX_BODY(1, intmax_t, uintmax_t, char, ToNumber, valid, isspace);
}

long long __attribute__ ((alias ("__mingw_strtoimax")))
__cdecl
__mingw_strtoll (const char* __restrict__ nptr, char ** __restrict__ endptr, int base);

uintmax_t __cdecl __mingw_strtoumax(const char * __restrict__ nptr, char ** __restrict__ endptr, int base)
{
	STRTOX_BODY(0, uintmax_t, uintmax_t, char, ToNumber, valid, isspace);
}

unsigned long long __attribute__ ((alias ("__mingw_strtoumax")))
__cdecl
__mingw_strtoull (const char* __restrict__ nptr, char ** __restrict__ endptr, int base);

intmax_t __cdecl __mingw_wcstoimax(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr, int base)
{
	STRTOX_BODY(1, intmax_t, uintmax_t, wchar_t, ToNumberW, validW, iswspace);
}

long long __attribute__ ((alias ("__mingw_wcstoimax")))
__cdecl
__mingw_wcstoll (const wchar_t* __restrict__ nptr, wchar_t ** __restrict__ endptr, int base);

uintmax_t __cdecl __mingw_wcstoumax(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr, int base)
{
	STRTOX_BODY(0, uintmax_t, uintmax_t, wchar_t, ToNumberW, validW, iswspace);
}

unsigned long long __attribute__ ((alias ("__mingw_wcstoumax")))
__cdecl
__mingw_wcstoull (const wchar_t* __restrict__ nptr, wchar_t ** __restrict__ endptr, int base);

long __cdecl __mingw_strtol(const char * __restrict__ nptr, char ** __restrict__ endptr, int base)
{
	STRTOX_BODY(1, long, unsigned long, char, ToNumber, valid, isspace);
}

unsigned long __cdecl __mingw_strtoul(const char * __restrict__ nptr, char ** __restrict__ endptr, int base)
{
	STRTOX_BODY(0, unsigned long, unsigned long, char, ToNumber, valid, isspace);
}

long __cdecl __mingw_wcstol(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr, int base)
{
	STRTOX_BODY(1, long, unsigned long, wchar_t, ToNumberW, validW, iswspace);
}

unsigned long __cdecl __mingw_wcstoul(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr, int base)
{
	STRTOX_BODY(0, unsigned long, unsigned long, wchar_t, ToNumberW, validW, iswspace);
}