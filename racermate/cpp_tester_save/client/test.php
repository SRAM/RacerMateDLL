
<?php

error_reporting(E_ALL);

$term = `stty -g`;
system("stty -icanon");


$address = "127.0.0.1";
$port = 9072;
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);


if ($socket === false) {
    echo "socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "\n";
}

/*
$b = socket_set_nonblock ($socket);
if ($b===false)  {
	socket_close($socket);
	system("stty '" . $term . "'");
	exit;
}
*/

$result = socket_connect($socket, $address, $port);
if ($result === false) {
    echo "socket_connect() failed.\nReason: ($result) " . socket_strerror(socket_last_error($socket)) . "\n";
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



while ($data = socket_read($socket, 2048)) {
	printf("------------------> %s\n", $data);
}

socket_close($socket);

system("stty '" . $term . "'");

?>

