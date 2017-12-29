--TEST--
Check for Light presence (PHP module and Zend extension)

--SKIPIF--
<?php if (!extension_loaded("light")) print "skip"; ?>

--FILE--
<?php
	var_dump(in_array('light', get_loaded_extensions()));
	var_dump(in_array('light', get_loaded_extensions(true)));
?>

--EXPECT--
bool(true)
bool(true)
