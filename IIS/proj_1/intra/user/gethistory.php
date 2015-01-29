<?php
	if (!isset($_SESSION)) {
		session_start();
	}
	require("../connectDB.php");

	$id_klienta  = $_SESSION['user_id'];
	$sql_query = "SELECT * FROM History WHERE osoba = '$id_klienta'";
	$result = mysql_query($sql_query);
	

	$rows = array();
	while ($row = mysql_fetch_object($result)) {
		$rows[] = $row;
	}

	mysql_close();
	echo json_encode($rows);
?>