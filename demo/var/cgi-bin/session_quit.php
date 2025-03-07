#!/usr/bin/php-cgi

<?php

session_destroy();
echo "<!DOCTYPE html>
<html lang='en'>
<head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width, initial-scale=1.0'>
<title>Message Received</title>
<style>
h1, p {  width: 50%; margin: 20px auto;}
</style>
</head>
<body>
<div class='home_bnt'>
<a href='/index.html'>H O M E</a><br />
<h1>The session is successefully destroied</h1>
</body>
</html>";

?>