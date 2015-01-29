<?php 
include_once("session_check.php"); 
include_once("save_edited_reservation.php");
?>

<html lang="en">
  <head>
    <?php	include_once("head.php"); ?>

    <link href="../../css/jquery-ui-1.10.1.min.css" rel="stylesheet" />
  <body class="reservations">

    <?php include_once("top_navbar.php"); ?>

    <div class="container-fluid">
      <div class="row">
    
    <?php include_once("left_navbar.php") ?>
        
        <div class="col-sm-9 col-sm-offset-3 col-md-10 col-md-offset-2 main">
          <h1 class="page-header">Seznam rezervací</h1>
		  <?php include_once("../status.php"); ?>
      
          <div class="table-responsive">
            <table id="table" class="table table-hover" data-sort-name="id_rezervacie" data-search="true" data-height="500" data-pagination="true" data-page-size="25">
              <thead>
                <tr>
                  <th data-sortable="true" data-field="id_rezervacie">ID</th>
                  <th data-sortable="true" data-field="meno">Jméno</th>
                  <th data-sortable="true" data-field="priezvisko">Pøijmení</th>
                  <th data-sortable="true" data-field="id_izby">Pokoj</th>
				  <th data-sortable="true" data-field="typ">Typ</th>
                  <th data-sortable="true" data-field="od">Od</th>
                  <th data-sortable="true" data-field="do">Do</th>
				  <th data-sortable="true" data-field="poznamka">Pozn.</th>
				  <th data-field="operate" data-formatter="operateFormatter" data-events="operateEvents">Úprava</th>
                </tr>
              </thead>
            </table>
          </div>
		  
		  <!-- Edited reservation -->
          <form  name="res_form" id="res_form" class="form-horizontal" role="form" method="post">
			<a name="editation"></a>
			  <fieldset>
			  <legend> Editace rezervace </legend>
            <!-- Reservation ID -->
            <div class="form-group">
                <label class="col-md-4 control-label" for="id_rezervacie">ID Rezervacie</label>  
                <div class="col-md-4">
                    <input id="id_rezervacie" name="id_rezervacie" type="text" class="form-control input-md" readonly>
                </div>
            </div>

            <!-- Client ID -->
            <div class="form-group">
                <label class="col-md-4 control-label" for="meno">Jméno</label>  
                <div class="col-md-4">
                    <input id="meno" name="meno" type="text" class="form-control input-md" readonly>
                </div>
            </div>
			
			<div class="form-group">
                <label class="col-md-4 control-label" for="priezvisko">Pøijmení</label>  
                <div class="col-md-4">
                    <input id="priezvisko" name="priezvisko" type="text" class="form-control input-md" readonly>
                </div>
            </div>
			
			<div class="form-group">
				<label class="col-md-4 control-label" for="typ">Pokoj</label>
				<div class="col-md-4">
					<?php
						require("../connectDB.php");
						$result = mysql_query("SELECT DISTINCT(typ) FROM Izba") or die(mysql_error());					
					?>
					<select id="typ" name="typ" class="form-control" required>
						<?php
							$counter = 0;
							while($row = mysql_fetch_array($result)) {
								echo ' <option>' . $row['typ'] . '</option>';
								$counter++;
							}
						?>
					</select>
				</div>
			</div>

            <!-- From -->
            <div class="form-group">
                <label class="col-md-4 control-label" for="od">Od</label>
                <div class="col-md-4">
                    <input id="od" name="od" type="date" class="form-control input-md" required>
                </div>
            </div>

             <!-- To -->
            <div class="form-group">
                <label class="col-md-4 control-label" for="do">Do</label>
                <div class="col-md-4">
                    <input id="do" name="do" type="date" class="form-control input-md" required>
                </div>
            </div>

             <!-- Note -->
            <div class="form-group">
                <label class="col-md-4 control-label" for="poznamka">Poznámka</label>
                <div class="col-md-4">
                    <input id="poznamka" name="poznamka" type="text" class="form-control input-md" >
                </div>
            </div>

            <!-- Submit -->
            <div class="form-group">
                <label class="col-md-4 control-label" for="submit"></label>
                <div class="col-md-2">
                    <button id="submit" name="submit" class="btn btn-success btn-block">Upravit</button>      
                </div>
                <div class="col-md-2">
                    <input id="cancel" name="cancel" class="btn btn-danger btn-block" type="button" onclick="$('#res_form').hide(300);" value="Zru¹it úpravy"></input>      
                </div>
            </div>
			</fieldset>
		  </form>
        </div>
      </div>
    </div>



  <script language="javascript">
	  
	function operateFormatter(value, row, index) {
        return [
            '<a class="edit ml10" href="javascript:void(0)" title="Edit">',
                '<i class="glyphicon glyphicon-edit"></i>',
            '</a>',
            '<a class="remove ml10" href="javascript:void(0)" title="Remove">',
                '<i class="glyphicon glyphicon-remove"></i>',
            '</a>'
        ].join('');
    }
	  
  function show(json_data) {
    $('#table').bootstrapTable(
      {data: json_data}
	).bootstrapTable('load', json_data
    ).bootstrapTable('resetView'
	);
  }
  
  function run() {
	  $('#res_form').hide();
    $.getJSON(
      "getreservations.php",
      {},
      show
    );
    
    $(window).resize(function () {
      $('#table').bootstrapTable('resetView');
    });
    
    };
  
  function fillform(json_data) {
	  console.log(json_data);
	  
	  for(key in json_data) {
	  if(json_data.hasOwnProperty(key))
		$('input[name='+key+']').val(json_data[key]);

	  $('select[name="typ"]').val(json_data['typ']);
	}
	$('#res_form').show(300);
//	  	$('form').loadJSON(json_data);
  };
  
  window.onload=run;
  
  window.operateEvents = {
        'click .edit': function (e, value, row, index) {
            fillform(row);
			window.location.hash="editation";
        },
        'click .remove': function (e, value, row, index) {
			if (confirm("Opravdu chcete zrusit rezervaci?")) {
				$.get("removeresbyid.php", {id: row['id_rezervacie']}, run);
			}
        }
    };
  
  
  </script>

   <!-- Bootstrap core JavaScript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
  <?php include_once("scripts.php"); ?>
  <script src="http://code.jquery.com/ui/1.11.2/jquery-ui.js"></script>
    <!-- IE10 viewport hack for Surface/desktop Windows 8 bug -->
    <script src="../../assets/js/ie10-viewport-bug-workaround.js"></script>
    
 <script>
  if (!Modernizr.inputtypes.date) {
    $('input[type=date]').datepicker({
        dateFormat: 'dd.mm.yy'
    });
  }

</script>
   
  </body>
</html>
