dnl $Id$
dnl config.m4 for extension IDGen

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(IDGen, for IDGen support,
Make sure that the comment is aligned:
[  --with-IDGen             Include IDGen support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(IDGen, whether to enable IDGen support,
dnl Make sure that the comment is aligned:
dnl [  --enable-IDGen           Enable IDGen support])

if test "$PHP_IDGEN" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-IDGen -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/IDGen.h"  # you most likely want to change this
  dnl if test -r $PHP_IDGEN/$SEARCH_FOR; then # path given as parameter
  dnl   IDGEN_DIR=$PHP_IDGEN
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for IDGen files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       IDGEN_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$IDGEN_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the IDGen distribution])
  dnl fi

  dnl # --with-IDGen -> add include path
  dnl PHP_ADD_INCLUDE($IDGEN_DIR/include)

  dnl # --with-IDGen -> check for lib and symbol presence
  dnl LIBNAME=IDGen # you may want to change this
  dnl LIBSYMBOL=IDGen # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $IDGEN_DIR/$PHP_LIBDIR, IDGEN_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_IDGENLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong IDGen lib version or lib not found])
  dnl ],[
  dnl   -L$IDGEN_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(IDGEN_SHARED_LIBADD)

  PHP_NEW_EXTENSION(IDGen, IDGen.c, $ext_shared)
fi
