<?php
	include("new_registration.php");
?>

<!DOCTYPE html>
<html lang="en">
  <head>
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta name="description" content="">
    <meta name="author" content="">

    <title>Hotek IIS - registrace</title>

    <!-- Bootstrap core CSS -->
    <link href="css/bootstrap.min.css" rel="stylesheet">

	<script src="jquery-1.9.1.min.js"></script>
	<script src="jquery-ui-1.10.1.min.js"></script>

    <script src="assets/js/ie-emulation-modes-warning.js"></script>

    <!-- HTML5 shim and Respond.js IE8 support of HTML5 elements and media queries -->
    <!--[if lt IE 9]>
      <script src="https://oss.maxcdn.com/html5shiv/3.7.2/html5shiv.min.js"></script>
      <script src="https://oss.maxcdn.com/respond/1.4.2/respond.min.js"></script>
    <![endif]-->

    <!-- Custom styles for this template -->
     <link href="crossroads.css" rel="stylesheet">
  </head>
<!-- NAVBAR
================================================== -->
  <body>

	<!-- Main navigator -->
	<div class="fill nopadding mainbody">
		<?php include_once("header.html"); ?>
		
		<!-- Reservation form -->
		<form class="form-horizontal" role="form" method="post">
			<fieldset>

			<!-- Form Name -->
			<legend>Registrace klienta</legend>

			<!-- Name -->
		
			<div class="form-group">
				<label class="col-md-4 control-label" for="name">Jméno*</label>  
				<div class="col-md-4">
					<input id="name" name="name" type="text" class="form-control input-md" 
						   <?php if (isset($error) && !empty($error)) echo 'value="' . $_POST['name'] . '"' ?>
						   required>
				</div>
			</div>

			<!-- Surname -->
			<div class="form-group">
				<label class="col-md-4 control-label" for="surname">Pøijmení*</label>  
				<div class="col-md-4">
					<input id="surname" name="surname" type="text" class="form-control input-md" 
						   <?php if (isset($error) && !empty($error)) echo 'value="' . $_POST['surname'] . '"' ?>
						   required>
				</div>
			</div>

			<!-- Phone number -->
			<div class="form-group">
				<label class="col-md-4 control-label" for="phone">Telefon</label>  
				<div class="col-md-4">
					<input id="phone" name="phone" placeholder="+420 123 456 789" type="tel" class="form-control input-md"
						   <?php if (isset($error) && !empty($error)) echo 'value="' . $_POST['phone'] . '"' ?>
						   >
				</div>
			</div>

			<!-- Email address -->
			<div class="form-group">
				<label class="col-md-4 control-label" for="email">Email*</label>
				<div class="col-md-4">
					<input id="email" name="email" type="email" class="form-control input-md" 
						   <?php if (isset($error) && !empty($error)) echo 'value="' . $_POST['email'] . '"' ?>
						   required>
				</div>
			</div>
			
			<!-- Password -->
			<div class="form-group">
				<label class="col-md-4 control-label" for="phone">Heslo*</label>  
				<div class="col-md-4">
					<input id="pass" name="pass" type="password" class="form-control input-md" required>
				</div>
			</div>

			<!-- Submit -->
			<div class="form-group">
				<label class="col-md-4 control-label" for="submit"></label>
					<div class="col-md-4">
						<button id="submit" name="submit" class="btn btn-success btn-block">Potvrdit</button>			
					</div>
			</div>
	
			 <?php include_once("intra/status.php"); ?>
			
			</fieldset>
		</form>

	</div>

	<!-- FOOTER -->
     <?php include_once("footer.html"); ?>
    <!-- Bootstrap core JavaScript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>
    <script src="js/bootstrap.min.js"></script>
    <script src="assets/js/docs.min.js"></script>
    <!-- IE10 viewport hack for Surface/desktop Windows 8 bug -->
    <script src="assets/js/ie10-viewport-bug-workaround.js"></script>
  </body>
</html>

