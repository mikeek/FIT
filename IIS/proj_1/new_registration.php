<?php
	session_start(); // Starting Session
	$error=''; // Variable To Store Error Message
	
	if (isset($_POST['submit'])) {
		// Define $username and $password
		$name = $_POST['name'];
		$surname = $_POST['surname'];
		$phone = $_POST['phone'];
		$email = $_POST['email'];
		$password = $_POST['pass'];
		
		$name = stripslashes($name);
		$surname = stripslashes($surname);
		$phone = stripslashes($phone);
		$email = stripslashes($email);
		$password = stripslashes($password);

		if(!empty($phone) && !preg_match("/^(\+420)? ?[1-9][0-9]{2} ?[0-9]{3} ?[0-9]{3}$/", $phone)) {
  			$error="©patný formát telefonního èísla";
			return;
		}
		
		require("intra/connectDB.php");

		$sql_query = "SELECT id_klienta FROM Klient WHERE email = '$email'";

		$result = mysql_query($sql_query);
		$rows = mysql_num_rows($result);
		if ($rows == 0) {
			$sql_query = "INSERT INTO Klient(meno, priezvisko, telefon, email, heslo) VALUES ('$name', '$surname', '$phone', '$email', '$password')";
			if (!mysql_query($sql_query)) {
				$error= "Do¹lo k chybì, zkuste to prosím pozdìji";
			} else {
				$result = mysql_query("SELECT id_klienta FROM Klient WHERE email = '$email'");
				$data = mysql_fetch_array($result); 
				
				$_SESSION['username'] = $email;
				$_SESSION['role'] = "user";
				$_SESSION['user_id'] = $data['id_klienta'];
//				Presmerovanie do klientskej casti				
				header( 'Location: intra/user/index.php' ) ;
			}
		} else {
			$error= "U¾ivatel s emailem \"$email\" ji¾ existuje";
		}
	}
?>