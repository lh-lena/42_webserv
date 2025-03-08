#!/usr/bin/python3

import os
import time
import http.cookies

# curl -v --cookie "Second=Fri, 16-Feb-2025 15:16:00; Count=3; InvalidCookie" http://localhost:8080/cookie.py

# cookies are deleted after the date specified in the Expires or after the period specified in the Max-Age attribute:
# Expires=Thu, 31 Oct 2021 07:28:00 GMT;
# Max-Age=60 (in sec)

# Create cookies
cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))

cookie_expiration = 60
# First visit
if "First" not in cookie:
    cookie["First"] = time.asctime(time.localtime())
    cookie["First"]["Path"] = os.environ["REQUEST_URI"]

cookie["Last"] = time.asctime(time.localtime())
cookie["Last"]["Path"] = os.environ["REQUEST_URI"]
cookie["Last"]["Max-Age"] = str(cookie_expiration)
cookie["First"]["Max-Age"] = str(cookie_expiration)

count = 0
if "Count" in cookie:
    count = int(cookie.get("Count", "0").value) + 1
else:
    cookie["Count"] = -1

cookie["Count"] = count
cookie["Count"]["Max-Age"] = str(cookie_expiration) 
cookie["Count"]["Path"] = os.environ["REQUEST_URI"]

# Send headers
print("Content-Type: text/html")
print(cookie.output())  # Properly formatted cookies
print()  # End of headers

# HTML Output
print("<!DOCTYPE html>")
print("<html lang=\"en\">")
print("<head>")
print("    <meta charset=\"UTF-8\">")
print("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">")
print("    <title>Cookie Information</title>")
print("    <style>")
print("        body { font-family: sans-serif; }")
print("        table {  width: 50%; margin: 20px auto; }")
print("        th, td {  padding: 8px; text-align: left; }")
print("        th { background-color: #f2f2f2; }")
print("        .home_bnt { margin-top: 20px; }")
print("    </style>")
print("</head>")
print("<body>")
print("<div class=\"home_bnt\">")
print("<a href=\"/index.html\">H O M E</a><br />")
print("<br><br><br>")
print("</div>")
print("<table border=\"0\">")
if "First" in cookie:
    print(f"<tr><td>Time First Visit : {cookie['First'].value} </td></tr>")
if "Last" in cookie:
    print(f"<tr><td>Time Last Visit : \"{cookie['Last'].value}\" </td></tr>")
    print(f"<tr><td>Cookies will be expired in : {cookie_expiration} sec</td></tr>")
if "Count" in cookie:
    print(f"<tr><td>Counter : {cookie['Count'].value}</tr>")
print("</table>")
print("</body>")
print("</html>")

#my vertion
# print ('Content-Type: text/html')
# if not get_cookie('first'): # Don't redefine value if aleady defined.
#   print ('Set-Cookie: First=' + time.asctime(time.localtime()) + '; Path=' + os.environ['REQUEST_URI'])
#   seconds_to_add = 60 * 1
#   current_time_tuple = time.localtime()
#   future_time_seconds = time.mktime(current_time_tuple) + seconds_to_add
#   future_time_tuple = time.localtime(future_time_seconds)
#   print ('Set-Cookie: Expires=' + time.asctime(future_time_tuple) + '; Path=' + os.environ['REQUEST_URI']) # Expires cookies
# print ('Set-Cookie: Last=' + time.asctime(time.localtime()) + '; Path=' + os.environ['REQUEST_URI'])
# print ('Set-Cookie: Count=' + str(val(get_cookie('Count')) + 1) + '; Path=' + os.environ['REQUEST_URI'] + '; Max-Age=60') # Increment counter
# print ()# End of header

# print("<!DOCTYPE html>")
# print("<html lang=\"en\">")
# print("<head>")
# print("    <meta charset=\"UTF-8\">")
# print("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">")
# print("</head>")
# print('<body>')
# print('<div class="home_bnt">')
# print('<a href="/index.html">H O M E</a></br>')
# print('</div>')
# print ('<table border="0">')
# # if get_cookie('first'): # Only dispay value if cookie is defined.
# #   print ('<tr><td>Time First Visit</td><td>:</td><td>' + get_cookie('first') + '</tr>')
# # if get_cookie('last'): # Only dispay value if cookie is defined.
# #   print ('<tr><td>Time Last Visit</td><td>:</td><td>' + get_cookie('last') + '</tr>')
# # if get_cookie('Expires'): # Only dispay value if cookie is defined.
# #   print ('<tr><td>Cookies will be expired </td><td>:</td><td>' + get_cookie('Expires') + '</tr>')
# # print ('<tr><td>Counter</td><td>:</td><td>' + str(val(get_cookie('count'))) + '</tr>')
# if get_cookie('first'): # Only dispay value if cookie is defined.
#   print (f'<tr><td>Time First Visit</td><td>:</td><td>' + get_cookie('first') + '</tr>')
# # if get_cookie('Last'): # Only dispay value if cookie is defined.
#   print (f'<tr><td>Time Last Visit</td><td>:</td><td>' + {cookie['Last'].value} + '</tr>')
# if get_cookie('first'): # Only dispay value if cookie is defined.
#   print (f'<tr><td>Cookies will be expired </td><td>:</td><td>' + val(get_cookie('expires')) + '</tr>')
# print (f'<tr><td>Counter</td><td>:</td><td>' + {cookie['Count'].value} + '</tr>')
# print ('</table>')
# print('</body>')
# print('</html>')
