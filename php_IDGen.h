/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_IDGEN_H
#define PHP_IDGEN_H

extern zend_module_entry IDGen_module_entry;
#define phpext_IDGen_ptr &IDGen_module_entry

#define PHP_IDGEN_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_IDGEN_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_IDGEN_API __attribute__ ((visibility("default")))
#else
#	define PHP_IDGEN_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     
ZEND_BEGIN_MODULE_GLOBALS(IDGen)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(IDGen)
*/

/* In every utility function you add that needs to use variables 
   in php_IDGen_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as IDGEN_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define IDGEN_G(v) TSRMG(IDGen_globals_id, zend_IDGen_globals *, v)
#else
#define IDGEN_G(v) (IDGen_globals.v)
#endif

#endif	/* PHP_IDGEN_H */

PHP_METHOD(IDGen, __construct);
PHP_METHOD(IDGen, __destruct);
PHP_METHOD(IDGen, get);
PHP_METHOD(IDGen, parse);
PHP_METHOD(IDGen, getCurrConfig);

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
