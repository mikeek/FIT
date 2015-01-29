<?php include_once("session_check.php"); ?>

<html lang="en">
    <?php include_once("head.php"); ?>

  <body class="history">

    <?php include_once("top_navbar.php"); ?>

    <div class="container-fluid">
      <div class="row">
		
		<?php include_once("left_navbar.php") ?>
        
        <div class="col-sm-9 col-sm-offset-3 col-md-10 col-md-offset-2 main">
          <h1 class="page-header">Seznam mých pobytù</h1>
		  
          <div class="table-responsive">
            <table id="table" class="table table-hover" data-sort-name="id_izby" data-search="true" data-height="500" data-pagination="true" data-page-size="25">
              <thead>
                <tr>
				  <th data-visible="false" data-field="id_pobytu">ID</th>
                  <th data-sortable="true" data-field="izba">ID pokoje</th>
                  <th data-sortable="true" data-field="typ">Typ</th>
                  <th data-sortable="true" data-field="cena" data-formatter="priceFormatter">Cena</th>
				  <th data-sortable="true" data-field="zaciatok">Od</th>
				  <th data-sortable="true" data-field="koniec">Do</th>
                </tr>
              </thead>
            </table>
          </div>
        </div>
		<div class="col-sm-3 col-sm-offset-3 col-md-4 col-md-offset-2">
		  <div class="panel panel-default">
			<div class="panel-heading">
				<h3 class="panel-title">Slu¾by v rámci pobytu</h3>
			</div>
			<div id="servlist" class="panel-body">
			</div>
		</div>
      </div>
		  
		  <div class="col-sm-3 col-md-4">
		  <div class="panel panel-default">
			<div class="panel-heading">
				<h3 class="panel-title">Popis slu¾by</h3>
			</div>
			<div class="panel-body">
				   <h4 id="servname" class="list-group-item-heading"></h4>
				   <p id="servinfo" class="list-group-item-text"></p>
				   <p id="servprice" class="list-group-item-text"></p>
			</div>
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
			"getservices.php",
			{ id: row['id_pobytu']},
			showservices
		);
	});
	}
	
	function run() {
		$.getJSON(
			"gethistory.php",
			{},
			show
		);
		
		$(window).resize(function () {
			$('#table').bootstrapTable('resetView');
		});
		
    };
	
	function priceFormatter(value) {
		return value + " Kè";
    }
	
	
  function showservices(json_data) {
	  $('#servlist').empty();
	  for (k in json_data) {
		  var txt = '<a href="#" class="list-group-item" onclick="setServInfo(\'' + json_data[k]['typ'] + '\', \'' + json_data[k]['popis'] + '\', \'' + json_data[k]['cena'] +  '\')" >' + json_data[k]['typ'] + '</a>';
		  $('#servlist').append(txt);
	  }
  }
  
  function setServInfo(name, info, price) {
	  $('#servname').text("Jméno: " + name);
	  $('#servinfo').text("Popis: " + info);
	  $('#servprice').text("Cena: " + price + " Kè");
  }
	
	window.onload=run;
	
	</script>
	
    <!-- Bootstrap core JavaScript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
	<?php include_once("scripts.php"); ?>
  </body>
</html>
