<?php
	require("../connectDB.php");

	$room_type = $_GET['typ'];
	$from = $_GET['from'];
	$to = $_GET['to'];

	$from = date('Y-m-d',strtotime(stripcslashes($from)));
	$to = date('Y-m-d',strtotime(stripcslashes($to)));
	$rows = array();

	if (strtotime($from) > strtotime($to)) {
			$error = "Datum bol zadany nekorektne!";
			echo json_encode($rows);
			return;
	}

	if (strtotime($from) < time() || strtotime($to) < time()) {
		$error = "Prajete si vytvorit rezervaciu v minulosti, bohuzial to nas hotel nepodporuje.";
		echo json_encode($rows);
		return;
	}
	
	$sql_query = "SELECT * FROM Izba WHERE typ = '$room_type' and Izba.id_izby NOT IN (select id_izby from Rezervacia_Izba where (DATE('$from') > do or DATE('$to') < od ) and (typ = '$room_type'))";
	$result = mysql_query($sql_query);
		
	while ($r = mysql_fetch_object($result)) {
		$rows[] = $r;
	}

	mysql_close();
	echo json_encode($rows);
?>