<?php
	include("session_check.php");
	require("../connectDB.php");
	
	$id_klienta = $_SESSION['user_id'];

	mysql_query("UPDATE Zamestnanec SET login = 'ZRUSEN', heslo = null WHERE id_zamestnanca = '$id_klienta'");
	mysql_close();
	
	$_SESSION['role'] = "none";
	
	header('Location: ../../index.php');
?>