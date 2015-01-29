<?php
	require("../connectDB.php");
	$uid = intval($_GET["id"]);
	$result = mysql_query("SELECT typ, popis, cena FROM Prehlad_sluzieb WHERE id_pobytu = '$uid'");
	$rows = array();
	while ($r = mysql_fetch_object($result)) {
    	$rows[] = $r;
	}

	mysql_close();
	
	echo json_encode($rows);
?>