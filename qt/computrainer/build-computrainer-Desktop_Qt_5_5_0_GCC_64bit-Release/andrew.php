
<?php
	date_default_timezone_set('America/New_York');
	$script_tz = date_default_timezone_get();

	$time = time();

	$date = date("Y-m-d H:i:s", $time);
	$hour = date("H", $time);											// starting hour = "00"
	$year = date("Y", $time);
	$month = date("m", $time);
	$day = date("d", $time);


	$cmd = "cp libcomputrainer.a /mnt/minty/home/larry/data/_fs/qt/computrainer/build-computrainer-Desktop_Qt_5_5_0_GCC_64bit-Release/libcomputrainer.a ./libs/linux",
	$a = array();
	$ret = 0;
	exec($cmd, $a, $ret);

?>

