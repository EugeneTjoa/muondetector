<?php
require_once "../Connection.php";

class Event {
    public $id;
    public $micros;
    public $pin;
    public $timestamp;
}

function getData() {
    $connection = new Connection();

    $sql = <<<EOD
    SELECT * FROM events
    ORDER BY id DESC
    LIMIT 100
    EOD;
    return $connection->query($sql);
}

function generateHtmlTable($result) {
    $table = "";
    while($row = $result->fetch_object("Event")) {
        $tr[] = <<<ROW
        <tr>
            <td>$row->id</td>
            <td>$row->micros</td>
            <td>$row->pin</td>
            <td>$row->timestamp</td>
        </tr>
        ROW;

    }
    $rows = implode("\n", $tr);

    $table = <<<TABLE
    <table class="table">
        <thead>
            <tr>
                <th>id</th>
                <th>micros</th>
                <th>pin</th>
                <th>timestamp</th>
            </tr>
        </thead>
        <tbody>
            $rows
        </tbody>
    </table> 
    TABLE;
    return $table;
}

function generateHtmlPage($table) {
    echo <<<PAGE
    <!doctype html>
    <html>
        <head>
            <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.0.0/dist/css/bootstrap.min.css" integrity="sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm" crossorigin="anonymous">
        </head>
        <body>
            <div class="container">
                <div class="row">
                    <div class="col-12">
                        <h1>Results</h1>
                        $table
                    </div>
                </div>
            </div>
        </body>
    </html> 
    PAGE;
}

function main() {
    $result = getData();
    $table = generateHtmlTable($result);
    generateHtmlPage($table);
}

main();