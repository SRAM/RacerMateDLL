<?php

	date_default_timezone_set('America/New_York');
	$script_tz = date_default_timezone_get();

	$contents = file_get_contents("../non_extended.h");

	/*
	//$b = preg_match("/#ifndef OBFUSCATE(.*?)#else/", $contents, $matches);
	$n = preg_match("/#ifndef OBFUSCATE(.*?)#else/", $contents, $matches);
	if ($n)  {
		printf("%s\n", $matches[0]);
	}
	*/
	$pos = strpos($contents, "		#ifndef OBFUSCATE");

	if ($pos)  {
		$s = substr($contents, $pos);
		//printf("%s\n", $s);
		$pos2 = strpos($s, "		#else");
		if ($pos2)  {
			$pos3 = $pos2 + 8;
			$s = substr($s, 0, $pos3);
			//printf("%s\n", $s);
			$a = explode("\n", $s);
			//print_r($a);
			$bp = 1;
		}
		else  {
			exit("oops2");
		}
	}
	else  {
		exit("oops1");
	}

	$n = count($a);

	for($i=0; $i<$n;$i++)  {
		if (!preg_match("/__declspec/", $a[$i], $matches))  {
			continue;
		}
		$s = trim($a[$i]);
		printf("%s\n", $s);
		$bp = $i;
	}

?>

