
<?php

	$txbuf = array(0,0,0,0,0,0,0);
	$buf = array(0,0,0,0,0,0,0);
	$pkt_mask = array(0,0,0,0,0,0,0);

	// floats:
	$wind = 0.0;
	$draft_wind = 0.0;
	$lbs = 160.0;
	$drag_aerodynamic = 8.0;
	$drag_tire = 2.0;
	$control_byte = 0x3c;
	$grade = 0.0;
	$manualWatts = 0.0;

	// ints
	$tx_select = 0;
	$upper_hr = 240;
	$lower_hr = 50;
	$bytes_out = 0;

	global $tx_select;
	global $wind, $draft_wind;
	global $lbs, $upper_hr, $lower_hr;
	global $drag_aerodynamic, $drag_tire;
	global $control_byte, $grade, $manualWatts;;
	global $txbuf, $buf;
	global $pkt_mask;
	global $bytes_out;
	global $fname;

	error_reporting(E_ALL);

	for($i=0; $i<1; $i++)  {
		updateHardware(null);
	}
	exit;

	$fname = "d:/php/readline.exe";
	if (!file_exists($fname))  {
		exit("oops\n");
	}

	//$term = `stty -g`;
	//system("stty -icanon");						// linux only
	//system("stty -icanon");

/*
	while(1)  {
		if (kbhit())  {
			$c = getc();
			if ($c=="\x1b")  {
				break;
			}
			if ($c==NULL)  {
				continue;
			}
			printf("%s", $c);
		}
		usleep(100000);
	}
	printf("\n");
*/


	$address = "127.0.0.1";
	$port = 9072;

	/*
	stream_set_blocking(STDIN, FALSE);
	while (1) {
		$c = fread(STDIN, 1);
		//printf("%s", $c);
		if ($c=="q")  {
			break;
		}
		usleep(100000);
	}
	printf("\nstarting\n");
	*/



	/*
	$b = socket_set_nonblock ($socket);
	if ($b===false)  {
		socket_close($socket);
		//system("stty '" . $term . "'");
		exit;
	}
	*/

	$flag = true;


	while($flag)  {
		$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);


		if ($socket === false) {
   		echo "socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "\n";
			//system("stty '" . $term . "'");
			exit;
		}

		// try to connect to server:

		while(1)  {
			if (kbhit())  {
				$c = getc();
				if ($c=="\x1b")  {
					$flag = false;
					break;
				}
				if ($c!=NULL)  {
					printf("%s", $c);
				}
			}

			printf(".");
			$result = @socket_connect($socket, $address, $port);
			if ($result === false) {
				//echo "socket_connect() failed.\nReason: ($result) " . socket_strerror(socket_last_error($socket)) . "\n";
				$s = socket_strerror(socket_last_error($socket));
				if (strpos($s, "actively refused it"))  {
					continue;
				}
				else  {
					printf("%s\n", $s);
				}
			}
			else  {
				printf("\nconnected\n");
				break;
			}
			usleep(100000);
		}														// while(1)

		if (!$flag)  {
			continue;
		}

		/*
		while(1)  {
			$read = array($socket);
			$write  = NULL;
			$except = NULL;
			$num_changed_sockets = socket_select($read, $write, $except, 0);

			if ($num_changed_sockets === false) {
				printf("socket error\n");
			}
			else if ($num_changed_sockets > 0) {
				printf("data\n");
			}
		}
		*/

		// keep reading from server as long as we're connected

		$type = PHP_BINARY_READ;
		//$type = PHP_NORMAL_READ;
		$connected = false;

		while(1)  {
			if (kbhit())  {
				$c = getc();
				if ($c=="\x1b")  {
					$flag = false;
					break;
				}
				if ($c!=NULL)  {
					printf("%s", $c);
				}
			}
			$data = @socket_read($socket, 2048, $type);
			if ($data===false)  {
				$s = socket_strerror(socket_last_error());
				if (strpos($s, "forcibly closed by the remote host"))  {
					printf("server disconnected\n");
				}
				else  {
					printf("\n%s\n", $s);
				}
				$connected = false;
				break;
			}
			else if ($data=="")  {
			}
			else if ($data=="RacerMate")  {
				printf("rx: %s\n", $data);
				$n = @socket_write($socket, "LinkUp", 6);
				if ($n != 6)  {
					printf("\n%s\n", socket_strerror(socket_last_error()));
				}
				printf("tx: %s\n", "LinkUp");
				$connected = true;								// connected to the server app
				printf("connected to server application\n");
			}
			else  {
				printf("------------------> %s\n", $data);
			}

			if ($connected)  {
				//updateHardware($socket);
				usleep(200000);								// sleep 200 ms
			}
			else  {
				usleep(100000);
			}
		}												// while(1)  {

		if (!$flag)  {
			continue;
		}

		$b = socket_shutdown($socket);
		printf("socket_shutdown = %s\n", $b?"true":"false");
		socket_close($socket);
		$socket = null;
		sleep(1);
	}												// while($flag)  {

	if ($socket != null)  {
		$b = @socket_shutdown($socket);
		//printf("\nb = %s\n", $b?"true":"false");
		@socket_close($socket);
		$socket = null;
	}

	//system("stty '" . $term . "'");

	exit;


	/********************************************************************************************

	********************************************************************************************/

	function updateHardware($socket)  {
		global $tx_select;
		global $wind, $draft_wind;
		global $lbs, $upper_hr, $lower_hr;
		global $drag_aerodynamic, $drag_tire;
		global $control_byte, $grade, $manualWatts;;
		global $txbuf, $buf;
		global $pkt_mask;
		global $bytes_out;

		$tx_select = ($tx_select & 0x70) + 0x10;		// 0x10 to 0x80

		switch($tx_select)  {

			case 0x10:								// grade	(-15.0 to +15.0)
				$i = round($grade);
				$is_signed = 1;
				break;

			case 0x20:  {						// wind
				$is_signed = 1;
				$i = round($wind + $draft_wind);
				break;
			}

			case 0x30:
				$is_signed = 0;
				$i = round($lbs);
				break;

			case 0x40:
				$is_signed = 0;
				$i = $lower_hr;
				$i |= 0x0800;							// set bit 11 for hb to understand
				break;

			case 0x50:
				$is_signed = 0;
				$i = $upper_hr;
				$i |= 0x0800;							// set bit 11 for hb to understand
				break;

			case 0x60:  {
				$is_signed = 0;
				$i = round(256.0*$drag_aerodynamic);		// 8.0 is default, 4095 is max (15.99 lbs)
				break;
			}

			case 0x70:  {
				$is_signed = 0;
				$i = round(256*$drag_tire);
				break;
			}

			case 0x80:
				$is_signed = 0;
				$i = $manualWatts;
				break;

			default:
				return 0;
		}												// switch

		if ($is_signed)  {									// signed?
			if ($i>2047)  {
				$i = 2047;
			}
			if ($i<-2048)  {
				$i = -2048;
			}
		}
		else  {
			if ($i<0)  {
				$i = 0;
			}
			if ($i>4095)  {
				$i = 4095;
			}
		}

		$txbuf[4] = ($tx_select | ((($i>>8)) & 0x0f));
		$txbuf[5] = $i&0xff;
		$txbuf[3] = $control_byte;

		$txbuf[1] = 0;
		$txbuf[2] = 0;

		$work = $txbuf[1];
		for($i=2;$i<6;$i++)  {
			$work = ($work+$txbuf[$i]) & 0xff;
		}
		$txbuf[0] = $work ^ 0xff;

		$work = 0x80;								// header byte

		//unsigned char buf[7];

		for($i=0; $i<6; $i++)  {
			$buf[$i] = $txbuf[$i]>>1;
			if ($txbuf[$i]&0x01)  {
				$work ^= $pkt_mask[$i];
			}
		}
		$buf[$i] = $work;

		$bytes_out += 7;

		$bytes = array();

		for($i=0; $i<7; $i++)  {
			//$bytes[$i] = pack("C", $buf[$i]);
			$bytes[$i] = chr($buf[$i]);
			if ($bytes[$i] != pack("C", $buf[$i]))  {
				$bp = $i;
			}
			//$bytes[$i] = pack("C", $buf[$i]); // "i" is unsigned little endian integer
			//socket_write($socket, $data);
			$v = intval($bytes[$i]);
			$v = $buf[$i];
			printf("%02x ", $buf[$i]);
			//printf("%s ", (int)$bytes[$i]);
		}
		printf("\n");

		//$bytes = pack('a', $buf);
		//To visualize that, use bin2hex:
		//echo bin2hex($bytes);
		//41 72 72 61 79
		//$s = bin2hex((string)$bytes);
		//$s = bin2hex((string)$buf);
		//printf("%s\n", $s);

		if ($socket != null)  {
			$n = @socket_write($socket, $buf, 7);
			if ($n != 7)  {
				printf("\nupdateHardware: %s\n", socket_strerror(socket_last_error()));
			}
		}

		//server->txx(buf, 7);

		return;
	}

/*************************************************************************

*************************************************************************/

function getc() {
	global $fname;

	/*
	if (!file_exists($fname))  {
		exit("oops\n");
	}
	*/

	$s = exec($fname);
	//printf("str = %s", $s);

	if (isset($s[0]))  {
	  	$c = $s[0];
	}
	else  {
		$c = NULL;
	}

	return $c;
}

/*************************************************************************

*************************************************************************/

function kbhit() {

	$fname = "d:/php/kbhit.exe";
	if (!file_exists($fname))  {
		exit("oops2\n");
	}

	$s = exec($fname);

	if (isset($s[0]))  {
	  	if ($s=="1")  {
			return true;
		}
	}
	return false;
}

?>

