/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */

#include <fenv.h>

/* FE_DFL_ENV: fesetenv recognizes it by address and calls _fpreset.  Its
   contents are what fegetenv returns after this _fpreset (x87 control
   word 0x037f, MXCSR 0x1f80), so that a copy of it means the same.  It is
   defined next to _fpreset so that linking CRT_fp8.o changes both.  */
const fenv_t __mingw_fe_dfl_env = { 0x3f3f003f, 0 };

void _fpreset (void);

void _fpreset (void)
{
#ifdef __GNUC__
  __asm__ ("fninit");
#else /* msvc: */
  __asm fninit;
#endif
}

#ifdef __GNUC__
void __attribute__ ((alias ("_fpreset"))) fpreset(void);
#else
void fpreset(void) {
    _fpreset();
}
#endif
