#!/usr/bin/php-cgi
<?php

// Enable error reporting
ini_set('display_errors', 1);
error_reporting(E_ALL);

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    echo('Status: 504 Method Not Allowed');
    echo json_encode(['success' => false, 'error' => 'No file uploaded.']);
    exit;
}

// Set the upload directory
$upload_dir = $_SERVER['UPLOAD_PATH']; // use the path from server config.
if (!is_dir($upload_dir)) {
    if (!mkdir($upload_dir, 0755, true)) { //create the directory if it does not exists
        header ('Status: 400 Bad request');
        echo json_encode(['success' => false, 'message' => 'Failed to create directory.']);
        exit;
    }
}
$files_list = "/var/www/html/uploads/files.txt";

echo('Content-Type: application/json');

// Check if a file was uploaded
if (!isset($_FILES['file'])) {
    echo('Status: 400 Bad Request');
    echo json_encode(['success' => false, 'error' => 'No file uploaded.']);
    exit;
}

$file_name = basename($_FILES['file']['name']);
$file_path = $upload_dir . $file_name;

// Move the uploaded file
if (move_uploaded_file($_FILES['file']['tmp_name'], $file_path)) {
    // Save filename to the list
    file_put_contents($files_list, $file_name . "\n", FILE_APPEND);

    // Redirect back to index.html after successful upload
    echo ("Location: /index.html");
    exit;
} else {
    echo ('Status: 500 Internal Server Error');
    echo json_encode(['success' => false, 'error' => 'File upload failed.']);
}
?>
