#!/usr/bin/php-cgi
<?php

// Enable error reporting
ini_set('display_errors', 1);
error_reporting(E_ALL);

// Redirect back to index.html after successful upload
// header ("Location: " . $_SERVER['HTTP_REFERER']);

$rawData = file_get_contents("php://input");
file_put_contents("debug.log", $rawData);
// if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
//     header('Status: 504 Method Not Allowed');
//     echo '504 Method Not Allowed.';
//     exit;
// }

// if (!isset($_SERVER['UPLOAD_PATH'])) {
//     header('Status: 400 Bad request');
//     echo "";
//     echo "400 Bad request\nUPLOAD_PATH is not set in the server environment.";
//     exit;
// }
// // Set the upload directory
// $upload_dir = $_SERVER['UPLOAD_PATH']; // use the path from server config.
// if (!is_dir($upload_dir)) {
//     if (!mkdir($upload_dir, 0755, true)) { //create the directory if it does not exists
//         header ('Status: 400 Bad request');
//         echo "";
//         echo "400 Bad request\nFailed to create directory.";
//         exit;
//     }
// }

// $files_list = $upload_dir . "/files.txt";

// // Check if a file was uploaded
// if (!isset($_FILES['file'])) {
//     header('Status: 400 Bad Request');
//     echo "\n";
//     var_dump($_SERVER['CONTENT_TYPE']);
//     echo getcwd();
//     echo "400 Bad request\nNo file uploaded.";
//     ob_end_flush(); // End output buffering
//     exit;
// }
// $file = $_FILES['file'];

// // Sanitize the file name to prevent directory traversal attacks
// $filename = basename($file['name']);
// $target_file = $upload_dir . $filename;

// // Move the uploaded file
// if (move_uploaded_file($file['tmp_name'], $file_path)) {
//     // Save filename to the list
//     file_put_contents($files_list, $file_name . "\n", FILE_APPEND);
//     echo "\n";
//     echo "File uploaded successfully: " . htmlspecialchars($filename);
// } else {
//     header ('Status: 500 Internal Server Error');
//     echo "\n";
//     echo "500 Internal Server Error\nFile upload failed.";
// }
// ob_end_flush(); // End output buffering

?>
