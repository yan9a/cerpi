<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <!--script src="d3.min.js"></script-->
    <script src="https://d3js.org/d3.v3.min.js"></script>

    <style>
        .chart div {
          font: 10px sans-serif;
          background-color: steelblue;
          text-align: right;
          padding: 3px;
          margin: 1px;
          color: white;
        }

        #dashboard {
          width: 320px;
          border: 1px solid black;
        }

        p {
          text-align: center;
        }
    </style>
</head>
<body>

<div id="dashboard">
	<p>Sensor values</p>
	<div class="chart">

	</div>
<br/>
</div>

<script>
//var data = [210, 860, 468, 681, 303, 565];

var data = [<?php

for ($x=0;$x<6;$x++) {
if($x!=0) echo ",";
$sv=shell_exec("/var/www/html/d3js/readsensor ".$x);
echo $sv;
} 

//include 'myarr.php';

?>
];


d3.select(".chart")
  .selectAll("div")
  .data(data)
    .enter()
    .append("div")
    .style("width", function(d) { return (d*300/860) + 'px' })
    .text(function(d) { return d+' mA'; });
</script>

</body>
</html>
            


