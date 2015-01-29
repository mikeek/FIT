<?php include_once("session_check.php"); 
      include_once("setpayment.php");
?>


<html lang="en">
  <?php	include_once("head.php"); ?>

  <body class="visitors">

    <?php include_once("top_navbar.php"); ?>

    <div class="container-fluid">
      <div class="row">
    
    <?php include_once("left_navbar.php") ?>
        
        <div class="col-sm-9 col-sm-offset-3 col-md-10 col-md-offset-2 main">
          <h1 class="page-header">Seznam pobytù</h1>

    <?php include_once("../status.php"); ?>
      
          <div class="table-responsive">
            <table id="table" class="table table-hover" data-sort-name="id_pobytu" data-search="true" data-height="500" data-pagination="true" data-page-size="25">
              <thead>
                <tr>
                  <th data-sortable="true" data-field="id_pobytu">ID</th>
                  <th data-sortable="true" data-field="zaciatok">Od</th>
                  <th data-sortable="true" data-field="koniec">Do</th>
                  <th data-sortable="true" data-field="platba">Platba</th>
                  <th data-sortable="true" data-field="osoba">Klient</th>
                  <th data-sortable="true" data-field="izba">Pokoj</th>
                  <th data-field="operate" data-formatter="operateFormatter" data-events="operateEvents">Zaplatit</th>
                </tr>
              </thead>
            </table>
          </div>

          <!-- Edited reservation -->
          <form  name="res_form" id="res_form" class="form-horizontal" role="form" method="post">
      <a name="editation"></a>
        <fieldset>
        <legend> Zaplatenie pobytu </legend>
            <!-- Reservation ID -->
            <div class="form-group">
                <label class="col-md-4 control-label" for="id_pobytu">ID Pobytu</label>  
                <div class="col-md-4">
                    <input id="id_pobytu" name="id_pobytu" type="text" class="form-control input-md" readonly>
                </div>
            </div>

            <!-- Client ID -->
            <div class="form-group">
                <label class="col-md-4 control-label" for="osoba">Uhradzuje</label>  
                <div class="col-md-4">
                    <input id="osoba" name="osoba" type="text" class="form-control input-md" readonly>
                </div>
            </div>
            
            <!-- Room ID -->
            <div class="form-group">
                <label class="col-md-4 control-label" for="izba">Cislo izby</label>  
                <div class="col-md-4">
                    <input id="izba" name="izba" type="text" class="form-control input-md" readonly>
                </div>
            </div>

            <!-- Suma -->
            <div class="form-group">
                <label class="col-md-4 control-label" for="suma">Suma</label>  
                <div class="col-md-4">
                    <input id="suma" name="suma" type="text" class="form-control input-md" readonly>
                </div>
            </div>

             <!-- Typ platby ID -->
            <div class="form-group">
                <label class="col-md-4 control-label" for="type">Typ platby</label>  
                <div class="col-md-4">
                   <select id="type" name="type" class="form-control" required>
                      <?php                      
                          echo ' <option value="' . 0 .'">Cash</option>'; 
                          echo ' <option value="' . 1 .'">Prevod</option>';             
                      ?>
                  </select>
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
      "getvisitors.php",
      {},
      show
    );
    
    $(window).resize(function () {
      $('#table').bootstrapTable('resetView');
    });
    
    };

  function fillform(json_data) {
    for(key in json_data) {
		if(json_data.hasOwnProperty(key))
		$('input[name='+key+']').val(json_data[key]);

	}
	$('#res_form').show(300);
  };
  
  window.onload=run;

  window.operateEvents = {
        'click .edit': function (e, value, row, index) {
			// Fill data from table
            fillform(row);
			
			// Fill sum from DB
			$.getJSON("getsumbyid", {id: row['id_pobytu']}, fillform);
			
			window.location.hash="editation";
        }};
  
  </script>

    <!-- Bootstrap core JavaScript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
    <?php	include_once("scripts.php");
     ?>
  </body>
</html>
