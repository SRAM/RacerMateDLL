#!/usr/bin/php
<?php

	/*
The first character indicates what is happening to the file:

    < means that a file is being transferred to the remote host (sent).
    > means that a file is being transferred to the local host (received).
    c means that a local change/creation is occurring for the item (such as the creation of a directory or the changing of a symlink, etc.).
    h means that the item is a hard link to another item (requires --hard-links).
    . means that the item is not being updated (though it might have attributes that are being modified).
    * means that the rest of the itemized-output area contains a message (e.g. "deleting").

The second character indicates what type of directory entry it is. Specifically:

    f for file
    d for directory
    L for symbolic link
    D for device
    S for special file (e.g. socket or fifo)

The remaining columns are described below:

    c means either that a regular file has a different checksum or that a symlink, device, or special file has a changed value.
    s means the size of a regular file is different and will be updated by the file transfer.
    t or T:
        t means the modification time is different and is being updated to the sender's value
        T means that the modification time will be set to the transfer time
    p means the permissions are different and are being updated to the sender's value
    o means the owner is different and is being updated to the sender's value
    g means the group is different and is being updated to the sender's value
    . unused

The following columns may not be present, depending on your transfer options

    a means that the ACL information changed
    x means that the extended attribute information changed

	*/


//echo "hello from _fs\n";
//exit;


	$dryrun = false;

	// figure out who's online, if anyone (vaio win7 or vaio linux)
	

	$linux = "larry@vaio_wired::fs/";
	$win7 = "larry@vaio_wired::fs/";
	//$win7 = "larry@vaio_win7::fs/";

	$dest = $win7;

	//      source                destination       exclude_file
	//backup("/home/larry/_fs",		$dest,				"c_excludes.txt");

	//$dest = "larry@shza:/_fs";
	
	//backup("/Users/larry/_fs",		"/Volumes/_fs",				"c_excludes.txt");
	backup("/Volumes/_fs/glib",		"/Users/larry/_fs/glib",				"c_excludes.txt");
	//backup("/Users/larry/_fs/glib",		"/Users/larry/mac_fs/glib",				"c_excludes.txt");
	exit;

/***********************************************************************************************

***********************************************************************************************/

function backup($src, $dest, $excludes)  {
	global $dryrun;


	if ($dryrun)  {
		echo "dryrun = true\n";
	}
	else  {
		echo "dryrun = false\n";
	}


	if (!ends_with("/", $src))  {
		$src .= "/.";
	}

	printf("\n%s ======================> %s\n", $src, $dest);


	$quiet = false;
	$recursive = false;
	$compress = false;
	$verbose = false;
	$relative = false;
	$itemize = false;
	$update = false;
	$delete = false;


	$options = "ClptoD";

	$recursive = true;
	$compress = true;
//	$verbose = true;
//	$relative = true;
	$update = true;
	$delete = true;


	if ($dryrun)  {
		$quiet = false;
		$itemize = true;
	}
	else  {
//		$quiet = true;
		$itemize = true;
	}

	if ($recursive)  {
		$options .= "r";
	}

	if ($dryrun)  {
		$options .= "n";
	}

	if ($compress)  {
		$options .= "z";
	}

	if ($verbose)  {
		$options .= "v";
	}

	if ($quiet)  {
		$options .= "q";
	}

	if ($relative)  {
		$options .= "R";
	}

	if ($itemize)  {
		$options .= "i";
	}

	if ($update)  {
		$options .= "u";
	}


	// --out-format=""

	if ($delete)  {
		$options .= " --delete";
	}

	$options .= " --exclude-from=$excludes";

	$cmd = "rsync -$options $src $dest";

	//echo "\ncmd = $cmd\n\n";

	system($cmd);

	return;
}

// from ...../funcs/string.php:

function starts_with($needle, $haystack, $case=true) {
    if($case){return (strcmp(substr($haystack, 0, strlen($needle)),$needle)===0);}
    return (strcasecmp(substr($haystack, 0, strlen($needle)),$needle)===0);
}

function ends_with($needle, $haystack, $case=true) {
    if($case){return (strcmp(substr($haystack, strlen($haystack) - strlen($needle)),$needle)===0);}
    return (strcasecmp(substr($haystack, strlen($haystack) - strlen($needle)),$needle)===0);
}



?>

