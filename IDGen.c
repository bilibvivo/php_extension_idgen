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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_IDGen.h"

const long INIT_ID      = 4611686018427387904; /*pow(2, 62)*/
const long EPOCH_TIME   = 1476148258123; /* 时间基数 */
const int BITS_FULL     = 64;
const int BITS_PRE      = 2;  /* 固定位01 */
const int BITS_TIME     = 41; /* 可支持69年 */
const int BITS_SERVER   = 5;  /* 可支持31台集群服务 */
const int BITS_WORKER   = 7;  /* 可支持业务数127个 */
const int BITS_SEQUENCE = 9;  /* 一毫秒内支持生成511个id */

int server_id = 0;
int sequence_id = 0;
int worker_id = 0;
long last_timestamp = 0;

/**
 * 生成毫秒级时间
 * @Author DENGDAOCHENG
 * @return [description]
 */
long timeGen()
{
	int wait_next_ms = 0;
	long time;
	struct timeval tv;
	
	do {
		if(wait_next_ms > 0) {
			usleep(1000);
		}
		gettimeofday(&tv, NULL);
		time = tv.tv_sec * 1000 + tv.tv_usec / 1000;

		if(time > last_timestamp) {
			sequence_id = 0;
		}

		wait_next_ms++;

	} while(time == last_timestamp && sequence_id >= (pow(2, BITS_SEQUENCE) - 1) || time < last_timestamp);

	last_timestamp = time;

	return time;
}

/**
 * 二进制转换
 * @Author DENGDAOCHENG
 * @param  id           [description]
 * @return              [description]
 */
char * convert(long id)
{
	static char buffer[64];
	int i = 0;

	while(id > 0) {
		buffer[i] = id % 2;
		id = id / 2;
		i++;
	}

	buffer[i] = 0;

	return buffer;
}

/* If you declare any globals in php_IDGen.h uncomment this:
   ZEND_DECLARE_MODULE_GLOBALS(IDGen)
   */

/* True global resources - no need for thread safety here */
static int le_IDGen;

/* {{{ PHP_INI
*/
/* Remove comments and fill if you need to have entries in php.ini
   PHP_INI_BEGIN()
   STD_PHP_INI_ENTRY("IDGen.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_IDGen_globals, IDGen_globals)
   STD_PHP_INI_ENTRY("IDGen.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_IDGen_globals, IDGen_globals)
   PHP_INI_END()
   */
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_IDGen_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_IDGen_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "IDGen", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
   */

PHP_METHOD(IDGen, __construct) {
	zval *arr, **data, *self, *value;
	HashTable *arr_hash;
	HashPosition pointer;
	int array_count;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &arr) == FAILURE) {
		RETURN_NULL();
	}

	arr_hash = Z_ARRVAL_P(arr);
	//array_count = zend_hash_num_elements(arr_hash);
	//php_printf("The array passed contains %d elements \n", array_count);

	self = getThis();

	for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(arr_hash, &pointer)) {
		char *key;
		int key_len;
		long index;
		
		zend_hash_get_current_key_ex(arr_hash, &key, &key_len, &index, 0, &pointer);
		convert_to_long_ex(data);
		//php_printf("%s => ", key);
		//php_printf("%ld\n", Z_LVAL_PP(data));

		//MAKE_STD_ZVAL(value);
		//ZVAL_STRINGL(value, Z_STRVAL_PP(data), Z_STRLEN_PP(data), 0);
		//SEPARATE_ZVAL_TO_MAKE_IS_REF(&value);

		if(strcmp(key, "server_id") == 0) {
			server_id = Z_LVAL_PP(data);
			//zend_update_property(Z_OBJCE_P(self), self, ZEND_STRL("server_id"), value TSRMLS_CC);
		}
		else if(strcmp(key, "sequence_id") == 0) {
			sequence_id = Z_LVAL_PP(data);
			//zend_update_property(Z_OBJCE_P(self), self, ZEND_STRL("sequence_id"), value TSRMLS_CC);
		}	
		else if(strcmp(key, "last_timestamp") == 0) {
			last_timestamp = Z_LVAL_PP(data);
			//zend_update_property(Z_OBJCE_P(self), self, ZEND_STRL("last_timestamp"), value TSRMLS_CC);
		}
	}

	RETURN_TRUE;
}

PHP_METHOD(IDGen, __destruct) {
	//php_printf("__destruct called.\n");
}

PHP_METHOD(IDGen, get) {
	long *arg;
	int arg_len;
	unsigned long id;
	long time;
	long diff_time;
	int shift;

	int max_server_id = pow(2, BITS_SERVER) - 1;
	int max_worker_id = pow(2, BITS_WORKER) - 1;
	int max_sequence_id = pow(2, BITS_SEQUENCE) - 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &arg, &arg_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	worker_id = arg;
	time = timeGen();
	diff_time = time - EPOCH_TIME;
	
	shift = BITS_FULL - BITS_PRE - BITS_TIME;
	id = INIT_ID | diff_time << shift;

	shift -= BITS_SERVER;
	id |= (server_id & max_server_id) << shift;

	shift -= BITS_WORKER;
	id |= (worker_id & max_worker_id) << shift;

	id |= sequence_id & max_sequence_id;

	sequence_id++;

	//php_printf("%ld\n", id);
	//return id;
	
	RETURN_LONG(time);
}

PHP_METHOD(IDGen, parse) {
	long *arg;
	int arg_len;
	char *buffer;
	int p_sequence_id = 0;
	int p_worker_id = 0;
	int p_server_id = 0;
	long p_timestamp = 0;

	array_init(return_value);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &arg, &arg_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	int i;
	buffer = convert(arg);
	for (i = 0; i < 64; ++i)
	{
		//printf("%d", buffer[i]);
		if (i < BITS_SEQUENCE)
		{
			p_sequence_id += buffer[i] == 0 ? 0 : pow(2, i);
		}
		else if(i >= BITS_SEQUENCE && i < BITS_SEQUENCE + BITS_WORKER) {
			p_worker_id += buffer[i] == 0 ? 0 : pow(2, i - BITS_SEQUENCE);
		}
		else if (i >= BITS_SEQUENCE + BITS_WORKER && i < BITS_SEQUENCE + BITS_WORKER + BITS_SERVER)
		{
			p_server_id += buffer[i] == 0 ? 0 : pow(2, i - BITS_WORKER - BITS_SEQUENCE);
		}
		else if (i >= BITS_SEQUENCE + BITS_WORKER + BITS_SERVER && i < BITS_SEQUENCE + BITS_WORKER + BITS_SERVER + BITS_TIME)
		{
			p_timestamp += buffer[i] == 0 ? 0 : pow(2, i - BITS_SERVER - BITS_WORKER - BITS_SEQUENCE);
		}
	}

	p_timestamp += EPOCH_TIME;
	//p_timestamp /= 1000;

	add_assoc_long(return_value, "timestamp", p_timestamp);
	add_assoc_long(return_value, "server_id", p_server_id);
	add_assoc_long(return_value, "worker_id", p_worker_id);
	add_assoc_long(return_value, "sequence_id", p_sequence_id);
}

PHP_METHOD(IDGen, getCurrConfig) {
	array_init(return_value);

	add_assoc_long(return_value, "sequence_id", sequence_id);
	add_assoc_long(return_value, "last_timestamp", last_timestamp);
	add_assoc_long(return_value, "server_id", server_id);
}

/* {{{ php_IDGen_init_globals
*/
/* Uncomment this function if you have INI entries
   static void php_IDGen_init_globals(zend_IDGen_globals *IDGen_globals)
   {
   IDGen_globals->global_value = 0;
   IDGen_globals->global_string = NULL;
   }
   */
/* }}} */

/* {{{ IDGen_functions[]
 *  *
 *   * Every user visible function must have an entry in IDGen_functions[].
 *    */
const zend_function_entry IDGen_functions[] = {
	PHP_FE(confirm_IDGen_compiled,	NULL)		/* For testing, remove later. */
	PHP_ME(IDGen, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(IDGen, __destruct,  NULL, ZEND_ACC_PUBLIC|ZEND_ACC_DTOR)
	PHP_ME(IDGen, get, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(IDGen, parse, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(IDGen, getCurrConfig, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END	/* Must be the last line in IDGen_functions[] */
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
*/
zend_class_entry *idgen_ce;
PHP_MINIT_FUNCTION(IDGen)
{
	/* If you have INI entries, uncomment these lines 
	   REGISTER_INI_ENTRIES();
	   */
	zend_class_entry idgen; INIT_CLASS_ENTRY(idgen, "IDGen", IDGen_functions);
	idgen_ce = zend_register_internal_class_ex(&idgen, NULL, NULL TSRMLS_CC);

	zend_declare_property_null(idgen_ce, ZEND_STRL("server_id"), ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(idgen_ce, ZEND_STRL("sequence_id"), ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(idgen_ce, ZEND_STRL("last_timestamp"), ZEND_ACC_PRIVATE TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
*/
PHP_MSHUTDOWN_FUNCTION(IDGen)
{
	/* uncomment this line if you have INI entries
	   UNREGISTER_INI_ENTRIES();
	   */
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
*/
PHP_RINIT_FUNCTION(IDGen)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
*/
PHP_RSHUTDOWN_FUNCTION(IDGen)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
*/
PHP_MINFO_FUNCTION(IDGen)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "IDGen support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	   DISPLAY_INI_ENTRIES();
	   */
}
/* }}} */

/* {{{ IDGen_module_entry
*/
zend_module_entry IDGen_module_entry = {
	STANDARD_MODULE_HEADER,
	"IDGen",
	IDGen_functions,
	PHP_MINIT(IDGen),
	PHP_MSHUTDOWN(IDGen),
	PHP_RINIT(IDGen),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(IDGen),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(IDGen),
	PHP_IDGEN_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_IDGEN
ZEND_GET_MODULE(IDGen)
#endif

	/*
	 * Local variables:
	 * tab-width: 4
	 * c-basic-offset: 4
	 * End:
	 * vim600: noet sw=4 ts=4 fdm=marker
	 * vim<600: noet sw=4 ts=4
	 */
