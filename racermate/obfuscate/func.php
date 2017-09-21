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
	else if (file_exists("g:/php/funcs/env.php"))  {
	//else if (file_exists("g:\\php\\funcs\\env.php"))  {
      $funcpath = "g:\\php\\funcs\\";
	}
	else if (file_exists("/media/data/php/funcs/env.php"))  {
      $funcpath = "/media/data/php/funcs/";
	}
	else  {
		$cwd = getcwd();
		exit("oops22: $cwd\n");
	}
	require_once ($funcpath . "env.php");				// causes a crlf!!!!
	require_once ($funcpath . "save_string.php");				// causes a crlf!!!!
	require_once($funcpath . "print_array.php");
   $env = get_env();

	////////////////////////////////////////////////////////////////////////////
	// http://www.php.net/manual/en/regexp.reference.meta.php
	// http://www.regular-expressions.info/dot.html
	// http://www.regular-expressions.info/lookaround.html		(negative lookahead)
	////////////////////////////////////////////////////////////////////////////

	$original = "set_use_heartrate_sensor";

	//---------------------
	// cannondale:
	//---------------------

	$who = "canNondaLE";
	$name = "cannondale";
	$funcname =  $original . "_" . $who;
	$newfuncname = "x" . md5($funcname);
	$a = array();
	$a['original'] = $original;
	$a['encrypted'] = $newfuncname;
	// for map.h:
	printf("\n#if LEVEL >= DLL_%s_ACCESS\n", strtoupper($who));
	printf("	#define %-30s                  %s			// \n", $original, $newfuncname);
	printf("#endif\n");

	//---------------------
	// ergvideo
	//---------------------

	$who = "ERgvidEo";
	$name = "ergvideo";
	//$original = $func;
	$funcname =  $original . "_" . $who;
	$newfuncname = "x" . md5($funcname);
	$a = array();
	$a['original'] = $original;
	$a['encrypted'] = $newfuncname;
	// for map.h:
	printf("\n#if LEVEL >= DLL_%s_ACCESS\n", strtoupper($who));
	printf("	#define %-30s                  %s			// \n", $original, $newfuncname);
	printf("#endif\n");

	//---------------------
	// full
	//---------------------

	$who = "FUll";
	$name = "full";
	$funcname =  $original . "_" . $who;
	$newfuncname = "x" . md5($funcname);
	$a = array();
	$a['original'] = $original;
	$a['encrypted'] = $newfuncname;
	// for map.h:
	printf("\n#if LEVEL >= DLL_%s_ACCESS\n", strtoupper($who));
	printf("	#define %-30s                  %s			// \n", $original, $newfuncname);
	printf("#endif\n");


exit;


?>

