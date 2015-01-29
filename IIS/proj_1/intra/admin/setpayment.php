<?php 
	require("../connectDB.php");

	if(isset($_POST['submit'])){
		$success = '';
		$suma = $_POST['suma'];
		$id_pobytu  = $_POST['id_pobytu'];
		$type = $_POST['type'];
		$id_klienta = $_POST['osoba'];
		$user_id = $_SESSION['user_id'];

		if ($type == 0) {
			$pay_type = "Cash";
		} else {
			$pay_type = "Prevod";
		}

		$sql_query = "INSERT INTO Platba(suma, typ, prevzal, uhradil) VALUES ('$suma', '$pay_type', '$user_id', '$id_klienta')";
		$result = mysql_query($sql_query) or die(mysql_error());

		$sql_query = "SELECT id_platby FROM Platba ORDER BY id_platby DESC LIMIT 1";
		$result = mysql_query($sql_query);
		$row = mysql_fetch_assoc($result);
		$id_platby = $row['id_platby'];

		$sql_query = "UPDATE Pobyt SET platba = '$id_platby' WHERE id_pobytu = '$id_pobytu'";
		$result = mysql_query($sql_query) or die(mysql_error());
		$success = "Platba bola uspesne uhradena";

	}
?>