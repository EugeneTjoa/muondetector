<?php

require_once "../Connection.php";

// Takes raw data from the request
$json = file_get_contents('php://input');

// Converts it into a PHP object
$rows = json_decode($json);

print_r($rows);

$connection = new Connection();

$values = array();
foreach($rows as $row) {
    $micros = $row->micros;
    $sessionId = $row->sessionId;
    $pin = $row->pin;
    $value = "($micros, $sessionId, $pin)";
    
    array_push($values, $value);
}

$valuesText = implode(", ", $values);
$sql = "INSERT INTO events (micros, sessionId, pin) VALUES $valuesText";

$connection->query($sql, true);
