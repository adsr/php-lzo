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

#ifndef PHP_LZO_H
#define PHP_LZO_H

extern zend_module_entry lzo_module_entry;
#define phpext_lzo_ptr &lzo_module_entry

#ifdef PHP_WIN32
    #define PHP_LZO_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
    #define PHP_LZO_API __attribute__ ((visibility("default")))
#else
    #define PHP_LZO_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define PHP_LZO_VERSION "0.1"

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

PHP_MINIT_FUNCTION(lzo);
PHP_MSHUTDOWN_FUNCTION(lzo);
PHP_MINFO_FUNCTION(lzo);

/**
 * The typedef below is taken from lzotest and slightly modified. It holds the
 * values contained in db.h, forming a database of compression algorithms.
 */
typedef struct
{
    const char *            name;
    lzo_uint32              mem_compress;
    lzo_uint32              mem_decompress;
    lzo_compress_t          compress;
    lzo_decompress_t        decompress;
}
compress_t;

PHP_FUNCTION(lzo_compress);
PHP_FUNCTION(lzo_decompress);

#endif    /* PHP_LZO_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
