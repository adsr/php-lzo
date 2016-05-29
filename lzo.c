/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
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
#include "php_lzo.h"

#include <lzo/lzoconf.h>
#include <lzo/lzo1.h>
#include <lzo/lzo1a.h>
#include <lzo/lzo1c.h>
#include <lzo/lzo1y.h>
#include <lzo/lzo2a.h>
#include <lzo/lzo1z.h>
#include <lzo/lzo1f.h>
#include <lzo/lzo1x.h>
#include <lzo/lzo1b.h>
#include <lzo/lzo_asm.h>

#if PHP_MAJOR_VERSION >= 7
    #define COMPAT_RETVAL_STRINGL(str, len, dup) \
        RETVAL_STRINGL(str, len)
    #define COMPAT_STRLEN_DELTA 0
    #define compat_str_len_t size_t
#else
    #define COMPAT_RETVAL_STRINGL(str, len, dup) \
        RETVAL_STRINGL(str, len, dup)
    #define COMPAT_STRLEN_DELTA 1
    #define compat_str_len_t int
#endif


/**
 * Database of compression algorithms and associated metadata
 * The header file (db.h) is taken from lzotest and slightly modified
 */
static const compress_t compress_database[] = {
#include "db.h"
    { NULL, 0, 0, NULL, NULL }
};
static const int compress_database_len = sizeof(compress_database) / sizeof(compress_database[0]);

/* {{{ lzo_functions[]
 *
 * Every user visible function must have an entry in lzo_functions[].
 */
const zend_function_entry lzo_functions[] = {
    PHP_FE(lzo_compress, NULL)
    PHP_FE(lzo_decompress, NULL)
    PHP_FE_END    /* Must be the last line in lzo_functions[] */
};
/* }}} */

/* {{{ lzo_module_entry
 */
zend_module_entry lzo_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "lzo",
    lzo_functions,
    PHP_MINIT(lzo),
    PHP_MSHUTDOWN(lzo),
    NULL,
    NULL,
    PHP_MINFO(lzo),
#if ZEND_MODULE_API_NO >= 20010901
    PHP_LZO_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_LZO
ZEND_GET_MODULE(lzo)
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(lzo)
{
    int i;

    /** Init lzo */
    if (lzo_init() != LZO_E_OK) {
        return FAILURE;
    }

    /** Register constants */
    for (i = 0; compress_database[i].compress != NULL; i++) {
        zend_register_long_constant(
            compress_database[i].name,
            strlen(compress_database[i].name) + COMPAT_STRLEN_DELTA,
            i,
            CONST_CS | CONST_PERSISTENT,
            module_number
            TSRMLS_CC
        );
    }

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(lzo)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(lzo)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "lzo support", "enabled");
    php_info_print_table_header(2, "extension version", PHP_LZO_VERSION);
    php_info_print_table_header(2, "liblzo version", lzo_version_string());
    php_info_print_table_end();
}
/* }}} */

/* {{{ Compress/decompress logic
 */
static void php_lzo_do(INTERNAL_FUNCTION_PARAMETERS, int is_compress) {
    int len;
    char *data = NULL;
    compat_str_len_t data_len = 0;
    long algorithm = -1;
    const compress_t *compression;
    lzo_bytep in;
    lzo_bytep out;
    lzo_bytep workmem;
    long in_len, out_len, workmem_len;
    int rc;

    /** Parse params */
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &data, &data_len, &algorithm) == FAILURE) {
        return;
    }

    /** Determine algorithm. Default to LZO1X-1 */
    if (algorithm == -1) {
        algorithm = 0;
    } else if (algorithm < 0 || algorithm >= compress_database_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "algorithm %ld not recognized; using LZO1X-1", algorithm);
        algorithm = 0;
    }
    compression = &(compress_database[algorithm]);

    /** Allocate buffers */
    in = (lzo_bytep)data;
    in_len = (long)data_len;
    if (is_compress) {
        out_len = in_len;
        workmem = (lzo_voidp)emalloc(compression->mem_compress);
    } else {
        out_len = in_len + in_len / 8 + 128 + 3; /* This formula is taken from doc/LZO.FAQ */
        workmem = NULL; /* Decompression does not need workmem */
    }
    out = (lzo_bytep)ecalloc(out_len + 1, sizeof(lzo_bytep));
    if (!out || (is_compress && !workmem)) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "out of memory");
        if (out) efree(out);
        if (workmem) efree(workmem);
        RETURN_FALSE;
    }

    /** (De)compress */
    if (is_compress) {
        rc = compression->compress(in, in_len, out, &out_len, workmem);
    } else {
        rc = compression->decompress(in, in_len, out, &out_len, NULL);
    }
    if (rc != LZO_E_OK) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, (is_compress ? "compression failed" : "decompression failed"));
        efree(out);
        if (workmem) efree(workmem);
        RETURN_FALSE;
    }

    /** Free workmem */
    if (workmem) efree(workmem);

    /** Return (de)compress data */
    COMPAT_RETVAL_STRINGL(out, out_len, 1);

    /** Free out */
    efree(out);
}
/* }}} */

/* {{{ proto string lzo_compress(string data [, int algorithm])
   Compress a string */
PHP_FUNCTION(lzo_compress)
{
    php_lzo_do(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

/* {{{ proto string lzo_decompress(string data [, int algorithm])
   Decompress a string */
PHP_FUNCTION(lzo_decompress)
{
    php_lzo_do(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
