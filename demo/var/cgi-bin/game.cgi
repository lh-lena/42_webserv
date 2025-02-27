#!/bin/bash

#nohup ./var/cgi-bin/2048 > /dev/null 2>&1 &
# gnome-terminal -- /var/cgi-bin/2048 &
echo "Content-Type: text/html"
echo "Location: /index.html"
echo "Status: 302 Found"
echo ""

osascript -e 'tell application "Terminal" to do script "Documents/webserv/demo/var/www/html/projects/2048"'
