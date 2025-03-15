#!/usr/bin/php-cgi
<?php

// Enable error reporting
ini_set('display_errors', 1);
error_reporting(E_ALL);

$rawData = file_get_contents("php://input");
file_put_contents("logs/debug.log", "##\n");
file_put_contents("logs/debug.log", $_SERVER["CONTENT_LENGTH"] . "\n", FILE_APPEND);
file_put_contents("logs/debug.log", file_get_contents("php://input") . "\n", FILE_APPEND);
file_put_contents("logs/debug.log", var_export($_FILES, true) . "\n", FILE_APPEND);
file_put_contents("logs/debug.log", $_FILES["file"]["error"] . "\n", FILE_APPEND);
if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    header('Status: 504 Method Not Allowed');
    echo"\n";
    echo "Method not allowed!";
    exit;
}

if (!isset($_SERVER['UPLOAD_PATH'])) {
    header('Status: 400 Bad request');
    echo"\n";
    echo "400 Bad request\nUPLOAD_PATH is not set in the server environment.";
    exit;
}
// Set the upload directory
$upload_dir = $_SERVER['UPLOAD_PATH']; // the path from server config
    
    // the full relative file path
if (strrpos($upload_dir, '/') !== strlen($upload_dir) - 1)
    $upload_dir .= '/';
$upload_dir = dirname(__DIR__, 2) . '/' . $upload_dir;
// if (!is_dir($upload_dir)) {
    //     if (!mkdir($upload_dir, 0755, true)) { //create the directory if it does not exists
    //         header ('Status: 400 Bad request');
    //         echo "";
    //         echo "400 Bad request\nFailed to create directory.";
    //         exit;
    //     }
    // }

$files_list = "logs/files.txt";

// Check if a file was uploaded
if (!isset($_FILES['file'])) {
    header('Status: 400 Bad Request');
    echo "\n";
    // var_dump($_SERVER['CONTENT_TYPE']);
    echo "Bad request\nNo file uploaded.";
    ob_end_flush(); // End output buffering
    exit;
}
$file = $_FILES['file'];
    
// Sanitize the file name to prevent directory traversal attacks
$filename = basename($file['name']);
$filepath = $upload_dir . $filename;
// $target_file = $upload_dir . $filename;

// Move the uploaded file
if (move_uploaded_file($file['tmp_name'], $filepath)) {
    // Save filename to the list
    header('Status: 201 Created');
    file_put_contents($files_list, $filename . "\n", FILE_APPEND);
    echo "\n";
    echo "File uploaded successfully: " . htmlspecialchars($filename);
} else {
    header ('Status: 500 Internal Server Error');
    echo "\n";
    echo "500 Internal Server Error\nFile upload failed.";
}
ob_end_flush(); // End output buffering

?>
