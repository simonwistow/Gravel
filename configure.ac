dnl Process this file with autoconf to produce a configure script.
AC_INIT
dnl When you can seen swf_parse.c, you're in the source directory
AC_CONFIG_SRCDIR([swf_parse.c])

dnl Library versioning

dnl ====================================
dnl Check for programs
dnl ====================================

AC_PROG_CC
AC_PROG_CPP
AC_MSG_NOTICE([starting libtool test])
AC_PROG_LIBTOOL
AC_MSG_NOTICE([libtool test done])

dnl ====================================
dnl Set some general compile options
dnl ====================================

dnl ==============================
dnl Check for libraries
dnl ==============================

dnl ==============================
dnl Checks for header files
dnl ==============================
AC_CHECK_HEADERS([stdlib.h string.h])

dnl ==============================
dnl Checks for library functions
dnl ==============================
AC_CHECK_FUNCS([strdup])

dnl ==============================
dnl Checks for types
dnl ==============================

AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)

case 1 in
        $ac_cv_sizeof_char)  SWF_U8="unsigned char";    SWF_S8="signed char";
                            pSWF_U8="uc";              pSWF_S8="c";;
esac

case 2 in
        $ac_cv_sizeof_char)  SWF_U16="unsigned char";   SWF_S16="signed char";
                            pSWF_U16="uc";             pSWF_S16="c";;
        $ac_cv_sizeof_short) SWF_U16="unsigned short";  SWF_S16="signed short";
                            pSWF_U16="u";              pSWF_S16="d";;
        $ac_cv_sizeof_int)   SWF_U16="unsigned int";    SWF_S16="signed int";
                            pSWF_U16="u";              pSWF_S16="d";;
esac

case 4 in
        $ac_cv_sizeof_short) SWF_U32="unsigned short";  SWF_S32="signed short";
                            pSWF_U32="u";              pSWF_S32="d";;
        $ac_cv_sizeof_int)   SWF_U32="unsigned int";    SWF_S32="signed int";
                            pSWF_U32="u";              pSWF_S32="d";;
        $ac_cv_sizeof_long)  SWF_U32="unsigned long";   SWF_S32="signed long";
                            pSWF_U32="ul";             pSWF_S32="ld";;

esac

if test -z "$SWF_U8"; then
        AC_MSG_ERROR(No 8 bit type found on this platform!)
fi
if test -z "$SWF_U16"; then
        AC_MSG_ERROR(No 16 bit type found on this platform!)
fi
if test -z "$SWF_U32"; then
        AC_MSG_ERROR(No 32 bit type found on this platform!)
fi

AC_SUBST(CC)
AC_SUBST(SWF_U8)
AC_SUBST(SWF_S8)
AC_SUBST(SWF_U16)
AC_SUBST(SWF_S16)
AC_SUBST(SWF_U32)
AC_SUBST(SWF_S32)
AC_SUBST(pSWF_U8)
AC_SUBST(pSWF_S8)
AC_SUBST(pSWF_U16)
AC_SUBST(pSWF_S16)
AC_SUBST(pSWF_U32)
AC_SUBST(pSWF_S32)


dnl What to do with all this information

AC_CONFIG_FILES([Makefile swf_memory.h])
AC_OUTPUT

