
<?php


	date_default_timezone_set('America/New_York');
	$script_tz = date_default_timezone_get();


	$time = time();

	$date = date("Y-m-d H:i:s", $time);
	$hour = date("H", $time);											// starting hour = "00"
	$year = date("Y", $time);
	$month = date("m", $time);
	$day = date("d", $time);

	if (!file_exists("./libs/mac"))  {
		mkdir("./libs/mac");
	}
	if (!file_exists("./libs/ios"))  {
		mkdir("./libs/ios");
	}
	if (!file_exists("./libs/linux"))  {
		mkdir("./libs/linux");
	}
	if (!file_exists("./libs/android"))  {
		mkdir("./libs/android");
	}

	//----------------------------------------------
	// clean out the old libs
	//----------------------------------------------

	$cmd = "rm ./libs/mac/libcomputrainer.a";
	$a = array();
	$ret = 0;
	exec($cmd, $a, $ret);

	$cmd = "rm ./libs/ios/libcomputrainer.a";
	$a = array();
	$ret = 0;
	exec($cmd, $a, $ret);

	$cmd = "rm ./libs/linux/libcomputrainer.a";
	$a = array();
	$ret = 0;
	exec($cmd, $a, $ret);
	
	$cmd = "rm ./libs/android/libcomputrainer.a";
	$a = array();
	$ret = 0;
	exec($cmd, $a, $ret);

	//----------------------------------------------
	// clean out the header files
	//----------------------------------------------

	$cmd = "rm ./inc/*";
	$a = array();
	$ret = 0;
	exec($cmd, $a, $ret);


	$cmds = array(
				"cp /mnt/minty/home/larry/data/_fs/qt/computrainer/build-computrainer-Desktop_Qt_5_5_0_GCC_64bit-Release/libcomputrainer.a ./libs/linux",
				"cp /mnt/minty/home/larry/data/_fs/qt/computrainer/build-computrainer-Android_for_armeabi_v7a_GCC_4_9_Qt_5_5_0-Release/libcomputrainer.a ./libs/android",
				"scp larry@mac:./data/_fs/qt/computrainer/build-computrainer-Desktop_Qt_5_5_0_clang_64bit-Release/libcomputrainer.a ./libs/mac",
				"scp larry@mac:./data/_fs/qt/computrainer/build-computrainer-iphoneos_clang_Qt_5_5_0_for_iOS-Release/libcomputrainer.a ./libs/ios",
				//"scp larry@mac:./data/_fs/qt/computrainer/build-computrainer-iphonesimulator_clang_Qt_5_5_0_for_iOS-Release/libcomputrainer.a ./libs/ios_sim",
				// headers:
				"cp *.h ./inc",
			);


	$n = count($cmds);

	for($i=0; $i<$n; $i++)  {
		$cmd = $cmds[$i];
		printf("%s\n", $cmd);

		$a = array();
		$ret = 0;
		exec($cmd, $a, $ret);

	}

	/*
	foreach(glob("*.mp3") as $filename)  {
		@unlink($filename);
	}
	*/

	@unlink("update.tgz");

	$cmd = "tar cvzf update.tgz libs inc";
	$a = array();
	$ret = 0;
	exec($cmd, $a, $ret);

?>

