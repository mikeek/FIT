<?php
	require("../connectDB.php");
	
	// init suma
	$arr = array("suma" => 0);
	
	// get pobyt
	$id_pobytu = intval($_GET["id"]);
	$result = mysql_query("SELECT * FROM Pobyt_izba WHERE id_pobytu = '$id_pobytu'");
	
	// radek sql -> pole
	$res = mysql_fetch_assoc($result);
	
	// suma += cena pobytu
	$arr['suma'] += $res['cena'];
	
	// get vsechny sluzby k pobytu
	$result = mysql_query("SELECT SUM(cena) AS cena_sluzeb FROM Sluzby_pobyt WHERE id_pobytu = '$id_pobytu'");

	// radek sql -> pole
	$res= mysql_fetch_assoc($result);
	$arr['suma'] += $res['cena_sluzeb'];
	
	mysql_close();
	
	echo json_encode($arr);
?>