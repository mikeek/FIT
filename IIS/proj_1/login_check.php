<?php
	session_start(); // Starting Session
	$error=''; // Variable To Store Error Message
	if (isset($_POST['submit'])) {
		// Define $username and $password
		$username = $_POST['username'];
		$password = $_POST['password'];
		
		$username = stripslashes($username);
		$password = stripslashes($password);
	
		require("intra/connectDB.php");

		$result = mysql_query("SELECT id_zamestnanca FROM Zamestnanec WHERE login = '$username' AND heslo = '$password'");
		$rows = mysql_num_rows($result);
		if ($rows == 1) {
			$_SESSION['username']=$username; 
			$_SESSION['role'] = "admin";
			$row = mysql_fetch_assoc($result);
			$_SESSION['user_id'] = $row['id_zamestnanca'];
			header( 'Location: intra/admin/index.php' ) ;
		} else {
			$result = mysql_query("SELECT id_klienta FROM Klient WHERE email = '$username' AND heslo = '$password'");
			$rows = mysql_num_rows($result);
			if ($rows == 1) {
				$_SESSION['username'] = $username; 
				$_SESSION['role'] = "user";
				$row = mysql_fetch_assoc($result);
				$_SESSION['user_id'] = $row['id_klienta'];
//				Presmerovanie do klientskej casti				
				header( 'Location: intra/user/index.php' ) ;
			} else {
				$error = "Neplatné u¾ivatelské jméno nebo heslo";
			}
		}
	}
?>