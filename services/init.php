<?php
require_once '../Connection.php';

$connection = new Connection();

$sqlDrop = "DROP TABLE IF EXISTS `events`";

$sqlCreate = <<<EOD
CREATE TABLE `events` (
`id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `micros` int(11) DEFAULT NULL,
  `sessionId` int(11) DEFAULT NULL,
  `pin` int(11) DEFAULT NULL,
  `timestamp` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
EOD;

$connection->query($sqlDrop, true);
$connection->query($sqlCreate, true);
