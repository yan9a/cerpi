<?php
$json = file_get_contents('php://input');
// echo $json."\n";
$data = json_decode($json);
$data->cherry = $data->apple.$data->banana;
header('Content-Type: application/json');
echo json_encode($data);
// curl -X POST "http://localhost/cpprestsdk/cpprest/postClient/j2.php" -H "accept: */*" -H "Content-Type: application/json" -d "{\"apple\":\"A\",\"banana\":\"B\"}"
?>
