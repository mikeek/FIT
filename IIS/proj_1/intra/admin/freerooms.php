<?php include_once("session_check.php"); ?>

<html lang="en">
	<?php	include_once("head.php"); ?>
	<link href="../../css/jquery-ui-1.10.1.min.css" rel="stylesheet" />
  <body class="freerooms">
<?php include_once("top_navbar.php"); ?>

    <div class="container-fluid">
      <div class="row">
    
    <?php include_once("left_navbar.php") ?>
        
        <div class="col-sm-9 col-sm-offset-3 col-md-10 col-md-offset-2 main">
          <h1 class="page-header">Seznam volných pokojù</h1>
		  <?php include_once("../status.php"); ?>
		  
		  <form name="filter_form" id="filter_form" class="form-horizontal" role="form">
			  <fieldset>
				  <div class="form-group">
					  <label class="col-md-2 control-label" for="room">Pokoj</label>
					  <div class="col-md-2">
						  <select id="typ" name="typ" class="form-control" required>
						  <?php
							require_once("../connectDB.php");
							$result = mysql_query("SELECT DISTINCT(typ) FROM Izba") or die(mysql_error());
							while ($row = mysql_fetch_array($result)) {
								echo '<option>' . $row['typ'] . '</option>';
							}
						   ?>
						  </select>
					  </div>

					  <label class="col-md-1 control-label" for="od">Od</label>
					  <div class="col-md-2">
						  <input id="od" name="od" placeholder="dd.mm.yyyy" type="date" class="form-control input-md" required>
					  </div>

					  <label class="col-md-1 control-label" for="do">Do</label>
					  <div class="col-md-2">
						  <input id="do" name="do" placeholder="dd.mm.yyyy" type="date" class="form-control input-md" required>
					  </div>
					  
					  <div class="col-md-2">
						  <input id="submit" name="submit" class="btn btn-success btn-block" onclick="run()" type="button" value="Zobrazit">
					  </div>
				  </div>
			  </fieldset>
		  </form>
		  
		  
		  
          <div class="table-responsive">
            <table id="table" class="table table-hover" data-sort-name="id_izby" data-search="true" data-height="500" data-pagination="true" data-page-size="25">
              <thead>
                <tr>
                  <th data-sortable="true" data-field="id_izby">ID</th>
                  <th data-sortable="true" data-field="cena">Cena</th>
                  <th data-sortable="true" data-field="typ">Typ</th>
                </tr>
              </thead>
            </table>
          </div>
		  </div>
      </div>
    </div>

  <script language="javascript">
  function show(json_data) {
    $('#table').bootstrapTable(
      {data: json_data}
    ).bootstrapTable('load', json_data
	).bootstrapTable('resetView');
  };
  
  function run() {
    $('#table').bootstrapTable( {data: {}} );
    $.getJSON(
      "getfreerooms.php",
      {typ: $('#typ').val(), from: $('#od').val(), to: $('#do').val()},
      show
    );
    
    $(window).resize(function () {
      $('#table').bootstrapTable('resetView');
    });
    
    };
  
  
  </script>
    <!-- Bootstrap core JavaScript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
   	<?php	include_once("scripts.php"); ?>

  </body>
  <!--<script src="http://code.jquery.com/jquery-1.10.2.js"></script>-->
  <!--<script src="http://code.jquery.com/ui/1.11.2/jquery-ui.js"></script>-->
  <!--<script src="../../js/jquery-1.9.1.min.js"></script>-->
  <script src="../../js/jquery-ui-1.10.1.min.js"></script>
  <script src="../../js/modernizr-2.6.2.min.js"></script>
  <script>
		if (!Modernizr.inputtypes.date) {
			$('input[type=date]').datepicker({
				dateFormat: 'dd.mm.yy'
			});
		}

	</script>
</html>
