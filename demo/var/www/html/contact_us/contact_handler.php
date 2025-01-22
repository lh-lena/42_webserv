
#$_POST = json_decode(file_get_contents("php://input"), true);
#echo var_dump($_POST);

#!/usr/bin/php-cgi
<?php

// define variables and set to empty values
$input = $data = $response = $name = $email = $message = $timestamp = "";

// determine the request method
$request_method = $_SERVER['REQUEST_METHOD'];

// enable error reporting
ini_set('display_errors', 1);
ini_set('error_log', __DIR__ . '/php_errors.log');
error_reporting(E_ALL);

// init response
$response = ['success' => false];
header('Content-Type: application/json');

if ($request_method === 'GET')
{
    // parse data from the query
    $name = test_input($_GET['name'] ?? '');
    $email = test_input($_GET['email'] ?? '');
    $message = test_input($_GET['message'] ?? '');
    $timestamp = date('Y-m-d H:i:s');
}
else if ($request_method === 'POST') {
    // parse data from the request body (JSON)

    // read input data from POST
    $input = file_get_contents('php://input');
    $data = json_decode($input, true);
    if ($data === null) {
        header('HTTP/1.1 400 Bad Request');
        http_response_code(400);
        echo json_encode(['success' => false, 'error' => 'Invalid JSON input.']);
        exit;
    }

    // Prepare email content
    // use htmlspecialchars to prevent XSS attacks when outputting user-provided data
    $name = test_input($data['name'] ?? '');
    $email = test_input($data['email'] ?? '');
    $message = test_input($data['message'] ?? '');
    $timestamp = date('Y-m-d H:i:s');
}
else {
    // Unsupported request method
    header('HTTP/1.1 405 Method Not Allowed');
    echo json_encode(['success' => false, 'error' => 'Method not allowed.']);
    exit;
}

if (empty($name) || empty($email) || empty($message) || !filter_var($email, FILTER_VALIDATE_EMAIL)) {
    header('HTTP/1.1 200 OK');
    echo json_encode(['success' => false, 'error' => 'All fields are required.']);
    exit;
}

$email_content = "New Contact Form Submission:\n\n";
$email_content .= "Name: $name\n";
$email_content .= "Email: $email\n";
$email_content .= "Message: $message\n";
$email_content .= "Submitted on: $timestamp\n";

// save to a file
$file = __DIR__ . '/messages.log';
file_put_contents($file, $email_content . "\n\n", FILE_APPEND);

// respond with success
$response = [
    'success' => true,
    'message' => htmlspecialchars("Hello $name, thank you for your message!")
];

// return a JSON response
header('HTTP/1.1 200 OK');
echo json_encode($response);

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}

?>
