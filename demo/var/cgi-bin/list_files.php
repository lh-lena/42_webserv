#!/usr/bin/php-cgi
<?php

header('Content-Type: application/json');

$files_list = "/var/www/html/uploads/files.txt";

if (file_exists($files_list)) {
    $files = file($files_list, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
    echo json_encode($files);
} else {
    echo json_encode([]);
}

?>
