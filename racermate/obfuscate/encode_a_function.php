<?php

	if (file_exists("/usr/local/www/apache22/mydata/funcs/env.php"))  {		// absolute pathe required for cron jobs
		$funcpath = "/usr/local/www/apache22/mydata/funcs/";
	}
	else if (file_exists("/media/data/php/func_copy_from_www/env.php"))  {
      $funcpath = "/media/data/php/func_copy_from_www/";
	}
	else if (file_exists("g:/php/func_copy_from_www/env.php"))  {
      $funcpath = "g:/php/func_copy_from_www/";
	}
	else if (file_exists("../../funcs/env.php"))  {
      $funcpath = "../../funcs/";
	}
	else if (file_exists("../funcs/env.php"))  {
      $funcpath = "../funcs/";
	}
	else if (file_exists("d:/php/funcs/env.php"))  {
	//else if (file_exists("g:\\php\\funcs\\env.php"))  {
      $funcpath = "d:\\php\\funcs\\";
	}
	else if (file_exists("/media/larry/data/php/funcs/env.php"))  {
      $funcpath = "/media/larry/data/php/funcs/";
	}
	else  {
		$cwd = getcwd();
		exit("oops22: $cwd\n");
	}
	require_once ($funcpath . "env.php");				// causes a crlf!!!!
	require_once ($funcpath . "save_string.php");				// causes a crlf!!!!
	require_once($funcpath . "print_array.php");
   $env = get_env();

	define("DLL_FULL_ACCESS",			65536	);
	define("DLL_ERGVIDEO_ACCESS",		32768	);
	define("DLL_trinerd_ACCESS",		24000	);
	define("DLL_CANNONDALE_ACCESS",	16384	);

	//$funcname = "GetAPIVersion";				// x77ae24db08410418b007f2f78197b12f
	$funcname = "GetPortNames";					// x7a3f1a877cbf4ac500ff32a493d9ef51

	$name = "ergvideo";

	if ($name=="cannondale")  {
		$who = "canNondaLE";
	}
	else if ($name=="trinerd")  {
		$who = "trinerd";
	}
	else if ($name=="ergvideo")  {
		$who = "";
	}
	else if ($name=='full')  {
		$who = "FUlL";
	}

	$old_funcname =  $funcname . "_" . $who;
	$newfuncname = "x" . md5($old_funcname);

	$newfuncname = "x" . md5($funcname);

	printf("%s  --->  %s\n", $funcname, $newfuncname);

exit;

?>

