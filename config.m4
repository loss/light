PHP_ARG_ENABLE(light, whether to enable Light support, [  --enable-light           Enable Light support])

if test "$PHP_LIGHT" != "no"; then
	PHP_ADD_BUILD_DIR($ext_builddir)
	PHP_NEW_EXTENSION(light, php_light.c src/autoloader.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1,, yes)
fi
