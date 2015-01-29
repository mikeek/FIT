<?php
	include("login_check.php");
?>

<!DOCTYPE html>
<html lang="en">
  <head>
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta name="description" content="">
    <meta name="author" content="">

    <title>Hotel IIS - pøihlá¹ení</title>

    <!-- Bootstrap core CSS -->
    <link href="css/bootstrap.min.css" rel="stylesheet">

    <!-- Custom styles for this template -->
    <!--<link href="signin.css" rel="stylesheet">-->
	
	<!-- Custom styles for this template -->
     <link href="crossroads.css" rel="stylesheet">

    <!-- Just for debugging purposes. Don't actually copy these 2 lines! -->
    <!--[if lt IE 9]><script src="../../assets/js/ie8-responsive-file-warning.js"></script><![endif]-->
    <script src="assets/js/ie-emulation-modes-warning.js"></script>

    <!-- HTML5 shim and Respond.js IE8 support of HTML5 elements and media queries -->
    <!--[if lt IE 9]>
      <script src="https://oss.maxcdn.com/html5shiv/3.7.2/html5shiv.min.js"></script>
      <script src="https://oss.maxcdn.com/respond/1.4.2/respond.min.js"></script>
    <![endif]-->
  </head>

  <body>
	<div class="fill nopadding mainbody">
		<?php include_once("header.html"); ?>

		<form class="form-horizontal" role="form" method="post">
		  <fieldset>

			<!-- Form Name -->
			<legend>Pøihlá¹ení do IS</legend>
			
			<!-- Login -->
			<div class="form-group">
				<label class="col-md-4 control-label" for="name">Login</label>  
				<div class="col-md-4">
					<input id="username" name="username" type="text" class="form-control input-md" 
						   <?php if (isset($error) && !empty($error)) echo 'value="' . $_POST['username'] . '"' ?>
						   required>
				</div>
			</div>
			
			<!-- Password -->
			<div class="form-group">
				<label class="col-md-4 control-label" for="phone">Heslo*</label>  
				<div class="col-md-4">
					<input id="password" name="password" type="password" class="form-control input-md" required>
				</div>
			</div>
			
			<!-- Submit -->
			<div class="form-group">
				<label class="col-md-4 control-label" for="submit"></label>
				<div class="col-md-4">
					<button id="submit" name="submit" class="btn btn-success btn-block">Pøihlásit se</button>
				</div>
			</div>
			
			<div class="form-group">
				<label class="col-md-4 control-label" for="reg"></label>
				<div class="col-md-4">
					<a href="registration.php" id="submit" name="reg" class="btn btn-info btn-block">Registrovat se</a>
				</div>
			</div>

		  <?php include_once("intra/status.php") ?>
		  </fieldset>
		</form>

	</div>

	<!-- FOOTER -->
	<?php include_once("footer.html"); ?>
    <!-- IE10 viewport hack for Surface/desktop Windows 8 bug -->
    <script src="assets/js/ie10-viewport-bug-workaround.js"></script>
  </body>
</html>