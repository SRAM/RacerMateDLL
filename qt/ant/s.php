
<?php
	/*
	date_default_timezone_set('America/New_York');
	$script_tz = date_default_timezone_get();

	$now = date("Y-m-d-H-i-s");

	$name = $now . ".txz";
	@unlink($name);
	$cmd = "tar -v --exclude-from=excludes.txt -czf $name ./";			// makes a .gz file
	*/

	$cmd = "zip x.zip -i @files.i";

	system($cmd);

?>





