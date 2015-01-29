<?php 
	if (!isset($_SESSION)) {
		session_start();
	}

	if (isset($_POST['submit'])) {
		$error = '';
		$success = '';
		$base_id = 0;
		$room = $_POST['typ'];

		require("../connectDB.php");

		$reservation_id = $_POST['id_rezervacie'];
		$from = stripcslashes($_POST['od']);
		$to = stripcslashes($_POST['do']);
		$note = stripcslashes($_POST['poznamka']);
		$from = date('Y-m-d',strtotime($from));
		$to = date('Y-m-d',strtotime($to));

		if (strtotime($from) > strtotime($to)) {
			$error = "Datum bol zadany nekorektne!";
			return;
		}

		if (strtotime($from) < time() || strtotime($to) < time()) {
			$error = "Prajete si vytvorit rezervaciu v minulosti, bohuzial to nas hotel nepodporuje.";	
			return;
		}
	
		switch ($room) {
			case "Standart":
				$base_id = 100;
				break;
			case "Classic":
				$base_id = 200;
				break;
			case "Exclusiv":
				$base_id = 300;
				break;
			default:
				$base_id = 500;
				break;
		}
		$sql_query = "SELECT od, do, id_klienta, id_izby  FROM Rezervacia WHERE id_rezervacie = '$reservation_id' ";
		$result = mysql_query($sql_query);
		$rows = mysql_fetch_assoc($result);

		$tmp_od = $rows['od'];
		$tmp_do = $rows['do'];
		$tmp_id_klienta = $rows['id_klienta'];
		$tmp_id_izby = $rows['id_izby'];

		$sql_query = "SELECT id_pobytu FROM Pobyt WHERE zaciatok = '$tmp_od' AND koniec = '$tmp_do' AND osoba = '$tmp_id_klienta' AND izba = '$tmp_id_izby'";
		
		$result = mysql_query($sql_query);
		$row = mysql_fetch_assoc($result);
		$id_pobytu = $row['id_pobytu'];

		$sql_query = "SELECT id_izby FROM Rezervacia_Izba WHERE NOT (DATE('$from') > do  OR DATE('$to') < od ) AND typ = '$room'";
		$result = mysql_query($sql_query);
		$rows = mysql_num_rows($result);

		if ($rows == 0) {
			$sql_query = "UPDATE Rezervacia SET od = DATE('$from'), do = DATE('$to'),  poznamka = '$note', id_izby = '$base_id' WHERE id_rezervacie = '$reservation_id'"; 
			$result = mysql_query($sql_query) or die(mysql_error());

			$sql_query = "UPDATE Pobyt SET zaciatok = DATE('$from'), koniec = DATE('$to'), izba = '$base_id' WHERE id_pobytu = '$id_pobytu'";
			$result = mysql_query($sql_query) or die(mysql_error());
			$success = "Rezervacia bola zmenena";
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
				$sql_query = "UPDATE Rezervacia SET od = DATE('$from'), do = DATE('$to'),  poznamka = '$note', id_izby = '$id_izby' WHERE id_rezervacie = '$reservation_id'"; 
				$result = mysql_query($sql_query) or die(mysql_error());

				$sql_query = "UPDATE Pobyt SET zaciatok = DATE('$from'), koniec = DATE('$to'), izba = '$id_izby' WHERE id_pobytu = '$id_pobytu'";
				$result = mysql_query($sql_query) or die(mysql_error());
				$success = "Rezervacia bola zmenena";
			} else {
				$error = "Tento typ izby nieje dostupny v datume $from - $to.";
				
			}
		}
		mysql_close();
	}
	
?>