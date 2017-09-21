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
	require_once ($funcpath . "env.php");
	require_once ($funcpath . "save_string.php");
	require_once($funcpath . "print_array.php");
   $env = get_env();

	////////////////////////////////////////////////////////////////////////////
	// http://www.php.net/manual/en/regexp.reference.meta.php
	// http://www.regular-expressions.info/dot.html
	// http://www.regular-expressions.info/lookaround.html		(negative lookahead)
	////////////////////////////////////////////////////////////////////////////

	// functions for trifriends

	$trinerds = array(
		"SetErgModeLoad",
		"is_connected",
		"start_trainer",
		"SetHRBeepBounds",
		"GetHandleBarButtons",
		"SetRecalibrationMode",
		"EndRecalibrationMode",
		"ResetALLtoIdle",
		"get_accum_tdc",
		"get_tdc",
		"SetVelotronParameters",
		"GetCurrentVTGear",
		"setGear",
		"get_bars",
		"get_calories",
		"get_np",
		"get_if",
		"get_tss",
		"get_pp",
		"set_ftp",
		"set_wind",
		"set_draftwind",
		"update_velotron_current",
		"set_velotron_calibration",
		"velotron_calibration_spindown",
		"get_status_bits",
		"set_use_speed_cadence_sensor",
		"Setlogfilepath",
		"Enablelogs",
		"GetFirmWareVersion",
		"get_dll_version",
		"GetAPIVersion",
		"check_for_trainers",
		"GetRacerMateDeviceID",
		"GetIsCalibrated",
		"GetCalibration",
		"get_errstr",
		"resetTrainer",
		"startTrainer",
		"ResetAverages",
		"GetTrainerData",
		"get_average_bars",
		"get_ss_data",
		"SetSlope",
		"setPause",
		"stopTrainer",
		"ResettoIdle",
		"set_use_heartrate_sensor"
	);

	//------------------------------
	// trinerds:
	//------------------------------

	$enc = array();

	$n = count($trinerds);

	for($i=0; $i<$n; $i++)  {

		$original = $trinerds[$i];

		$who = "tRINErdS";
		$name = "trinerds";
		//$original = $func;
		$funcname =  $original . "_" . $who;
		$newfuncname = "x" . md5($funcname);
		$a = array();
		$a['original'] = $original;
		$a['encrypted'] = $newfuncname;
		// for map.h:
		//printf("#define %-30s                  %s			// \n", $original, $newfuncname);

		array_push($enc, $a);
	}

	
	for($i=0; $i<$n; $i++)  {
		$encrypted = $enc[$i]['encrypted'];
		$original = $enc[$i]['original'];
		printf("\n#elif LEVEL >= DLL_%s_ACCESS\n", strtoupper($who));
		printf("%s\n", $encrypted);
		printf("				// %s\n", $original);
		//printf("#endif\n");

		$bp = $i;
	}
exit;


?>

