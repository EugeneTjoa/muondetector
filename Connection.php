<?php
class Connection {
    public $db;

    public function __construct()
    {
        $host = strtolower($_SERVER['HTTP_HOST']);
echo $host;
        switch($host) {
            case 'dev.muondetector':
                $servername = "localhost";
                $dbname = "muondetector";
                $username = "root";
                $password = "root";
                break;
            case 'previewsite.eu':
                $servername = "localhost";
                $dbname = "tjoadesign_wirechamber";
                $username = "tjoadesign_wirechamber";
                $password = "YfL3gFAg";
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
                if ($result === TRUE) {
                    echo "SQL executed successfully";
                } else {
                    echo "Error: " . $this->db->error;
                }
            }
        }
        return $result;
    }
}
