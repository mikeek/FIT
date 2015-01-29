<?php
	require("../connectDB.php");
	$uid = intval($_GET["id"]);
	$result = mysql_query("SELECT * FROM Rezervacie_all WHERE id_klienta = '$uid'");
//	$result = mysql_query("SELECT * FROM Rezervacia");
	$rows = array();
	while ($r = mysql_fetch_object($result)) {
    	$rows[] = $r;
	}

	mysql_close();
	
	echo json_encode($rows);
?>