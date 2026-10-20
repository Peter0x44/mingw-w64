/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */

#include <_mingw.h>
#include <fenv.h>

#if defined(__i386__) || (defined(__x86_64__) && !defined(__arm64ec__))
/* FE_DFL_ENV: fesetenv recognizes it by address and calls _fpreset.  Its
   contents are what fegetenv returns after this _fpreset (x87 control
   word 0x027f, MXCSR 0x1f80), so that a copy of it means the same.  On
   other targets it is defined in fesetenv.c.  */
const fenv_t __mingw_fe_dfl_env = { 0x3f3f103f, 0 };
#endif

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
