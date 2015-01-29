<?php include_once("session_check.php"); ?>

<html lang="en">
	<?php	include_once("head.php"); ?>

  <body class="clients">
<?php include_once("top_navbar.php"); ?>

    <div class="container-fluid">
      <div class="row">
    
    <?php include_once("left_navbar.php") ?>
        
        <div class="col-sm-9 col-sm-offset-3 col-md-10 col-md-offset-2 main">
          <h1 class="page-header">Seznam klientù</h1>
      
          <div class="table-responsive">
            <table id="table" class="table table-hover" data-card-view="true" data-sort-name="id_klienta" data-search="true" data-height="500" data-pagination="true" data-page-size="25">
              <thead>
                <tr>
                  <th data-sortable="true" data-field="id_klienta">ID</th>
                  <th data-sortable="true" data-field="meno">Jméno</th>
                  <th data-sortable="true" data-field="priezvisko">Pøijmení</th>
                  <th data-sortable="true" data-field="telefon">Telefon</th>
                  <th data-sortable="true" data-field="email">Email</th>
                </tr>
              </thead>
            </table>
          </div>
		  
		  <h2 class="page-header">Rezervace klienta</h2>
		  <div class="table-responsive">
            <table id="table_reserv" class="table table-hover" data-sort-name="od" data-search="true" data-height="300" data-pagination="true" data-page-size="10">
              <thead>
                <tr>
                  <th data-sortable="true" data-field="od">Od</th>
                  <th data-sortable="true" data-field="do">Do</th>
                  <th data-sortable="true" data-field="id_izby">Pokoj</th>
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
    ).on('click-row.bs.table', function (e, row, $element) {
		$.getJSON(
			"getresbyid.php",
			{ id: row['id_klienta']},
			showres
		);
	}
	);
  };
  
  function run() {
    $.getJSON(
      "getclients.php",
      {},
      show
    );
    
    $(window).resize(function () {
      $('#table').bootstrapTable('resetView');
    });
    
    };
  
  function showres(json_data) {
	  console.log(json_data);
	  $('#table_reserv').bootstrapTable(
			  {data: json_data}	  
		).bootstrapTable('load', json_data);
	};
  
  
  window.onload=run;
  
  </script>
    <!-- Bootstrap core JavaScript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
   	<?php	include_once("scripts.php"); ?>

  </body>
</html>
