 <?php
	if (isset($error) && !empty($error)) {
		echo '<div class="alert alert-danger" role="alert">' . $error . '</div>';
	}
	if (isset($success) && !empty($success)) {
		echo '<div class="alert alert-success" role="alert">' . $success . '</div>';
	}
?>