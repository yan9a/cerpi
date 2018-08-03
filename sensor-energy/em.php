<html>
<head>
<meta http-equiv="refresh" content="1">
<title>Energy Monitor</title>
<script type="text/javascript" src="https://www.gstatic.com/charts/loader.js">
</script>
<script type="text/javascript">
    google.charts.load('current', {'packages':['corechart']});
    google.charts.setOnLoadCallback(drawChart);
    function drawChart() {
		var data = google.visualization.arrayToDataTable([['T(s)',  'P(W)'],
			<?php include 'eseconds.php'; ?>]);
		var options={title:'Energy consumption',vAxis:{title: 'Watt-second'}};
		var chart = new google.visualization.SteppedAreaChart(document.getElementById('chart_div'));
		chart.draw(data, options);
    }
</script>
</head>
<body>
<div style="margin-left: auto; margin-right:auto; position: relative; 
	width: 800px; padding: 50px; box-shadow: 0 2px 6px rgba(100,100,100,0.3);">
<h2>Energy Monitor</h2>
<div id="chart_div" style="width: 700px; height: 400px;"></div>
<a style="float: right; font-size: 20px; color: #444444;" 
href="http://cool-emerald.blogspot.com/">cool-emerald.blogspot.com</a>
</div>
</body>
</html>