<?php include_once("session_check.php"); ?>

<html lang="en">
  <?php	include_once("head.php"); ?>

  <body class="rooms">

    <?php include_once("top_navbar.php"); ?>

    <div class="container-fluid">
      <div class="row">
		
		<?php include_once("left_navbar.php") ?>
        
        <div class="col-sm-9 col-sm-offset-3 col-md-10 col-md-offset-2 main">
          <h1 class="page-header">Seznam pokojù</h1>
		  
          <div class="table-responsive">
            <table id="table" class="table table-hover" data-sort-name="id_izby" data-search="true" data-height="500" data-pagination="true" data-page-size="25">
              <thead>
                <tr>
                  <th data-sortable="true" data-field="id_izby">ID pokoje</th>
                  <th data-sortable="true" data-field="cena">Cena</th>
                  <th data-sortable="true" data-field="typ">Typ</th>
				  <!--<th data-sortable="true" data-field="volny">VolnÃ½</th>-->
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
		)
	}
	
	function run() {
		$.getJSON(
			"getrooms.php",
			{},
			show
		);
		
		$(window).resize(function () {
			$('#table').bootstrapTable('resetView');
		});
		
    };
	
	window.onload=run;
	
	</script>
	
    <!-- Bootstrap core JavaScript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
  <?php	include_once("scripts.php"); ?>
  </body>
</html>
