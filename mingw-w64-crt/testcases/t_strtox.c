/**
 * Test C23 binary prefix support (0b/0B) for strtol/wcstol families
 * This file tests the implementations in misc/strtol.c, misc/strtoul.c, 
 * misc/wcstol.c, misc/wcstoul.c and the strtoimax/strtoumax/wcstoimax/wcstoumax
 * functions with binary prefix parsing.
 */

#ifdef NDEBUG
#undef NDEBUG
#endif

#define __USE_MINGW_STRTOX 1
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <inttypes.h>
#include <limits.h>
#include <assert.h>

void test_strtol_binary(void)
{
    char *endptr;
    long result;
    
    /* Base 0 (auto-detect): Binary with 0b prefix */
    result = strtol("0b1010", &endptr, 0);
    assert(result == 10);
    assert(*endptr == '\0');
    
    /* Base 0 (auto-detect): Binary with 0B prefix (uppercase) */
    result = strtol("0B1111", &endptr, 0);
    assert(result == 15);
    
    /* Explicit base 2 with prefix */
    result = strtol("0b101010", &endptr, 2);
    assert(result == 42);
    
    /* Explicit base 2 without prefix */
    result = strtol("101010", &endptr, 2);
    assert(result == 42);
    
    /* Negative binary number */
    result = strtol("-0b1010", &endptr, 0);
    assert(result == -10);
    
    /* Positive sign binary */
    result = strtol("+0b1010", &endptr, 0);
    assert(result == 10);
    
    /* Whitespace handling */
    result = strtol("  0b1010", &endptr, 0);
    assert(result == 10);
    
    /* Invalid digit stops parsing */
    result = strtol("0b102", &endptr, 0);
    assert(result == 2);
    assert(*endptr == '2');
    
    /* Just prefix without digits */
    result = strtol("0b", &endptr, 0);
    assert(result == 0);
    
    /* Mixed with other text */
    result = strtol("0b1010xyz", &endptr, 0);
    assert(result == 10);
    assert(strcmp(endptr, "xyz") == 0);
    
    /* Binary prefix should not be recognized with base 10 */
    result = strtol("0b1010", &endptr, 10);
    assert(result == 0);
    assert(*endptr == 'b');
    
    /* Binary prefix should not be recognized with base 8 */
    result = strtol("0b1010", &endptr, 8);
    assert(result == 0);
    assert(*endptr == 'b');
}

void test_strtol_hex(void)
{
    char *endptr;
    long result;
    
    /* Base 0 (auto-detect): Hexadecimal with 0x prefix */
    result = strtol("0x1A", &endptr, 0);
    assert(result == 26);
    assert(*endptr == '\0');
    
    /* Base 0 (auto-detect): Hexadecimal with 0X prefix (uppercase) */
    result = strtol("0XFF", &endptr, 0);
    assert(result == 255);
    
    /* Explicit base 16 with prefix */
    result = strtol("0xFF", &endptr, 16);
    assert(result == 255);
    
    /* Explicit base 16 without prefix */
    result = strtol("FF", &endptr, 16);
    assert(result == 255);
    
    /* Explicit base 16 lowercase */
    result = strtol("0x7eadbeef", &endptr, 16);
    assert(result == 0x7eadbeef);
    
    /* Negative hex number */
    result = strtol("-0x10", &endptr, 0);
    assert(result == -16);
    
    /* Positive sign hex */
    result = strtol("+0xFF", &endptr, 0);
    assert(result == 255);
    
    /* Whitespace handling */
    result = strtol("\t\n0x20", &endptr, 0);
    assert(result == 32);
    
    /* Invalid digit stops parsing */
    result = strtol("0x1G", &endptr, 0);
    assert(result == 1);
    assert(*endptr == 'G');
    
    /* Just prefix without digits */
    result = strtol("0x", &endptr, 0);
    assert(result == 0);
    
    /* Mixed with other text */
    result = strtol("0xABCxyz", &endptr, 0);
    assert(result == 0xABC);
    assert(strcmp(endptr, "xyz") == 0);
    
    /* Hex prefix should not be recognized with base 10 */
    result = strtol("0x10", &endptr, 10);
    assert(result == 0);
    assert(*endptr == 'x');
}

void test_strtol_octal(void)
{
    char *endptr;
    long result;
    
    /* Base 0 (auto-detect): Octal with 0 prefix */
    result = strtol("012", &endptr, 0);
    assert(result == 10);
    
    /* Explicit base 8 */
    result = strtol("77", &endptr, 8);
    assert(result == 63);
    
    /* Explicit base 8 with 0 prefix */
    result = strtol("077", &endptr, 8);
    assert(result == 63);
    
    /* Negative octal number */
    result = strtol("-010", &endptr, 0);
    assert(result == -8);
    
    /* Invalid digit stops parsing */
    result = strtol("089", &endptr, 0);
    assert(result == 0); /* Octal doesn't have 8 or 9 */
    assert(*endptr == '8');
}

void test_strtol_decimal(void)
{
    char *endptr;
    long result;
    
    /* Base 0 (auto-detect): Decimal */
    result = strtol("42", &endptr, 0);
    assert(result == 42);
    
    /* Base 0 (auto-detect): Just "0" */
    result = strtol("0", &endptr, 0);
    assert(result == 0);
    
    /* Explicit base 10 */
    result = strtol("123", &endptr, 10);
    assert(result == 123);
}

void test_strtol_other(void)
{
    char *endptr;
    long result;
    
    /* Base 3 */
    result = strtol("102", &endptr, 3);
    assert(result == 11); /* 1*9 + 0*3 + 2 */
    
    /* Base 36 (maximum) */
    result = strtol("zz", &endptr, 36);
    assert(result == 1295); /* 35*36 + 35 */
}

void test_strtoul_binary(void)
{
    char *endptr;
    unsigned long result;
    
    result = strtoul("0b1010", &endptr, 0);
    assert(result == 10);
    
    result = strtoul("0b11111111", &endptr, 0);
    assert(result == 255);
    
    result = strtoul("0b11111111", &endptr, 2);
    assert(result == 255);
    
    result = strtoul("-0b1010", &endptr, 0);
    assert(result == (unsigned long)-10);
}

void test_strtoul_hex(void)
{
    char *endptr;
    unsigned long result;
    
    result = strtoul("0xFFFFFFFF", &endptr, 0);
    assert(result == 0xFFFFFFFF);
    
    result = strtoul("DEADBEEF", &endptr, 16);
    assert(result == 0xDEADBEEF);
    
    result = strtoul("-0xFF", &endptr, 0);
    assert(result == (unsigned long)-255);
}

void test_strtoul_octal(void)
{
    char *endptr;
    unsigned long result;
    
    result = strtoul("0777", &endptr, 0);
    assert(result == 511);
    
    result = strtoul("377", &endptr, 8);
    assert(result == 255);
}

void test_strtoul_decimal(void)
{
    char *endptr;
    unsigned long result;
    
    result = strtoul("4294967295", &endptr, 10);
    assert(result == 4294967295UL);
    
    /* Negative sign wraps around for unsigned */
    result = strtoul("-1", &endptr, 10);
    assert(result == ULONG_MAX);
}

void test_wcstol_binary(void)
{
    wchar_t *endptr;
    long result;
    
    result = wcstol(L"0b1010", &endptr, 0);
    assert(result == 10);
    assert(*endptr == L'\0');
    
    result = wcstol(L"0B1111", &endptr, 0);
    assert(result == 15);
    
    result = wcstol(L"0b1111", &endptr, 2);
    assert(result == 15);
    
    result = wcstol(L"-0b1010", &endptr, 0);
    assert(result == -10);
    
    result = wcstol(L"  0b101", &endptr, 0);
    assert(result == 5);
}

void test_wcstol_hex(void)
{
    wchar_t *endptr;
    long result;
    
    result = wcstol(L"0x1A", &endptr, 0);
    assert(result == 26);
    
    result = wcstol(L"0XFF", &endptr, 0);
    assert(result == 255);
    
    result = wcstol(L"7EADBEEF", &endptr, 16);
    assert(result == 0x7eadbeef);
    
    result = wcstol(L"-0xFF", &endptr, 0);
    assert(result == -255);
    
    result = wcstol(L"\t0x20", &endptr, 0);
    assert(result == 32);
}

void test_wcstol_octal(void)
{
    wchar_t *endptr;
    long result;
    
    result = wcstol(L"012", &endptr, 0);
    assert(result == 10);
    
    result = wcstol(L"077", &endptr, 8);
    assert(result == 63);
    
    result = wcstol(L"-010", &endptr, 0);
    assert(result == -8);
}

void test_wcstol_decimal(void)
{
    wchar_t *endptr;
    long result;
    
    result = wcstol(L"42", &endptr, 0);
    assert(result == 42);
    
    result = wcstol(L"12345", &endptr, 10);
    assert(result == 12345);
}

void test_wcstoul_binary(void)
{
    wchar_t *endptr;
    unsigned long result;
    
    result = wcstoul(L"0b1010", &endptr, 0);
    assert(result == 10);
    
    result = wcstoul(L"0b11111111", &endptr, 0);
    assert(result == 255);
    
    result = wcstoul(L"0b101010", &endptr, 2);
    assert(result == 42);
}

void test_wcstoul_hex(void)
{
    wchar_t *endptr;
    unsigned long result;
    
    result = wcstoul(L"0xFFFFFFFF", &endptr, 0);
    assert(result == 0xFFFFFFFF);
    
    result = wcstoul(L"FFFFFFFF", &endptr, 16);
    assert(result == 0xFFFFFFFF);
}

void test_wcstoul_octal(void)
{
    wchar_t *endptr;
    unsigned long result;
    
    result = wcstoul(L"0777", &endptr, 0);
    assert(result == 511);
}

void test_wcstoul_decimal(void)
{
    wchar_t *endptr;
    unsigned long result;
    
    result = wcstoul(L"4294967295", &endptr, 10);
    assert(result == 4294967295UL);
    
    result = wcstoul(L"-1", &endptr, 10);
    assert(result == ULONG_MAX);
}

void test_strtoimax_binary(void)
{
    char *endptr;
    intmax_t result;
    
    result = strtoimax("0b1010", &endptr, 0);
    assert(result == 10);
    
    result = strtoimax("0B1111", &endptr, 0);
    assert(result == 15);
    
    result = strtoimax("0b111111", &endptr, 2);
    assert(result == 63);
    
    result = strtoimax("-0b1010", &endptr, 0);
    assert(result == -10);
}

void test_strtoimax_hex(void)
{
    char *endptr;
    intmax_t result;
    
    result = strtoimax("0x7FFFFFFF", &endptr, 0);
    assert(result == 0x7FFFFFFF);
    
    result = strtoimax("DEADBEEF", &endptr, 16);
    assert(result == 0xDEADBEEF);
    
    result = strtoimax("-0xFF", &endptr, 0);
    assert(result == -255);
}

void test_strtoimax_octal(void)
{
    char *endptr;
    intmax_t result;
    
    result = strtoimax("0777", &endptr, 0);
    assert(result == 511);
}

void test_strtoimax_decimal(void)
{
    char *endptr;
    intmax_t result;
    
    result = strtoimax("123456789", &endptr, 10);
    assert(result == 123456789);
}

void test_strtoll_binary(void)
{
    char *endptr;
    long long result;

    result = strtoll("0b1010", &endptr, 0);
    assert(result == 10LL);
    assert(*endptr == '\0');

    result = strtoll("0B1111", &endptr, 0);
    assert(result == 15LL);

    result = strtoll("0b101010", &endptr, 2);
    assert(result == 42LL);

    result = strtoll("-0b1010", &endptr, 0);
    assert(result == -10LL);

    result = strtoll("+0b1010", &endptr, 0);
    assert(result == 10LL);

    result = strtoll("  0b1010", &endptr, 0);
    assert(result == 10LL);

    result = strtoll("0b102", &endptr, 0);
    assert(result == 2LL);
    assert(*endptr == '2');

    result = strtoll("0b", &endptr, 0);
    assert(result == 0LL);

    result = strtoll("0b1010xyz", &endptr, 0);
    assert(result == 10LL);
    assert(strcmp(endptr, "xyz") == 0);
}

void test_strtoll_hex(void)
{
    char *endptr;
    long long result;

    result = strtoll("0x7FFFFFFFFFFFFFFF", &endptr, 0);
    __builtin_printf("%lld\n", result);
    assert(result == 0x7FFFFFFFFFFFFFFFLL);

    result = strtoll("DEADBEEF", &endptr, 16);
    assert(result == 0xDEADBEEFLL);

    result = strtoll("-0x10", &endptr, 0);
    assert(result == -16LL);
}

void test_strtoll_octal(void)
{
    char *endptr;
    long long result;

    result = strtoll("012", &endptr, 0);
    assert(result == 10LL);

    result = strtoll("077", &endptr, 8);
    assert(result == 63LL);
}

void test_strtoll_decimal(void)
{
    char *endptr;
    long long result;

    result = strtoll("123456789", &endptr, 10);
    assert(result == 123456789LL);
}

void test_strtoull_binary(void)
{
    char *endptr;
    unsigned long long result;

    result = strtoull("0b1010", &endptr, 0);
    assert(result == 10ULL);

    result = strtoull("0b11111111", &endptr, 0);
    assert(result == 255ULL);

    result = strtoull("0b11111111", &endptr, 2);
    assert(result == 255ULL);

    result = strtoull("-0b1010", &endptr, 0);
    assert(result == (unsigned long long)-10);
}

void test_strtoull_hex(void)
{
    char *endptr;
    unsigned long long result;

    result = strtoull("0xFFFFFFFFFFFFFFFF", &endptr, 0);
    assert(result == 0xFFFFFFFFFFFFFFFFULL);

    result = strtoull("DEADBEEF", &endptr, 16);
    assert(result == 0xDEADBEEFULL);
}

void test_strtoull_octal(void)
{
    char *endptr;
    unsigned long long result;

    result = strtoull("0777", &endptr, 0);
    assert(result == 511ULL);
}

void test_strtoull_decimal(void)
{
    char *endptr;
    unsigned long long result;

    result = strtoull("18446744073709551615", &endptr, 10);
    assert(result == 18446744073709551615ULL);
}

void test_strtoumax_binary(void)
{
    char *endptr;
    uintmax_t result;
    
    result = strtoumax("0b1010", &endptr, 0);
    assert(result == 10);
    
    result = strtoumax("0b1111111111111111", &endptr, 0);
    assert(result == 65535);
    
    result = strtoumax("0b11111111", &endptr, 2);
    assert(result == 255);
}

void test_strtoumax_hex(void)
{
    char *endptr;
    uintmax_t result;
    
    result = strtoumax("0xFFFFFFFF", &endptr, 0);
    assert(result == 0xFFFFFFFF);
    
    result = strtoumax("FFFFFFFFFFFFFFFF", &endptr, 16);
    assert(result == 0xFFFFFFFFFFFFFFFFULL);
}

void test_strtoumax_octal(void)
{
    char *endptr;
    uintmax_t result;
    
    result = strtoumax("01777", &endptr, 0);
    assert(result == 1023);
}

void test_strtoumax_decimal(void)
{
    char *endptr;
    uintmax_t result;
    
    result = strtoumax("18446744073709551615", &endptr, 10);
    assert(result == UINTMAX_MAX);
}

void test_wcstoimax_binary(void)
{
    wchar_t *endptr;
    intmax_t result;
    
    result = wcstoimax(L"0b1010", &endptr, 0);
    assert(result == 10);
    
    result = wcstoimax(L"0b101010", &endptr, 2);
    assert(result == 42);
    
    result = wcstoimax(L"-0b1010", &endptr, 0);
    assert(result == -10);
}

void test_wcstoimax_hex(void)
{
    wchar_t *endptr;
    intmax_t result;
    
    result = wcstoimax(L"0x7FFFFFFF", &endptr, 0);
    assert(result == 0x7FFFFFFF);
    
    result = wcstoimax(L"ABCDEF", &endptr, 16);
    assert(result == 0xABCDEF);
    
    result = wcstoimax(L"-0x100", &endptr, 0);
    assert(result == -256);
}

void test_wcstoimax_octal(void)
{
    wchar_t *endptr;
    intmax_t result;
    
    result = wcstoimax(L"0777", &endptr, 0);
    assert(result == 511);
}

void test_wcstoimax_decimal(void)
{
    wchar_t *endptr;
    intmax_t result;
    
    result = wcstoimax(L"987654321", &endptr, 10);
    assert(result == 987654321);
}

void test_wcstoumax_binary(void)
{
    wchar_t *endptr;
    uintmax_t result;
    
    result = wcstoumax(L"0b1010", &endptr, 0);
    assert(result == 10);
    
    result = wcstoumax(L"0b11111111", &endptr, 0);
    assert(result == 255);
    
    result = wcstoumax(L"0b111111", &endptr, 2);
    assert(result == 63);
}

void test_wcstoumax_hex(void)
{
    wchar_t *endptr;
    uintmax_t result;
    
    result = wcstoumax(L"0xFFFFFFFF", &endptr, 0);
    assert(result == 0xFFFFFFFF);
    
    result = wcstoumax(L"FFFFFFFF", &endptr, 16);
    assert(result == 0xFFFFFFFF);
}

void test_wcstoumax_octal(void)
{
    wchar_t *endptr;
    uintmax_t result;
    
    result = wcstoumax(L"01777", &endptr, 0);
    assert(result == 1023);
}

void test_wcstoumax_decimal(void)
{
    wchar_t *endptr;
    uintmax_t result;
    
    result = wcstoumax(L"18446744073709551615", &endptr, 10);
    assert(result == UINTMAX_MAX);
}

int main(void)
{
    test_strtol_binary();
    test_strtol_hex();
    test_strtol_octal();
    test_strtol_decimal();
    test_strtol_other();
    
    test_strtoul_binary();
    test_strtoul_hex();
    test_strtoul_octal();
    test_strtoul_decimal();
    
    test_wcstol_binary();
    test_wcstol_hex();
    test_wcstol_octal();
    test_wcstol_decimal();
    
    test_wcstoul_binary();
    test_wcstoul_hex();
    test_wcstoul_octal();
    test_wcstoul_decimal();

    test_strtoll_binary();
    test_strtoll_hex();
    test_strtoll_octal();
    test_strtoll_decimal();
    
    test_strtoull_binary();
    test_strtoull_hex();
    test_strtoull_octal();
    test_strtoull_decimal();

    test_strtoimax_binary();
    test_strtoimax_hex();
    test_strtoimax_octal();
    test_strtoimax_decimal();
    
    test_strtoumax_binary();
    test_strtoumax_hex();
    test_strtoumax_octal();
    test_strtoumax_decimal();
    
    test_wcstoimax_binary();
    test_wcstoimax_hex();
    test_wcstoimax_octal();
    test_wcstoimax_decimal();
    
    test_wcstoumax_binary();
    test_wcstoumax_hex();
    test_wcstoumax_octal();
    test_wcstoumax_decimal();
    
    return 0;
}
