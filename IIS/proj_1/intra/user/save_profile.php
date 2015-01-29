<?php 
	if (!isset($_SESSION)) {
		session_start();
	}

	if (isset($_POST['submit'])) {
		$error = '';
		$success = '';
		
		require("../connectDB.php");

		$user_id = $_SESSION['user_id'];
		$name = stripcslashes($_POST['name']);
		$surname = stripcslashes($_POST['surname']);
		$phone = stripcslashes($_POST['phone']);

		if(!empty($phone) && !preg_match("/^(\+420)? ?[1-9][0-9]{2} ?[0-9]{3} ?[0-9]{3}$/", $phone)) {
  			$error="Zly format telefonneho cisla";
			return;
		}
		
		$sql_query = "UPDATE Klient SET meno = '$name', priezvisko = '$surname', telefon = '$phone' WHERE id_klienta = '$user_id'";
		$result = mysql_query($sql_query) or die(mysql_error());
		$success = "Zmena profilu bola ulozena.";

		mysql_close();
	}
	
?>