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

	////////////////////////////////////////////////////////////////////////////

	// http://www.php.net/manual/en/regexp.reference.meta.php
	// http://www.regular-expressions.info/dot.html
	// http://www.regular-expressions.info/lookaround.html		(negative lookahead)

	/*
	$s = "z(x);
		z(x) {";
	$pat = "/z\((.*?)\)(?!;)\s*\{/";				// negative lookahead
	//$pat = "/z\((.*?)\)(?= )\s*\{/";				// positive lookahead
	//$pat = "/z\((.*?)\)(?<\;)\s*\{/";		// negative lookbehind (?<!pattern)

	$n = preg_match($pat, $s, $matches);
	print_r($matches);
exit;
	*/

	/*
	$s = "if ( (what != DEVICE_COMPUTRAINER) && (what != DEVICE_VELOTRON) )  {
		what = check_for_trainers(comport);
		if (what != DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON )  {
			return WRONG_DEVICE;
		}
	}
	EnumDeviceType check_for_trainers(int _comport)  {
	";
	$pat = "/check_for_trainers\((.*?)\)(?!;)\s*\{/";					// negative lookahead
	//$pat = "/check_for_trainers\s*\((.*?)\)(?!;)\s*\{/sm";			// negative lookahead
	$n = preg_match($pat, $s, $matches);
	if ($n == 0)  {
		printf("can't find it\n");
		exit;
	}
	//print_r($matches);
	$s = $matches[0];
	printf("%s\n", $s);
	$bp = 2;
exit;
	*/

////////////////////////////////////////////////////////////////////////////

// from map.h:

	define("DLL_FULL_ACCESS",			65536	);
	define("DLL_ERGVIDEO_ACCESS",		32768	);
	define("DLL_trinerd_ACCESS",		24000	);
	define("DLL_CANNONDALE_ACCESS",	16384	);

	$r = array();									// results array

	$infname = "../dll.h";
	$pi = pathinfo($infname);
	$outfname = "x.x";

	$contents = file_get_contents($infname);								// dll.h
	//save_string($contents, "xx.xx");
	//printf("%s\n", $contents);

	//$cannondale = get_level("DLL_CANNONDALE_ACCESS", $contents, $infname);
	//$trinerd = get_level("DLL_TRINERD_ACCESS", $contents, $infname);
	$ergvideo = get_level("DLL_ERGVIDEO_ACCESS", $contents, $infname);
	//$full = get_level("DLL_FULL_ACCESS", $contents, $infname);

	if (true)  {
		if ($cannondale != null)  {
			printf("\nCANNONDALE:\n");
			print_r($cannondale);
		}

		if ($trinerd != null)  {
			printf("\ntrinerd:\n");
			print_r($ergvideo);
		}

		if ($ergvideo != null)  {
			printf("\nERGVIDEO:\n");
			print_r($ergvideo);
		}

		if ($full != null)  {
			printf("\nFULL:\n");
			print_r($full);
		}

	}


	$results = array();

	$name = "cannondale";
	$who = "canNondaLE";
	$a = get($name, $who, $cannondale);
	$results[$name] = $a;


	$name = "trinerd";
	$who = "trinerd";
	$a = get($name, $who, $cannondale);
	$b = get($name, $who, $trinerd);
	$c = array_merge($a, $b);
	$results[$name] = $c;


	$name = "ergvideo";
	$who = "ERgvidEo";
	$a = get($name, $who, $cannondale);
	$b = get($name, $who, $trinerd);
	$c = get($name, $who, $ergvideo);
	$d = array_merge($a, $b, $c);
	$results[$name] = $d;

	$name = "full";
	$who = "FUlL";
	$a = get($name, $who, $cannondale);
	$b = get($name, $who, $trinerd);
	$c = get($name, $who, $ergvideo);
	$d = get($name, $who, $full);
	$e = array_merge($a, $b, $c, $d);
	$results[$name] = $e;

	if (false)  {
		printf("cannondale count = %d\n", count($results['cannondale']));
		printf("trinerd count = %d\n", count($results['trinerd']));
		printf("ergvid count = %d\n", count($results['ergvideo']));
		printf("full count = %d\n", count($results['full']));
	}


	if (true)  {
		//print_r($results);
		//print_array(false, "RESULTS", $results);
		//print_array(false, "CANNONDALE", $results['cannondale']);
		//print_array(false, "trinerd", $results['trinerd']);
		//print_array(false, "ERGVIDEO", $results['ergvideo']);
		//print_array(false, "FULL", $results['full']);
	}

	//make_header1("cannondale", $results);				// makes cannondale.h for the main dll directory
	//make_header2("cannondale", $results);				// makes cannondale.h for the standalone test program in ../cannondale/

	//make_header1("trinerd", $results);				// makes trinerd.h for the main dll directory
	//make_header2("trinerd", $results);				// makes trinerd.h for the standalone test program in ../trinerd/

	make_header1("full", $results);					// makes full.h for the main dll directory
	make_header2("full", $results);					// makes full.h for the standalone test program in ../

	//make_header("ergvideo", $results);
	//make_header("full", $results);

	//make_cpp("cannondale", $results);
	//make_cpp("trinerd", $results);
	//make_cpp("ergvideo", $results);
	make_cpp("full", $results);

	//$docs = get_docs();
	//print_r($docs);

	//make_doc("cannondale", $results, $docs);
	//make_doc("trinerd", $results, $docs);

exit;

/********************************************************************************************

********************************************************************************************/


function get_level($level, $contents, $infname)  {

	$pat = "/\t#if \(LEVEL == $level\)(.*?)#endif/sm";

	$n = preg_match($pat, $contents, $match);
	if ($n != 1)  {
		printf("can't find %s in %s\n", $pat, $infname);
		exit;
	}

	$s = $match[0];

	printf("%s", $s);

	$n = preg_match_all("/__declspec(.*?);/sm", $s, $matches);
	if ($n==0)  {
		printf("can't find __declspec\n");
		printf("%s\n", $s);
		exit;
	}

	$a = array();

	for($i=0; $i<$n; $i++)  {
		$s = trim($matches[0][$i]);
		$s = trim(str_replace("throw(...)", "", $s));
		$s = trim(str_replace("inline ", "", $s));
		$s = trim(str_replace("__declspec(dllexport) ", "", $s));
		$s = trim(str_replace(";", "", $s));
		$proto = $s;
		$s = strrev($s);
		$s = preg_replace("/^\)(.*?)\(/", "", $s);
		$s = preg_replace("/ (.*?)$/", "", $s);
		$s = preg_replace("/\*+$/", "", $s);

		$s = strrev($s);

		$b = array();
		$b['name'] = $s;
		$b['proto'] = $proto;
		//array_push($a, $s);
		array_push($a, $b);
		//printf("%3d  %s\n", $i+1, $s);
	}

	return $a;
}									// function get_level($contents)  {



/********************************************************************************************

********************************************************************************************/

function map($results)  {

	$level = array_keys($results);

	$n = count($results);

	//---------------------------------
	// first check for duplicates:
	//---------------------------------

	$enc = array();

	for($k=0; $k<$n; $k++)  {
		$r = $results[$level[$k]];
		$m = count($r);
		for($i=0; $i<$m; $i++)  {
			$encrypted = $r[$i]['encrypted'];
			array_push($enc, $encrypted);
		}
	}

	$z = array_unique($enc);
	$n1 = count($enc);
	$n2 = count($z);

	if ($n1 != $n2)  {
		printf("collision! n1 = %d, n2 = %d\n", $n1, $n2);
		exit;
	}

	//print_array(false, "RESULTS", $results);


	$fp = fopen("../map.h", "w");

	//printf("\n// tack this on to ../ergvid.h (from ./obfuscate/enc2.php):\n\n");
	fprintf($fp, "#ifndef _MAP_H_\n#define _MAP_H_\n\n");
	$bp = 0;

	$s = "
#define DLL_FULL_ACCESS			65536
#define DLL_ERGVIDEO_ACCESS		32768
#define DLL_CANNONDALE_ACCESS	16384
#define DLL_MINIMUM_ACCESS		    1

#ifdef _DEBUG
	//#define EXTENDED_DLL
	//#define LEVEL	DLL_FULL_ACCESS
	#define LEVEL	DLL_CANNONDALE_ACCESS
	//#define LEVEL	DLL_ERGVIDEO_ACCESS
#else
	//#define EXTENDED_DLL						// allow for tts, motion sensor, plotter ENGINE only
	#define LEVEL DLL_MINIMUM_ACCESS
#endif
	";

	fprintf($fp, "%s\n", $s);

	for($k=0; $k<$n; $k++)  {
		$r = $results[$level[$k]];
		$m = count($r);

		$ss = sprintf("\n#if LEVEL >= DLL_%s_ACCESS\n", strtoupper($level[$k]));
		printf("%s", $ss);
		fprintf($fp, "%s", $ss);

		for($i=0; $i<$m; $i++)  {
			$original = $r[$i]['original'];
			$encrypted = $r[$i]['encrypted'];
			$s = sprintf("#define %-30s %s", $original, $encrypted);
			$ss = sprintf("\t%s\t\t\t// %d\n", $s, $i+1);
			printf("%s", $ss);
			fprintf($fp, "%s", $ss);
		}

		$ss = sprintf("\n#endif\n");
		printf("%s", $ss);
		fprintf($fp, "%s", $ss);

	}

	fprintf($fp, "#endif\t\t\t// #ifndef _MAP_H_\n");

	fclose($fp);

	return;
}								// map()

/********************************************************************************************
	produces doc.txt
	this is a file of all the function names with parameters named.
********************************************************************************************/

function doc($results)  {

	$contents = file_get_contents("ergvid.cpp");
	$level = array_keys($results);
	$n = count($level);

	$fp = fopen("doc.txt", "w");

	for($k=0; $k<$n; $k++)  {							// for each level
		$lev = $level[$k];								// level name
		$ss = sprintf("%s\r\n", $lev);
		//printf("%s", $ss);
		fprintf($fp, "%s", $ss);
		$r = $results[$lev];
		$m = count($r);
		//debugging
		if (strpos($lev, "full") !== false)  {
			$bp = 2;
		}

		for($i=0; $i<$m; $i++)  {
			$original = $r[$i]['original'];
			$encrypted = $r[$i]['encrypted'];
			//$pat = "/$original(.*?)[^;{]/sm";
			//$pat = "/$original(.*?)\{/sm";
			$pat = "/$original(.*?)\{/";
			//debugging
			if (strpos($original, "GetCalibration") !== false)  {
				$bp = 2;
			}
			$nn = preg_match($pat, $contents, $match);
			if ($nn==0)  {
				printf("can't find %s\n", $original);
				fclose($fp);
				exit;
			}
			else if ($nn==1)  {

			}
			else  {
				$bp = 7;
			}
			$s = $match[0];

			/*
			// skip calls of the function:

			//debugging
			if (strpos($original, "check_for_trainers") !== false)  {
				$bp = 2;
			}

			$pat = "/$original(.*?);/sm";
			$nn = preg_match($pat, $s, $match);

			if ($nn==0)  {
				$bp = 0;
			}
			else if ($nn==1)  {
				$bp = 2;
			}
			else  {
				$bp = 5;
			}
			*/

			$s = preg_replace("/\s+\{(.*?)$/", ";", $s);
			$ss = sprintf("\t%s\r\n", $s);
			//printf("%s", $ss);
			fprintf($fp, "%s", $ss);

			$bp = $i;
		}

		$bp = $k;
	}

	fclose($fp);

	//print_array(false, "RESULTS", $results);

	return;

}										// doc()

/********************************************************************************************
	modifies ergvid.cpp
********************************************************************************************/

function ergvid($results)  {

	$contents = file_get_contents("ergvid.cpp");
	$level = array_keys($results);
	$n = count($level);

	$fp = fopen("x.y", "w");

	$a = array();

	for($k=0; $k<$n; $k++)  {							// for each level
		$lev = $level[$k];								// level name
		$ss = sprintf("%s\r\n", $lev);
		$r = $results[$lev];
		$m = count($r);

		for($i=0; $i<$m; $i++)  {						// for each function in this level of access
			$original = $r[$i]['original'];
			$encrypted = $r[$i]['encrypted'];
			//$pat = "/$original(.*?)[^;{]/sm";
			//$pat = "/$original(.*?)\{/sm";
			$pat = "/$original\((.*?)\)(?!;)\s*\{/";					// negative lookahead
			//debugging
			if (strpos($original, "check_for_trainers") !== false)  {
				$bp = 2;
			}
			$nn = preg_match($pat, $contents, $match);
			if ($nn==0)  {
				printf("can't find %s\n", $original);
				fclose($fp);
				exit;
			}
			else if ($nn!=1)  {
				$bp = 7;
			}
			$s = $match[0];

			//$s = preg_replace("/\s+\{(.*?)$/", ";", $s);
			//$ss = sprintf("\t%s\r\n", $s);

			if ($lev=='cannondale')  {
				//$ss = sprintf("\n#if LEVEL >= DLL_CANNNONDALE_ACCESS\n");
				$s = str_replace($original, $encrypted, $s);
				$ss = sprintf("/* %-30s */ %-140s             // cannondale", $original, $s);
				if (!isset($a[$original]))  {
					$a[$original] = array();
				}
				array_push($a[$original], $ss);
			}
			else if ($lev=='ergvideo')  {
				$s = str_replace($original, $encrypted, $s);
				$ss = sprintf("/* %-30s */ %-140s             // ergvideo", $original, $s);
				if (!isset($a[$original]))  {
					$a[$original] = array();
				}
				array_push($a[$original], $ss);
			}
			else if ($lev=='full')  {
				$s = str_replace($original, $encrypted, $s);
				$ss = sprintf("/* %-30s */ %-140s             // full", $original, $s);
				if (!isset($a[$original]))  {
					$a[$original] = array();
				}
				array_push($a[$original], $ss);
			}
			else  {
				$bp = 2;
			}

			$bp = $i;
		}

		$bp = $k;
	}

	fclose($fp);

	$n = count($a);

	foreach($a as $func=>$levs)  {
		$m  = count($levs);
		//printf("%s\n", $func);
		$pat = "/$func(.*?)\{/";
		//debugging
		if (strpos($func, "GetCalibration") !== false)  {
			$bp = 2;
		}
		$nn = preg_match($pat, $contents, $match);
		if ($nn==0)  {
			printf("can't find %s\n", $original);
			fclose($fp);
			exit;
		}
		else if ($nn==1)  {
		}
		else  {
			$bp = 7;
		}
		$full_original = $match[0];

		$r = "";								// replacement

		for($j=0; $j<$m; $j++)  {					// for each access level
			if ($j==0)  {
				$r .= sprintf("\r\n");
			}

			$s = $levs[$j];
			if (strpos($s, "// cannondale")!==false)  {
				$level = "cannondale";
				if ($j==0)  {
					$header = "#if LEVEL >= DLL_CANNNONDALE_ACCESS";
				}
				else  {
					$header = "#elif LEVEL >= DLL_CANNNONDALE_ACCESS";
				}
			}
			else if (strpos($s, "// ergvideo")!==false)  {
				$level = "ergvideo";
				if ($j==0)  {
					$header = "#if LEVEL >= DLL_ERGVIDEO_ACCESS";
				}
				else  {
					$header = "#elif LEVEL >= DLL_ERGVIDEO_ACCESS";
				}
			}
			else if (strpos($s, "// full")!==false)  {
				$level = "full";
				if ($j==0)  {
					$header = "#if LEVEL >= DLL_FULL_ACCESS";
				}
				else  {
					$header = "#elif LEVEL >= DLL_FULL_ACCESS";
				}
			}
			else  {
				$bp = 0;
			}

			$r .= sprintf("%s\r\n", $header);
			$r .= sprintf("	%s\r\n", $s);
			if ($j==($m-1))  {
				$r .= sprintf("#endif\r\n");
			}

			$bp = $j;
		}

		$contents = str_replace($full_original, $r, $contents);
		printf("%s", $r);
		$bp = $i;
	}

	file_put_contents("ergvid.mod", $contents);

	return;

}										// ergvid()

/********************************************************************************************

********************************************************************************************/

function get($name, $who, $funcs)  {
	$list = array();
	$n = count($funcs);

	for($i=0; $i<$n; $i++)  {
		$original = $funcs[$i]['name'];
		$funcname =  $original . "_" . $who;
		$newfuncname = "x" . md5($funcname);
		$a = array();
		$a['original'] = $original;
		$a['encrypted'] = $newfuncname;
		$a['proto'] = $funcs[$i]['proto'];
		array_push($list, $a);
		$bp = $i;
	}

	return $list;
}												// get()


/********************************************************************************************

********************************************************************************************/

function get_docs()  {

	$contents = file_get_contents("../dll.txt");
	if ($contents==null)  {
		printf("can't find docs\n");
		exit;
	}

	$len = strlen($contents);
	if ($len==null)  {
		printf("can't find docs\n");
		exit;
	}

	$list = array();

	$pat = "/\^(.*?)\\$/sm";

	$n = preg_match_all($pat, $contents, $matches);
	if ($n == 0)  {
		printf("can't find docs\n");
		exit;
	}

	//print_r($matches);
	//print_array(false, "MATCHES[0]", $matches[0]);
	//printf("\n\n\n");
	//print_array(false, "MATCHES[1]", $matches[1]);

	$bp = 1;

	//$fp = fopen("x.x", "w");

	for($i=0; $i<$n; $i++)  {
		$s = $matches[0][$i];
		$s = trim($s, "^$ \t");

		$pat = "/^(.*?)\n/";

		$m = preg_match_all($pat, $s, $match);
		if ($m == 0)  {
			printf("can't find what\n");
			exit;
		}

		$what = $match[0][0];
		$what = trim($what);
		//printf("%s\n", $s);

		//debugging
		if (stripos($what, "state information") !== false)  {
			$bp = 0;
		}

		if (stripos($what, ", ") !== false)  {
			$a = explode(",", $what);
			$bp = 2;
			if (count($a)!=2)  {
				printf("count != 2\n");
				exit;
			}
			$list[trim($a[0])] = $s;
			$list[trim($a[1])] = $s;
		}
		else  {
			$list[$what] = $s;
		}


		$bp = $i;

		/*
		if (strpos($s, '^AP Interface functions')!==false)  {
			$s = trim($s, "^$ \t");
			printf("%s\n", $s);
			$bp = 7;
		}
		else if (strpos($s, '^Notes:')!==false)  {
			$bp = 8;
		}
		*/


	}

	//fclose($fp);

	return $list;
}


/********************************************************************************************

********************************************************************************************/

function make_doc($name, $results, $docs)  {

	$indices = array_keys($docs);
	//print_r($indices);
	//print_r($docs);

	//$fp = fopen("../" .$name . ".txt", "w");
	if ($name=='full')  {
		$fp = fopen("../" . $name . ".txt", "w");
	}
	else  {
		$fp = fopen("../$name/" . $name . ".txt", "w");
	}

	fprintf($fp, "%s\n", trim($docs['AP Interface functions']));
	fprintf($fp, "\n%s\n", trim($docs['Structures']));

	$a = $results[$name];
	$n = count($a);

	for($i=0; $i<$n; $i++)  {
		$original = $a[$i]['original'];
		$encrypted = $a[$i]['encrypted'];
		$proto = $a[$i]['proto'];

		if (!isset($docs[$original]))  {
			printf("can't find %s in \$docs\n", $original);
			continue;
		}
		$s = trim($docs[$original]);

		fprintf($fp, "\n------------------------------------------------------------------------------\n");
		fprintf($fp, "\n%s\n", $s);

		$bp = $i;
	}

	fprintf($fp, "\n------------------------------------------------------------------------------\n");
	fprintf($fp, "\n%s\n", trim($docs['Handlebar Controller state information']));
	fclose($fp);

	return;
}									// make_doc

/********************************************************************************************
	makes $name.h for the main dll directory
********************************************************************************************/

function make_header1($name, $results)  {

	if ($name=='full')  {
		$fp = fopen("../" . $name . ".h", "w");
	}
	else  {
		$s = "../" . $name . ".h";
		$fp = fopen($s, "w");
	}


	fprintf($fp, "#ifndef _%s_H_\r\n", strtoupper($name));
	fprintf($fp, "#define _%s_H_\r\n\r\n", strtoupper($name));

	//$contents = file_get_contents("../../tlib/public.h");



	/*
	fprintf($fp, "\r\n#include \"levels.h\"\r\n");
	fprintf($fp, "\r\n#if LEVEL == DLL_%s_ACCESS\r\n\r\n", strtoupper($name));
	*/

	//fprintf($fp, "#include <tdefs.h>\r\n");
	//fprintf($fp, "#include <course.h>\r\n\r\n");

	fprintf($fp, "#include <public.h>\r\n\r\n");

	if ($name=="full")  {
		fprintf($fp, "#include <physics.h>\r\n");
		fprintf($fp, "#include <datasource.h>\r\n\r\n");
		fprintf($fp, "#include <decoder.h>\r\n\r\n");
	}
	else if ($name=="ergvideo")  {
		fprintf($fp, "#include <bike.h>\r\n\r\n");
	}
	else if ($name=="trinerd")  {
		//fprintf($fp, "#include <bike.h>\r\n\r\n");
	}

	//-------------------------------
	// add the mapping:
	//-------------------------------

	$n = count($results[$name]);

	for($i=0; $i<$n; $i++)  {
		$original = $results[$name][$i]['original'];
		$encrypted = $results[$name][$i]['encrypted'];
		fprintf($fp, "#define %-30s %s\n", $original, $encrypted);
	}


	fprintf($fp, "\r\n");
	fprintf($fp, "//-------------------------------\r\n");
	fprintf($fp, "// prototypes:\r\n");
	fprintf($fp, "//-------------------------------\r\n\r\n");

	//-------------------------------
	// add the prototype:
	//-------------------------------

	for($i=0; $i<$n; $i++)  {
		$original = $results[$name][$i]['original'];
		$encrypted = $results[$name][$i]['encrypted'];
		$proto = $results[$name][$i]['proto'];
		$s = str_replace($original, $encrypted, $proto);
		fprintf($fp, "__declspec(dllexport) %s;\r\n", $s);
	}

	//fprintf($fp, "\r\n#endif				//#if LEVEL == DLL_%s_ACCESS\"\r\n", strtoupper($name));

	fprintf($fp, "\n#endif				// #ifdef _%s_H_\n\n", strtoupper($name));
	fclose($fp);

	return;
}									// make_header1

/********************************************************************************************
	makes $name.h for the standalone test program in ../$name/
********************************************************************************************/

function make_header2($name, $results)  {

	if ($name=='full')  {
		//xxx
		//$fp = fopen("../" . $name . ".h", "w");
		return;
	}
	else  {
		$fp = fopen("../$name/" . $name . ".h", "w");
	}


	fprintf($fp, "#ifndef _%s_H_\r\n", strtoupper($name));
	fprintf($fp, "#define _%s_H_\r\n\r\n", strtoupper($name));

	$s = "
struct TrainerData	{
	float kph;			// ALWAYS in KPH, application will metric convert. <0 on error
	float cadence;		// in RPM, any number <0 if sensor not connected or errored.
	float HR;			// in BPM, any number <0 if sensor not connected or errored.
	float Power;		// in Watts <0 on error
};

enum EnumDeviceType {
	DEVICE_NOT_SCANNED,					// unknown, not scanned
	DEVICE_DOES_NOT_EXIST,				// serial port does not exist
	DEVICE_EXISTS,							// exists, openable, but no RM device on it
	DEVICE_COMPUTRAINER,
	DEVICE_VELOTRON,
	DEVICE_SIMULATOR,
	DEVICE_RMP,
	DEVICE_ACCESS_DENIED,				// port present but can't open it because something else has it open
	DEVICE_OPEN_ERROR,					// port present, unable to open port
	DEVICE_OTHER_ERROR					// prt present, error, none of the above
};

struct SSDATA	{
	float ss;
	float lss;
	float rss;
	float lsplit;
	float rsplit;
};

enum  {
	ALL_OK = 0,
	DEVICE_NOT_RUNNING = INT_MIN,			// 0x80000000
	WRONG_DEVICE,							// 0x80000001
	DIRECTORY_DOES_NOT_EXIST,
	DEVICE_ALREADY_RUNNING,
	BAD_FIRMWARE_VERSION,
	VELOTRON_PARAMETERS_NOT_SET,
	BAD_GEAR_COUNT,
	BAD_TEETH_COUNT,
	PORT_DOES_NOT_EXIST,
	PORT_OPEN_ERROR,
	PORT_EXISTS_BUT_IS_NOT_A_TRAINER,
	DEVICE_RUNNING,
	BELOW_UPPER_SPEED,
	ABORTED,
	TIMEOUT,
	BAD_RIDER_INDEX,
	DEVICE_NOT_INITIALIZED,
	CAN_NOT_OPEN_FILE,
	GENERIC_ERROR
};

enum DIRTYPE {
	DIR_PROGRAM,                    // 0
	DIR_PERSONAL,
	DIR_SETTINGS,
	DIR_REPORT_TEMPLATES,
	DIR_REPORTS,
	DIR_COURSES,
	DIR_PERFORMANCES,
	DIR_DEBUG,
	DIR_HELP,                       // 8
	NDIRS                           // 9
};


	";

	fprintf($fp, "\r\n%s\r\n", $s);


	/*
	fprintf($fp, "\r\n#include \"levels.h\"\r\n");
	fprintf($fp, "\r\n#if LEVEL == DLL_%s_ACCESS\r\n\r\n", strtoupper($name));
	*/

	//fprintf($fp, "#include <tdefs.h>\r\n");
	//fprintf($fp, "#include <course.h>\r\n\r\n");

	if ($name=="full")  {
		fprintf($fp, "#include <physics.h>\r\n");
		fprintf($fp, "#include <physics.h>\r\n");
		fprintf($fp, "#include <datasource.h>\r\n\r\n");
		fprintf($fp, "#include <decoder.h>\r\n\r\n");
	}
	else if ($name=="ergvideo")  {
		fprintf($fp, "#include <bike.h>\r\n\r\n");
	}
	else if ($name=="trinerd")  {
		//fprintf($fp, "#include <bike.h>\r\n\r\n");
	}

	//-------------------------------
	// add the mapping:
	//-------------------------------

	$n = count($results[$name]);

	for($i=0; $i<$n; $i++)  {
		$original = $results[$name][$i]['original'];
		$encrypted = $results[$name][$i]['encrypted'];
		fprintf($fp, "#define %-30s %s\n", $original, $encrypted);
	}


	fprintf($fp, "\r\n");
	fprintf($fp, "//-------------------------------\r\n");
	fprintf($fp, "// prototypes:\r\n");
	fprintf($fp, "//-------------------------------\r\n\r\n");

	//-------------------------------
	// add the prototype:
	//-------------------------------

	for($i=0; $i<$n; $i++)  {
		$original = $results[$name][$i]['original'];
		$encrypted = $results[$name][$i]['encrypted'];
		$proto = $results[$name][$i]['proto'];
		$s = str_replace($original, $encrypted, $proto);
		fprintf($fp, "__declspec(dllexport) %s;\r\n", $s);
	}

	//fprintf($fp, "\r\n#endif				//#if LEVEL == DLL_%s_ACCESS\"\r\n", strtoupper($name));

	fprintf($fp, "\n#endif				// #ifdef _%s_H_\n\n", strtoupper($name));
	fclose($fp);

	return;
}									// make_header2

/********************************************************************************************
	makes cannondale.cpp for the main dll directory
********************************************************************************************/

function make_cpp($name, $results)  {

	//print_array(false, "RESULTS", $results);			// ok here



	$contents = file_get_contents("../dll.cpp");								// dll.cpp
	//save_string($contents, "xx.xx");

	$funcs = $results[$name];

	//$fp = fopen("../$name.cpp", "w");
	if ($name=='full')  {
		// replace #if 0 with #if 1
		$contents = preg_replace("/#if 0/", "#if 1", $contents);
		//$pat = "#^//\^\s*$func(.*?)\s+#";							// get rid of starting marker
		//$s = preg_replace($pat, "", $match[0]);

		file_put_contents("../full.cpp", $contents);				// note: this is unencrypted for full access

		return;
	}
	else  {
		$s = "../$name" . ".cpp";
		$fp = fopen($s, "w");
	}

	fprintf($fp, "\r\n#include \"levels.h\"\r\n");
	fprintf($fp, "\r\n#if LEVEL == DLL_%s_ACCESS\r\n", strtoupper($name));
	fprintf($fp, "\r\n#pragma message (\"***********************  DLL_%s_ACCESS  ***********************\")\r\n", strtoupper($name));

	if ($name=="full")  {
		fprintf($fp, "\r\n#ifdef WIN32\r\n");
		fprintf($fp, "	#include <conio.h>\r\n");
		fprintf($fp, "	#include <nvram.h>\r\n");
		fprintf($fp, "#endif\r\n");

		fprintf($fp, "const int ctmask[6] = { 0x01, 0x02, 0x08, 0x10, 0x04, 0x20 };\r\n");
		fprintf($fp, "const int vtmask[6] = { 0x02, 0x04, 0x10, 0x01, 0x08, 0x20 };\r\n");

	}
	else if ($name=="ergvideo")  {
		fprintf($fp, "\r\n#ifdef WIN32\r\n");
		fprintf($fp, "	#include <conio.h>\r\n");
		fprintf($fp, "	#include <nvram.h>\r\n");
		fprintf($fp, "#endif\r\n");

		fprintf($fp, "const int ctmask[6] = { 0x01, 0x02, 0x08, 0x10, 0x04, 0x20 };\r\n");
		fprintf($fp, "const int vtmask[6] = { 0x02, 0x04, 0x10, 0x01, 0x08, 0x20 };\r\n");

	}
	else if ($name=="trinerd")  {
		fprintf($fp, "\r\n#ifdef WIN32\r\n");
		fprintf($fp, "	#include <conio.h>\r\n");
		fprintf($fp, "	#include <nvram.h>\r\n");
		fprintf($fp, "#endif\r\n");

		fprintf($fp, "const int ctmask[6] = { 0x01, 0x02, 0x08, 0x10, 0x04, 0x20 };\r\n");
		fprintf($fp, "const int vtmask[6] = { 0x02, 0x04, 0x10, 0x01, 0x08, 0x20 };\r\n");

	}

	fprintf($fp, "\r\n#include \"dll_common.h\"\r\n");
	fprintf($fp, "#include \"dll_globals.h\"\r\n");
	fprintf($fp, "#include \"dll.h\"\r\n");
	fprintf($fp, "#include \"internal.h\"\r\n");

	$n = count($funcs);							// 20 for cannondale

	for($i=0; $i<$n; $i++)  {

		//--------------------------------------------------------------------
		// extract the functions out of dll.cpp and put them into $name.cpp
		//--------------------------------------------------------------------

		$func = $funcs[$i]['original'];
		$enc = $funcs[$i]['encrypted'];

		printf("%3d  %s\n", $i+1, $func);

		//debugging
		if (stripos($func, "starttrainer") !== false)  {
			$bp = 2;
		}


		// find the function in ../dll.cpp

		//xxx

		//$pat = "#^//\^\s*$func(.*?)//\\$#sm";
		$pat = "#^//\^\s*$func\s+(.*?)//\\$#sm";

		$nn = preg_match($pat, $contents, $match);
		if ($nn != 1)  {
			printf("can't find funcion %s\n", $func);
			fclose($fp);
			exit;
		}

		fprintf($fp, "\r\n");
		fprintf($fp, "%s\r\n", "/*******************************************************************************************************");
		fprintf($fp, "	%s\r\n", $func);
		fprintf($fp, "%s\r\n", "*******************************************************************************************************/");
		fprintf($fp, "\r\n");

		$pat = "#^//\^\s*$func(.*?)\s+#";							// get rid of starting marker
		$s = preg_replace($pat, "", $match[0]);
		$pat = "#//\\$#";													// get rid of ending marker
		$s = preg_replace($pat, "", $s);

		$a = explode("\r\n", $s);
		$m = count($a);

		for($j=0; $j<$m; $j++)  {
			$ss = rtrim($a[$j]);
			//debugging
			if (stripos($ss, "setlogfilepath") !== false)  {
				$bp = 2;
			}

			if (strpos($ss, $func) !== false)  {
				$pat = "/\{$/";
				if (preg_match($pat, $ss))  {
					$bp = $j;
					$ss = str_replace($func, $enc, $ss);
				}
			}
			fprintf($fp, "%s\r\n", $ss);
			$bp = $j;
		}
		$bp = $i;
	}								// for($i=0; $i<$n; $i++)  {

	fprintf($fp, "\r\n#endif					//#if LEVEL == DLL_%s_ACCESS\"\r\n", strtoupper($name));
	fclose($fp);

	return;
}											// make_header2()

?>

