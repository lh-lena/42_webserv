#!/usr/bin/php-cgi
<?php

// Enable error reporting
ini_set('display_errors', 1);
error_reporting(E_ALL);

// Set the upload directory
$upload_dir = "/var/www/html/uploads/";
$files_list = "/var/www/html/uploads/files.txt";

// Ensure the directory exists
if (!file_exists($upload_dir)) {
    mkdir($upload_dir, 0777, true);
}

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
