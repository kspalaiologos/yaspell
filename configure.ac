AC_PREREQ([2.68])
AC_INIT([yaspell], [1.0.0], [https://github.com/kspalaiologos/yaspell])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign subdir-objects tar-pax color-tests])
AM_SILENT_RULES([yes])
AC_CONFIG_MACRO_DIR([build-aux])

AC_MSG_CHECKING([whether system or user specified compiler flags are set])
AM_CONDITIONAL([PASSED_CFLAGS], [test -n "$CFLAGS"])
AM_COND_IF([PASSED_CFLAGS], [AC_MSG_RESULT([yes])], [AC_MSG_RESULT([no])])

AC_ARG_ENABLE([arch-native],
			AS_HELP_STRING([--disable-arch-native], [Disable CPU-specific optimizations]))
AM_CONDITIONAL([ENABLE_ARCH_NATIVE], [test x"$enable_arch_native" != xno])

AC_ARG_ENABLE([lto],
			AS_HELP_STRING([--disable-lto], [Disable link-time optimizations]))
AM_CONDITIONAL([ENABLE_LTO], [test x"$enable_lto" != xno])

AM_COND_IF([PASSED_CFLAGS], [
	AC_MSG_NOTICE([skipping compiler feature detection, using '$CFLAGS'])
], [
    CFLAGS=""
	AX_CHECK_COMPILE_FLAG([-O3], [CFLAGS="-O3 $CFLAGS"], [])
	AX_CHECK_COMPILE_FLAG([-g3], [CFLAGS="-g3 $CFLAGS"], [])
	AC_MSG_NOTICE([using '$CFLAGS' plus compiler feature detection])

	AM_COND_IF([ENABLE_ARCH_NATIVE], [
				AX_CHECK_COMPILE_FLAG([-march=native], [CFLAGS="$CFLAGS -march=native"],
									[AC_MSG_ERROR([Compiler does not support native optimizations, use --disable-arch-native])])
				AX_CHECK_COMPILE_FLAG([-mtune=native], [CFLAGS="$CFLAGS -mtune=native"],
									[AC_MSG_ERROR([Compiler does not support native optimizations, use --disable-arch-native])])
	])

    AM_COND_IF([ENABLE_LTO], [
                AX_CHECK_COMPILE_FLAG([-flto], [CFLAGS="-flto $CFLAGS" LDFLAGS="-flto $LDFLAGS"],
                                    [AC_MSG_ERROR([Compiler does not support LTO, use --disable-lto])])
    ])
])

AC_PROG_CC([clang gcc icc])

AC_CONFIG_FILES([Makefile])
AC_ARG_PROGRAM
AC_OUTPUT