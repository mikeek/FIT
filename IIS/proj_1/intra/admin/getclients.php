<?php
	require("../connectDB.php");
	$result = mysql_query("SELECT * FROM Klient");
	
	$rows = array();
	while ($r = mysql_fetch_object($result)) {
    	$rows[] = $r;
	}

	mysql_close();
	echo json_encode($rows);
?>