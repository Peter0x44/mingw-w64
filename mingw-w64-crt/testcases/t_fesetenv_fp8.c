/* t_fesetenv, linked with CRT_fp8.o: on x86, _fpreset and FE_DFL_ENV use
   53-bit x87 precision.  */
#define EXPECT_X87_CW 0x027f
#include "t_fesetenv.c"
