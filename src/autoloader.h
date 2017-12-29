/*
	+----------------------------------------------------------------------+
	| Light PHP Micro Framework                                            |
	+----------------------------------------------------------------------+
	|                                                                      |
	+----------------------------------------------------------------------+
	| Author: loss <loss@lightphp.com>                                     |
	+----------------------------------------------------------------------+
*/

#ifndef HAVE_LIGHT_AUTOLOADER_H
#define HAVE_LIGHT_AUTOLOADER_H

#include <php.h>

extern zend_class_entry *light_autoloader_ce;

#define LIGHT_AUTOLOADER_METHOD_NAME				"load"
#define LIGHT_SPL_AUTOLOAD_REGISTER_NAME		"spl_autoload_register"
#define LIGHT_SPL_AUTOLOAD_UNREGISTER_NAME	"spl_autoload_unregister"

int light_autoloader_register(zval *autoloader_ptr, zend_bool prepend);
int light_autoloader_unregister(zval *autoloader_ptr);
int light_autoloader_insert(zend_string *prefix, zval *paths, zend_bool prepend);
int light_autoloader_find(zend_string *class);
int light_autoloader_is_opcached(zend_string *path);
int light_autoloader_include(zend_string *path);

// TODO: Move this one to utils lib
void set_zval_property(zval* object, char* name, zval* val);

PHP_METHOD(Autoloader, __construct);
PHP_METHOD(Autoloader, register);
PHP_METHOD(Autoloader, unregister);
PHP_METHOD(Autoloader, set);
PHP_METHOD(Autoloader, find);
PHP_METHOD(Autoloader, load);

PHP_MINIT_FUNCTION(Light_Autoloader);

#endif