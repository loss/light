/*
	+----------------------------------------------------------------------+
	| Light PHP Micro Framework                                            |
	+----------------------------------------------------------------------+
	|                                                                      |
	+----------------------------------------------------------------------+
	| Author: loss <loss@lightphp.com>                                     |
	+----------------------------------------------------------------------+
*/

#ifndef HAVE_LIGHT_AUTOLOADER
#define HAVE_LIGHT_AUTOLOADER

#include <php.h>
#include <php_main.h>
#include <zend_smart_str.h>
#include <ext/standard/php_string.h>
#include <ext/standard/php_var.h>

#include "autoloader.h"

zend_class_entry *light_autoloader_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(light_autoloader_ctor_arginfo, NULL, 0, 0)
	ZEND_ARG_TYPE_INFO(0, map, IS_ITERABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(light_autoloader_register_arginfo, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, prepend, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(light_autoloader_unregister_arginfo, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(light_autoloader_set_arginfo, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
	ZEND_ARG_ARRAY_INFO(0, paths, 0)
	ZEND_ARG_TYPE_INFO(0, prepend, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(light_autoloader_find_arginfo, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(light_autoloader_load_arginfo, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
ZEND_END_ARG_INFO()

zend_function_entry light_autoloader_methods[] = {
	PHP_ME(Autoloader, __construct,	light_autoloader_ctor_arginfo,				ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Autoloader, register,		light_autoloader_register_arginfo,		ZEND_ACC_PUBLIC)
	PHP_ME(Autoloader, unregister,	light_autoloader_unregister_arginfo,	ZEND_ACC_PUBLIC)
	PHP_ME(Autoloader, set,					light_autoloader_set_arginfo,					ZEND_ACC_PUBLIC)
	PHP_ME(Autoloader, find,				light_autoloader_find_arginfo,				ZEND_ACC_PUBLIC)
	PHP_ME(Autoloader, load,				light_autoloader_load_arginfo,				ZEND_ACC_PUBLIC)
	PHP_FE_END
};

int light_autoloader_register(zval *autoloader_ptr, zend_bool prepend)
{
	zval zargs[3], func, method, retval;
	int ret;

	/* bool spl_autoload_register([callable $autoload_function [, bool $throw = true [, bool $prepend = false ]]]) */
	array_init(&zargs[0]);
	ZVAL_STRING(&method, LIGHT_AUTOLOADER_METHOD_NAME);
	ZVAL_STRING(&func, LIGHT_SPL_AUTOLOAD_REGISTER_NAME);
	/* callable $autoload_function */
	zend_hash_next_index_insert(Z_ARRVAL(zargs[0]), autoloader_ptr);
	zend_hash_next_index_insert(Z_ARRVAL(zargs[0]), &method);
	/* bool $throw */
	ZVAL_BOOL(&zargs[1], IS_TRUE);
	/* bool $prepend */
	ZVAL_BOOL(&zargs[2], prepend);

	do {
		zend_fcall_info fci = { sizeof(fci), func, &retval, zargs, NULL, 1, 3 };
		ret = zend_call_function(&fci, NULL);
		zval_ptr_dtor(&func);
		zval_ptr_dtor(&method);
		zval_ptr_dtor(&retval);
		// for (i = 0; i <= fci.param_count - 1; i++)
		// 	zval_ptr_dtor(&zargs[i]);
		// efree(zargs);
		zval_ptr_dtor(&zargs[0]);
		zval_ptr_dtor(&zargs[1]);
		zval_ptr_dtor(&zargs[2]);
	} while (0);

	return ret;
}

int light_autoloader_unregister(zval *autoloader_ptr)
{
	zval zargs, func, method, retval;
	int ret;

	/* bool spl_autoload_unregister(mixed $autoload_function) */
	array_init(&zargs);
	ZVAL_STRING(&method, LIGHT_AUTOLOADER_METHOD_NAME);
	ZVAL_STRING(&func, LIGHT_SPL_AUTOLOAD_UNREGISTER_NAME);
	/* mixed $autoload_function */
	zend_hash_next_index_insert(Z_ARRVAL(zargs), autoloader_ptr);
	zend_hash_next_index_insert(Z_ARRVAL(zargs), &method);

	do {
		zend_fcall_info fci = { sizeof(fci), func, &retval, &zargs, NULL, 1, 1 };
		ret = zend_call_function(&fci, NULL);
		zval_ptr_dtor(&func);
		zval_ptr_dtor(&method);
		zval_ptr_dtor(&retval);
		zval_ptr_dtor(&zargs);
	} while (0);

	return ret;
}

int light_autoloader_insert(zend_string *prefix, zval *paths, zend_bool prepend)
{
	return 1;
}

int light_autoloader_find(zend_string *class)
{
	return 1;
}

int light_autoloader_is_file_opcached(zend_string *path)
{
	return 1;
}

int light_autoloader_import(zend_string *path)
{
	char *filepath;
	int filepath_len;
	zend_file_handle fh;
	zend_op_array *op;
	int ret;

	filepath_len = (int)spprintf(&filepath, 0, "%s", ZSTR_VAL(path));

	php_printf("import_param_path: %s\n", ZSTR_VAL(path));

	ret = php_stream_open_for_zend_ex(filepath, &fh, USE_PATH|STREAM_OPEN_FOR_INCLUDE);
	if (ret == SUCCESS) {
		zend_string *opened_path;

		if (!fh.opened_path)
			fh.opened_path = zend_string_copy(path);
		opened_path = zend_string_copy(fh.opened_path);

		php_printf("import_opened_path: %s\n", ZSTR_VAL(opened_path));

		if (zend_hash_add_empty_element(&EG(included_files), fh.opened_path)) {
			php_printf("inside zend_hash_add\n", NULL);
			//php_printf("php_lint_script: %s\n", php_lint_script(&fh) ? "SUCCESS" : "FAILURE");
			php_printf("before zend_compile_file\n", NULL);
			op = zend_compile_file(&fh, ZEND_REQUIRE);
			php_printf("before zend_destroy_file_handle\n", NULL);
			zend_destroy_file_handle(&fh);
		} else {
			php_printf("not inside zend_hash_add\n", NULL);
			op = NULL;
			zend_file_handle_dtor(&fh);
		}
		zend_string_release(opened_path);

		if (op) {
			php_printf("import_op is not null\n", NULL);
			zval result;
			ZVAL_UNDEF(&result);
			zend_try {
				zend_execute(op, &result);
				php_printf("before destroy_op_array\n", NULL);
				destroy_op_array(op);
				php_printf("before efree\n", NULL);
				efree(op);
			} zend_end_try();
			if (!EG(exception)) {
				php_printf("before zval_ptr_dtor\n", NULL);
				zval_ptr_dtor(&result);
			}
			return 1;
		}
	}
	return 0;
}

// TODO: Move this one to utils lib
void set_zval_property(zval* object, char* name, zval* val)
{
	zend_update_property(Z_OBJCE_P(object), object, name, strlen(name), val);
	if (Z_REFCOUNTED_P(val)) Z_DELREF_P(val);
}

PHP_METHOD(Autoloader, __construct)
{
	/*zval *value, *map;
	zend_string *key;
	zval prefixes;

	array_init(&prefixes);

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|A", &map) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(map) == IS_ARRAY) {
		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(map), key, value) {
			if (Z_TYPE_P(value) == IS_STRING) {
				convert_to_string(value);
				add_assoc_str(&prefixes, ZSTR_VAL(key), zend_string_dup(Z_STR_P(value), 0));
			} else if (Z_TYPE_P(value) == IS_ARRAY) {
				add_assoc_zval(&prefixes, ZSTR_VAL(key), value);
				php_var_dump(value, 10);
			}
		} ZEND_HASH_FOREACH_END();
	} else if (Z_TYPE_P(map) == IS_OBJECT) {
		// TODO: Parse iterable objects
		php_var_dump(map, 10);
	} else {
		// unknown error !
	}

	set_zval_property(getThis(), "prefixes", &prefixes);*/
}

PHP_METHOD(Autoloader, register)
{
	zend_bool prepend = 0;
	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|b", &prepend) == FAILURE) {
		return;
	}
	php_printf("prepend: %s\n", (prepend) ? "true" : "false");
	RETURN_BOOL(!light_autoloader_register(getThis(), prepend));
}

PHP_METHOD(Autoloader, unregister)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_BOOL(!light_autoloader_unregister(getThis()));
}

PHP_METHOD(Autoloader, set)
{
	RETURN_FALSE;
}

PHP_METHOD(Autoloader, find)
{
	RETURN_FALSE;
}

PHP_METHOD(Autoloader, load)
{
	zend_string *class;
	//zend_string *buf;
	zend_string *path;
	int result = 0;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "S", &class) == FAILURE) {
		return;
	}

	/*php_printf("load: %s\n", ZSTR_VAL(class));

	buf = zend_string_copy(class);

	php_strtr(ZSTR_VAL(buf), ZSTR_LEN(buf), "\\", "/", 1);
	php_printf("php_strtr: src/%s.php\n", ZSTR_VAL(buf));*/

	path = zend_string_init(ZEND_STRL("inc/Path/To/Classes/CustomClass.php"), 0);
	result = light_autoloader_import(path);
	zend_string_release(path);

	RETURN_BOOL(result);
}

// TODO: LIGHT_MINIT_FUNCTION(Autoloader)

PHP_MINIT_FUNCTION(Light_Autoloader)
{
	zend_class_entry ce;

	INIT_NS_CLASS_ENTRY(ce, "Light", "Autoloader", light_autoloader_methods);

	light_autoloader_ce = zend_register_internal_class(&ce);
	light_autoloader_ce->ce_flags |= ZEND_ACC_FINAL;

	zend_declare_property_null(light_autoloader_ce, ZEND_STRL("prefixes"), ZEND_ACC_PRIVATE);

	return SUCCESS;
}

#endif /* !HAVE_LIGHT_AUTOLOADER */