#!/bin/bash

echo "Working dir: $(pwd)"
echo "Content-Type: text/html"
echo "Location: /index.html"
echo "Status: 302 Found"
echo ""

#mac
# osascript -e 'tell application "Terminal" to do script "Documents/webserv/demo/var/www/html/projects/2048_mac"'

#linux
# gnome-terminal -- bash -c "./var/www/html/projects/2048; exec bash"

#nohup ./var/cgi-bin/2048 > /dev/null 2>&1 &

