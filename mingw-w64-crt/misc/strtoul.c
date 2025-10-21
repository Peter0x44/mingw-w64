/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */

#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

/* Helper macros */

/* convert digit character to number, in any base */
#define ToNumber(c)	(isdigit(c) ? (c) - '0' : \
			 isupper(c) ? (c) - 'A' + 10 : \
			 islower(c) ? (c) - 'a' + 10 : \
			 -1		/* "invalid" flag */ \
			)
/* validate converted digit character for specific base */
#define valid(n, b)	((n) >= 0 && (n) < (b))

unsigned long
__cdecl
__strtoul(const char * __restrict__ nptr, char ** __restrict__ endptr, int base)
	{
	register unsigned long	accum;	/* accumulates converted value */
	register int		n;	/* numeral from digit character */
	int			minus;	/* set iff minus sign seen (yes!) */
	int			toobig;	/* set iff value overflows */

	if ( endptr != NULL )
		*endptr = (char *)nptr;	/* in case no conversion's performed */

	if ( base < 0 || base == 1 || base > 36 )
		{
		errno = EDOM;
		return 0;		/* unspecified behavior */
		}

	/* skip initial, possibly empty sequence of white-space characters */

	while ( isspace(*nptr) )
		++nptr;

	/* process subject sequence: */

	/* optional sign (yes!) */

	if ( (minus = *nptr == '-') || *nptr == '+' )
		++nptr;

	if ( base == 0 )
        {
		if ( *nptr == '0' )
            {
			if ( nptr[1] == 'X' || nptr[1] == 'x' )
				base = 16;
			else if ( nptr[1] == 'b' || nptr[1] == 'B' )
				base = 2;
			else
				base = 8;
		    }
		else
				base = 10;
		}

    /* optional "0x" or "0X" for base 16, "0b" or "0B" for base 2 */
    
	if ( base == 16 && *nptr == '0' && (nptr[1] == 'X' || nptr[1] == 'x') )
		nptr += 2;		/* skip past this prefix */
	else if ( base == 2 && *nptr == '0' && (nptr[1] == 'b' || nptr[1] == 'B') )
		nptr += 2;		/* skip past this prefix */

	/* check whether there is at least one valid digit */

	n = ToNumber(*nptr);
	++nptr;

	if ( !valid(n, base) )
		return 0;		/* subject seq. not of expected form */

	accum = n;

	for ( toobig = 0; n = ToNumber(*nptr), valid(n, base); ++nptr )
		if ( accum > ULONG_MAX / base ) 
			toobig = 1;	/* but keep scanning */
		else
			{
			accum *= base;
			if ( accum > ULONG_MAX - n )
				toobig = 1; /* but keep scanning */
			else
				accum += n;
			}

	if ( endptr != NULL )
		*endptr = (char *)nptr;	/* points to first not-valid-digit */

	if ( toobig )
		{
		errno = ERANGE;
		return ULONG_MAX;
		}
	else
		return minus ? -accum : accum;
	}

unsigned long __cdecl __strtoul(const char * __restrict__ nptr, char ** __restrict__ endptr, int base);

unsigned long __cdecl __mingw_strtoul(const char * __restrict__ nptr, char ** __restrict__ endptr, int base)
    __attribute__((alias("__strtoul")));