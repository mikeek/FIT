<?php include_once("session_check.php"); 
	
	include("save_profile.php");
	require("../connectDB.php");
	
	$user = $_SESSION['username'];
	$query = mysql_query("SELECT meno, priezvisko, telefon, email FROM Klient WHERE email = '$user'");
	$userdata = mysql_fetch_array($query); 
?>

<html lang="en">
    <?php include_once("head.php"); ?>

  <body class="client">

    <?php include_once("top_navbar.php"); ?>

    <div class="container-fluid">
      <div class="row">
		
		<?php include_once("left_navbar.php") ?>
        
        <div class="col-sm-9 col-sm-offset-3 col-md-10 col-md-offset-2 main">
          <h1 class="page-header">Mùj profil</h1>
		
		  <?php include_once("../status.php"); ?>
		  
		  <!-- User profile -->
		  <form class="form-horizontal" role="form" method="post">
			
			<!-- Name -->
			<div class="form-group">
				<label class="col-md-4 control-label" for="name">Jméno</label>  
				<div class="col-md-4">
					<input id="name" name="name" type="text" class="form-control input-md" value="<?php echo $userdata['meno']?>" required>
				</div>
			</div>

			<!-- Surname -->
			<div class="form-group">
				<label class="col-md-4 control-label" for="surname">Pøijmení</label>  
				<div class="col-md-4">
					<input id="surname" name="surname" type="text" class="form-control input-md" value="<?php echo $userdata['priezvisko']?>" required>
				</div>
			</div>

			<!-- Phone number -->
			<div class="form-group">
				<label class="col-md-4 control-label" for="phone">Telefon</label>  
				<div class="col-md-4">
					<input id="phone" name="phone" placeholder="+420 123 456 789" type="text" class="form-control input-md" value="<?php echo $userdata['telefon']?>">
				</div>
			</div>

			<!-- Email address -->
			<div class="form-group">
				<label class="col-md-4 control-label" for="email">Email</label>
				<div class="col-md-4">
					<input id="email" name="email" type="email" class="form-control input-md" value="<?php echo $userdata['email']?>" readonly>
				</div>
			</div>

			<!-- Submit -->
			<div class="form-group">
				<label class="col-md-4 control-label" for="submit"></label>
					<div class="col-md-4">
						<button id="submit" name="submit" class="btn btn-success btn-block">Upravit</button>			
					</div>
			</div>
			<div class="form-group">
				<label class="col-md-4 control-label" for="submit"></label>
				<div class="col-md-4">
					<input onclick="removeClick()" type="button" id="remove" name="remove" class="btn btn-danger btn-block" value="Zru¹it úèet">
				</div>
		  </div>
		</form>
		  
        </div>
      </div>
    </div>
	  
    <!-- Bootstrap core JavaScript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
	<?php include_once("scripts.php"); ?>
	
	<script>
		
		function removeClick() {
			if (confirm("Opravdu chcete zru¹it úèet?")) {
				window.location.href= "removeclient.php";
			}
		}
	</script>
	
  </body>
</html>
