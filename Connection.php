<?php
require_once("./env.php");

class Connection {
    public $db;

    public function __construct()
    {
        $host = strtolower($_SERVER['HTTP_HOST']);

        switch($host) {
            case 'dev.muondetector':
                $servername = "localhost";
                $dbname = "muondetector";
                $username = "root";
                $password = "root";
                break;
            case REMOTE_HOST:
                $servername = REMOTE_SERVER;
                $dbname = REMOTE_DB;
                $username = REMOTE_USERNAME;
                $password = REMOTE_PASSWORD;
                break;
            default:
                die('Unknown host');
        }


        $conn = new mysqli($servername, $username, $password, $dbname);

        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        }

        $this->db = $conn;
    }

    public function query($sql, $verbose = false)
    {
        $result = false;
        if (isset($this->db)) {
            $result = $this->db->query($sql);
            if ($verbose) {
                echo $sql. PHP_EOL;
                if ($result === FALSE) {
                    echo "Error: " . $this->db->error;
                } else {
                    echo "SQL executed successfully";
                }
            }
        }
        return $result;
    }
}
