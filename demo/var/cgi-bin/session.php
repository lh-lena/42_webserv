#!/usr/bin/php-cgi

<?php

echo "Content-Type:text/plane\n\n";
session_start();
    echo "The session ID is:" . session_id();
?>
