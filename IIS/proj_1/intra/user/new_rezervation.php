<?php
	if (!isset($_SESSION)) {
		session_start();
	}
	
	$error = '';
	$success = '';
	$id_izby = -1;
	$user = $_SESSION['username'];
	$id_klienta = $_SESSION['user_id'];
	
	if (isset($_POST['submit'])) {		
		$from = $_POST['from'];
		$to = $_POST['to'];
		$id_room = $_POST['room'];
		$note = $_POST['note'];
		
		$from = date('Y-m-d',strtotime(stripslashes($from)));
		$to = date('Y-m-d',strtotime(stripslashes($to)));
		$id_room = stripslashes($id_room);
		$note = stripslashes($note);

		require("../connectDB.php");
		
		if ($id_room == "Vyberte typ pokoje") {
			$error = "Musite vybrat typ izby!";
			return;
		}
		
		if (strtotime($from) > strtotime($to)) {
			$error = "Datum bol zadany nekorektne!";
			return;
		}

		if (strtotime($from) < time() || strtotime($to) < time()) {
			$error = "Prajete si vytvorit rezervaciu v minulosti, bohuzial to nas hotel nepodporuje.";
			return;
		}
							
		switch ($id_room) {
			case 1:
				$room = "Standart";
				$base_id = 100;
				break;
			case 2:
				$room = "Classic";
				$base_id = 200;
				break;
			case 3:
				$room = "Exclusiv";
				$base_id = 300;
				break;
			default:
				$room = "Prezident";
				$base_id = 500;
				break;
		}
			
			$sql_query = "SELECT id_izby FROM Rezervacia_Izba WHERE NOT (STR_TO_DATE('$from', '%Y-%m-%d') > do  OR STR_TO_DATE('$to', '%Y-%m-%d') < od ) AND typ = '$room'";
			$result = mysql_query($sql_query);
			$rows = mysql_num_rows($result);
			if ($rows == 0) {
				$sql_query = "INSERT INTO Rezervacia(od, do, id_klienta, id_izby, poznamka) VALUES(DATE('$from'), DATE('$to'), '$id_klienta',  '$base_id', '$note' )"; 
				$result = mysql_query($sql_query) or die(mysql_error());
				$sql_query = "INSERT INTO Pobyt(zaciatok, koniec, platba, osoba, izba) VALUES(DATE('$from'), DATE('$to'), 0, '$id_klienta', '$base_id' )";
				$result = mysql_query($sql_query) or die(mysql_error());
				$success = "Registracia bola vlozena do DB";
			} else {
				if ($rows <= 10) {
					$counter = 1;
					while($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
						foreach ($row as $key) {
							if ($counter == $rows) {
								$id_izby = $key;
							}
							$counter++;
						}
					}
					$id_izby++;
					$sql_query = "INSERT INTO Rezervacia(od, do, id_klienta, id_izby, poznamka) VALUES(DATE('$from'), DATE('$to'), '$id_klienta',  '$id_izby', '$note' )"; 
					$result = mysql_query($sql_query) or die(mysql_error());
					$sql_query  = "INSERT INTO Pobyt(zaciatok,koniec,platba,osoba,izba) VALUES(DATE('$from'), DATE('$to'), 0, '$id_klienta', '$id_izby')";
					$result = mysql_query($sql_query) or die(mysql_error());
					$success = "Registracia bola vlozena do DB";
				} else {
					$error = "Tento typ izby nieje dostupny v datume $from - $to.";
					
				}
			}


		mysql_close();
	}
?>