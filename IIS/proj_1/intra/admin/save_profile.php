<?php 
	if (!isset($_SESSION)) {
		session_start();
	}

	if (isset($_POST['submit'])) {
		$error = '';
		$success = '';
		
		require("../connectDB.php");

		$user_id = $_POST['id_zam'];
		$name = stripcslashes($_POST['name']);
		$surname = stripcslashes($_POST['surname']);
		
		$sql_query = "UPDATE Zamestnanec SET meno = '$name', priezvisko = '$surname' WHERE id_zamestnanca = '$user_id'";
		$result = mysql_query($sql_query) or die(mysql_error());
		$success = "Zmena profilu bola ulozena.";

		mysql_close();
	}
	
?>