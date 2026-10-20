/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */

#include <_mingw.h>

extern void (* __MINGW_IMP_SYMBOL(_fpreset))(void);
void _fpreset (void);

void _fpreset (void)
{
  (* __MINGW_IMP_SYMBOL(_fpreset))();
#if defined(__i386__) || (defined(__x86_64__) && !defined(__arm64ec__))
  /* On x86_64 the msvcrt/UCRT _fpreset only resets MXCSR and does not
     touch the x87 unit, so reset it here as well, with the 53-bit
     precision (0x027f) that _fpreset sets on i386.  */
  {
    unsigned short cw = 0x027f;
#ifdef __GNUC__
    __asm__ __volatile__ ("fninit\n\tfldcw %0" : : "m" (cw));
#else /* msvc: */
    __asm fninit
    __asm fldcw cw
#endif
  }
#endif
}

#ifdef __GNUC__
void __attribute__ ((alias ("_fpreset"))) fpreset(void);
#else
void fpreset(void) {
    _fpreset();
}
#endif
