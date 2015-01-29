<?php
	if (!isset($_SESSION)) {
		session_start();
	}
	if (!isset($_SESSION['username']) || $_SESSION['role'] !== "user") {
		header("Location:../../index.php");
	}
?>