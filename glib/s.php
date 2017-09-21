<?php
	$now = date("Y-m-d");
	@unlink($now . ".zip");
	$cmd = sprintf("zip -r %s . -i@files.i", $now);
	system($cmd);

	printf("\n\n***************** FINISHED **********************\n\n");

	$cmd = "ls -al $now" . ".zip";

	printf("cmd = %s\n\n", $cmd);


	system($cmd);

	printf("\nfinished...\n");

?>
