<?php
	require("../connectDB.php");
	$uid = intval($_GET["id"]);
	mysql_query("DELETE FROM Rezervacia WHERE id_rezervacie = '$uid'");
	
	mysql_close();
?>