<?php
	if(!isset($_SESSION)) {
		session_start();
	}
	require("../connectDB.php");

	$today_date = date('Y-m-d');
	$user_id = $_SESSION['user_id'];

	$sql_query = "SELECT * FROM Rezervacie_all WHERE id_klienta = '$user_id' AND do >= DATE('$today_date')";
	$result = mysql_query($sql_query);
	$rows = array();

	while($row = mysql_fetch_object($result)) {
		$rows[] = $row;
	}

	mysql_close();
	echo json_encode($rows);
?>