/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */

/* Link this object to make _fpreset, and so program startup and
   fesetenv(FE_DFL_ENV), set the x87 precision to 24 bits (single
   precision) on x86.  GCC links it for -mpc32.  */

#include <_mingw.h>
#include <fenv.h>

#if defined(__i386__) || (defined(__x86_64__) && !defined(__arm64ec__))
/* FE_DFL_ENV: fesetenv recognizes it by address and calls _fpreset.  Its
   contents are what fegetenv returns after this _fpreset (x87 control
   word 0x007f, MXCSR 0x1f80), so that a copy of it means the same.  On
   other targets it is defined in fesetenv.c.  */
const fenv_t __mingw_fe_dfl_env = { 0x3f3f203f, 0 };
#endif

extern void (* __MINGW_IMP_SYMBOL(_fpreset))(void);
void _fpreset (void);

void _fpreset (void)
{
  (* __MINGW_IMP_SYMBOL(_fpreset))();
#if defined(__i386__) || (defined(__x86_64__) && !defined(__arm64ec__))
  /* The msvcrt/UCRT _fpreset does not touch the x87 unit on x86_64, so
     reset it here and set 24-bit precision (0x007f).  */
  {
    unsigned short cw = 0x007f;
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
