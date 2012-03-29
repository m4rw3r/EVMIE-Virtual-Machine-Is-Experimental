#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(EVMIE Virtual Machine Is Experimental, 0.1, m4rw3r@gmail.com)
AC_CONFIG_SRCDIR([src/Value.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([inttypes.h limits.h stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINTPTR_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC

# Check data-sizes and alignment as we store doubles and pointers in the same bytes
AC_CHECK_SIZEOF([double])
AC_CHECK_SIZEOF([uintptr_t])
AC_CHECK_SIZEOF([uint64_t])
AC_CHECK_ALIGNOF([double])
AC_CHECK_ALIGNOF([uintptr_t])
AC_CHECK_ALIGNOF([uint64_t])

# Validate for IEEE 754 doubles max size, so we can add to the double bitvalue
# without destroying information
AC_MSG_CHECKING([double positive infinity bit-value])
AC_RUN_IFELSE([AC_LANG_PROGRAM([[#import <stdint.h>
union Test { double dbl; int64_t bits};]], [[return ((union Test){.dbl = (1.0 / 0.0)}).bits != 0x7ff0000000000000;]])], AC_MSG_RESULT([OK]), AC_MSG_ERROR([Infinity not equal to 0x7ff0000000000000]))

AC_MSG_CHECKING([double negative infinity bit-value])
AC_RUN_IFELSE([AC_LANG_PROGRAM([[#import <stdint.h>
union Test { double dbl; int64_t bits};]], [[return ((union Test){.dbl = (-1.0 / 0.0)}).bits != 0xfff0000000000000;]])], AC_MSG_RESULT([OK]), AC_MSG_ERROR([Negative infinity not equal to 0xfff0000000000000]))

AC_MSG_CHECKING([double largest positive non-infinity bit-value])
AC_RUN_IFELSE([AC_LANG_PROGRAM([[#import <stdint.h>
union Test {double dbl; int64_t bits;};]], [[return (((union Test){.dbl = 15.95E307}).bits >= 0x7ff0000000000000) || (((union Test){.dbl = 15.95E307 + 15.95E307}).bits > 0x7ff0000000000000);]])], AC_MSG_RESULT([OK]), AC_MSG_ERROR([Large positive double value larger than infinity]))

AC_MSG_CHECKING([double largest positive non-infinity bit-value])
AC_RUN_IFELSE([AC_LANG_PROGRAM([[#import <stdint.h>
union Test {double dbl; int64_t bits;};]], [[return (((union Test){.dbl = -15.95E307}).bits >= 0xfff0000000000000) || (((union Test){.dbl = -15.95E307 - 15.95E307}).bits > 0xfff0000000000000);]])], AC_MSG_RESULT([OK]), AC_MSG_ERROR([Large negative double value larger than infinity]))

AC_MSG_CHECKING([double qNaN bit-value])
AC_RUN_IFELSE([AC_LANG_PROGRAM([[#import <stdint.h>
union Test { double dbl; int64_t bits};]], [[return ((union Test){.dbl = (0.0 / 0.0)}).bits != 0x7ff8000000000000;]])], AC_MSG_RESULT([OK]), AC_MSG_ERROR([qNaN not equal to 7ff8000000000000]))

# TODO: Tests for pointer addresses?

# Everything seems to be ok
AC_DEFINE([DOUBLE_IS_IEEE_754], [1], [Double data type approximately conforms to IEEE 754 when it comes to byte-range])

AC_OUTPUT
