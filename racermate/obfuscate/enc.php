<?php

	/*
	$item = "__declspec(dllexport) inline float *get_bars(int, int);";
	$n = preg_match("/\s[a-zA-Z0-9_\*]+\(/", $item, $matches);
	if (!$n)  {
		exit("logic error 1\n");
	}
	print_r($matches);
exit;
	*/


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
   $env = get_env();

	$r = array();									// results array

	$indepfiles = array(
				"../non_extended.h",
				"../extended.h",
		 );

	//-----------------------------------------------------
	// H E A D E R   F I L E S:
	//-----------------------------------------------------

	for($k=0; $k<count($indepfiles); $k++)  {
		$infname = $indepfiles[$k];
		$pi = pathinfo($infname);
		$outfname = $pi['dirname'] . "/" .$pi['filename'] . "_obfuscated." . $pi['extension'];

		$r[$k]['in_name'] = $infname;
		$r[$k]['out_name'] = $outfname;

		$contents = file_get_contents($infname);
		$n = preg_match("/#ifndef OBFUSCATE(.*?)#else/sm", $contents, $matches);
		if (!$n)  {
			printf("can't find pattern in %s\n", $infname);
			exit;
		}

		$a = array();
		$raw = array();
		$funcs = array();
		$md5 = array();

		$str = substr($matches[0], 19);
		$a = explode("\n", $str);

		for ($i=0; $i<count($a); $i++)  {
			$s = trim($a[$i]);
			if (strpos($s, "__declspec(dllexport)")===false)  {
				continue;
			}
			array_push($raw, $s);
		}
		$r[$k]['raw'] = $raw;


		for($i=0; $i<count($r[$k]['raw']); $i++)  {
			$item = $r[$k]['raw'][$i];
			$item = str_replace("*", "", $item);
			$n = preg_match("/\s[a-zA-Z0-9_\*]+\(/", $item, $matches);
			if (!$n)  {
				exit("logic error 1a\n");
			}
			$funcname = trim($matches[0], "\t (");
			array_push($funcs, $funcname);
			$newfuncname = "x" . md5($funcname);
			//printf("%s ----> %s\n", $funcname, $newfuncname);
			array_push($md5, $newfuncname);
			$r[$k]['raw'][$i] = str_replace($funcname, $newfuncname, $r[$k]['raw'][$i]);
			$r[$k]['raw'][$i] .= "					// $funcs[$i]";
			$bp = $i;
		}

		$r[$k]['funcs'] = $funcs;
		$r[$k]['md5'] = $md5;

		if (true)  {
			$fname = $r[$k]['out_name'];
			$fp = fopen($fname, "w");
			for($i=0; $i<count($r[$k]['raw']); $i++)  {
				//printf("%s\n", $non_extended_list[$i]);
				fprintf($fp, "%s\n", $r[$k]['raw'][$i]);
			}
			fclose($fp);											// finished creating non_extended_obfuscated
		}

		$bp = $k;
	}											// for($k....

	//print_r($r);

	$bp = 0;

	//------------------------------------------------
	// now dow the dependent files:
	//------------------------------------------------

	$depfiles = array(
		 "../ergvid.cpp",
	);

	if (true)  {
		for($k=0; $k<count($depfiles); $k++)  {
			$infname = $depfiles[$k];
			$pi = pathinfo($infname);
			$outfname = $pi['dirname'] . "/" .$pi['filename'] . "_obfuscated." . $pi['extension'];

			$infp = fopen($infname, "r");
			if (!$infp)  {
				exit("can't open $infname");
			}

			$outfp = fopen($outfname, "w");

			while(1)  {
				$line = fgets($infp, 4096);
				if ($line===false)  {
					break;
				}

				if (strpos($line, "#ifdef OBFUSCATE") !== false)  {
					continue;									// just throw these lines away
				}

				$nindepfiles = count($indepfiles);

				for($i=0; $i<$nindepfiles; $i++)  {
					$nfuncs = count($r[$i]['funcs']);
					for($j=0; $j<$nfuncs; $j++)  {
						if (strpos($line, "ev->"))  {
							continue;
						}
						if (strpos($line, $r[$i]['funcs'][$j]))  {
							$line = str_replace($r[$i]['funcs'][$j], $r[$i]['md5'][$j], $line);			// search, replace, subject
						}
					}											// for each func in that header file
					$bp = $k;
				}												// for each header file
				fprintf($outfp, "%s", $line);
			}													// while 1 (reading the input file)

			if (!feof($infp)) {
				echo "Error: unexpected fgets() fail\n";
			}
			fclose($infp);
			fclose($outfp);
			$bp = $k;
		}
	}

	$bp = 0;

exit;

?>

