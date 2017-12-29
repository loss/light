--TEST--
Autoloader class

--SKIPIF--
<?php if (!extension_loaded("light")) print "skip"; ?>

--FILE--
<?php
	var_dump(in_array('Light\Autoloader', get_declared_classes()));
	var_dump(class_exists('Light\Autoloader'));

	$loader = new Light\Autoloader;
?>

--EXPECT--
bool(true)
bool(true)
