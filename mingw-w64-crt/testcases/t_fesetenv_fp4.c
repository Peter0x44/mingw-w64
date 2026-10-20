/* t_fesetenv, linked with CRT_fp4.o: on x86, _fpreset and FE_DFL_ENV use
   24-bit x87 precision.  */
#define EXPECT_X87_CW 0x007f
#include "t_fesetenv.c"
