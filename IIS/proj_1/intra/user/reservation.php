<?php include_once("session_check.php"); 
	include("new_rezervation.php");
?>

<html lang="en">
  <?php include_once("head.php"); ?>

	<link href="../../css/jquery-ui-1.10.1.min.css" rel="stylesheet" />
  <body class="reservation">

    <?php include_once("top_navbar.php"); ?>

    <div class="container-fluid">
      <div class="row">
		
		<?php  include_once("left_navbar.php") ?>
        
        <div class="col-sm-9 col-sm-offset-3 col-md-10 col-md-offset-2 main">
          <h1 class="page-header">Rezervace pokoje</h1>
		  
		   <?php include_once("../status.php"); ?>
		  
		<!-- Reservation form -->
		<form class="form-horizontal" role="form" method="post" action="reservation.php">
			<fieldset>
			
			<!-- FROM -->
			<div class="form-group">
				<label class="col-md-4 control-label" for="from">od - do</label>
				<div class="col-md-2">
					<input id="from" name="from" placeholder="dd.mm.yyyy" type="date" class="form-control input-md" required>
				</div>
				<div class="col-md-2">
					<input id="to" name="to" placeholder="dd.mm.yyyy" type="date" class="form-control input-md" required>
				</div>
				
			</div>
			
			<!-- Room type -->
			
			<div class="form-group">
				<label class="col-md-4 control-label" for="room">Pokoj</label>
				<div class="col-md-4">
					<?php
						require("../connectDB.php");
						$result = mysql_query("SELECT DISTINCT(typ) FROM Izba") or die(mysql_error());					
					?>
					<select id="room" name="room" class="form-control" required>
						<option>Vyberte typ pokoje</option>
						<?php
							$counter = 1;
							while($row = mysql_fetch_array($result)) {
								echo ' <option value="' . $counter .'">' . $row['typ'] . '</option>';
								$counter++;
							}
						?>
					</select>
				</div>
			</div>
			
			<!-- Note -->
			<div class="form-group">
				<label class="col-md-4 control-label" for="note">Poznámka</label>
				<div class="col-md-4">                     
					<textarea class="form-control" id="note" name="note"></textarea>
				</div>
			</div>

			<!-- Submit -->
			<div class="form-group">
				<label class="col-md-4 control-label" for="submit"></label>
					<div class="col-md-4">
						<button id="submit" name="submit" class="btn btn-success">Potvrdit rezervaci</button>			
					</div>
			</div>
			
			</fieldset>
		</form>
		</div>
	  </div>
	</div>

	  <!-- Bootstrap core JavaScript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
	<?php include_once("scripts.php"); ?>
    <script src="http://code.jquery.com/jquery-1.10.2.js"></script>
	<script src="http://code.jquery.com/ui/1.11.2/jquery-ui.js"></script>
    <!-- IE10 viewport hack for Surface/desktop Windows 8 bug -->
    <script src="../../assets/js/ie10-viewport-bug-workaround.js"></script>
	
	<script src="../../js/jquery-1.9.1.min.js"></script>
	<script src="../../js/jquery-ui-1.10.1.min.js"></script>
	  <script src="../../js/modernizr-2.6.2.min.js"></script>
	  
	  
 <script>
	if (!Modernizr.inputtypes.date) {
    $('input[type=date]').datepicker({
        dateFormat: 'dd.mm.yy'
    });
	}

</script>
    
  </body>
</html>

