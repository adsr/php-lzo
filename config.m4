PHP_ARG_WITH(lzo, for lzo support,
[  --with-lzo             Include lzo support])

if test "$PHP_LZO" != "no"; then
  dnl
  dnl Find header files
  dnl
  SEARCH_PATH="/usr/local /usr"
  SEARCH_FOR="/include/lzo/lzoconf.h"  
  if test -r $PHP_LZO/$SEARCH_FOR; then
    LZO_DIR=$PHP_LZO
  else
    AC_MSG_CHECKING([for lzo files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        LZO_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  if test -z "$LZO_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please install lzo development files])
  fi
  PHP_ADD_INCLUDE($LZO_DIR/include)

  dnl
  dnl Check library
  dnl
  LIBNAME=lzo2
  LIBSYMBOL=lzo_version
  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $LZO_DIR/lib, LZO_SHARED_LIBADD)
    AC_DEFINE(HAVE_LZOLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong lzo lib version or lib not found])
  ],[
    -L$LZO_DIR/lib -lm
  ])
  PHP_SUBST(LZO_SHARED_LIBADD)

  PHP_NEW_EXTENSION(lzo, lzo.c, $ext_shared)
fi
