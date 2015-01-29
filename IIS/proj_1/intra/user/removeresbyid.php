<?php
	include("session_check.php");
	require("../connectDB.php");
	$uid = intval($_GET["id"]);
	$from = $_GET["from"];
	$to = $_GET["to"];
	$id_klienta = $_SESSION['user_id'];

	$sql_query = "SELECT izba FROM Pobyt WHERE osoba = '$id_klienta' AND zaciatok = '$from' AND koniec = '$to'";

	$result = mysql_query($sql_query);
	$row = mysql_fetch_array($result);
	$id_room = $row[0];

	$sql_query = "DELETE FROM Rezervacia WHERE id_rezervacie = '$uid'";

	mysql_query($sql_query);


	$sql_query = "DELETE FROM Pobyt WHERE osoba = '$id_klienta' AND zaciatok = DATE('$from') AND koniec = DATE('$to') AND izba = '$id_room'";


	mysql_query($sql_query) or die(mysql_error());
	
	mysql_close();
?>