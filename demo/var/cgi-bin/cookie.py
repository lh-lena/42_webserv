#!/usr/bin/python3

import os, time

# curl -v --cookie "Second=Fri, 16-Feb-2025 15:16:00; Count=3; InvalidCookie" http://localhost:8080/cookie.py

def get_cookie(_match):
  # Returns the value from the matching cookie or '' if not defined.
  if 'HTTP_COOKIE' in os.environ:
    cookies = os.environ['HTTP_COOKIE']
    cookies = cookies.split('; ')

    for cookie in cookies:
      if '=' in cookie:  # Ensure the cookie has a valid name=value structure.
        (_name, _value) = cookie.split('=', 1)  # Split only on the first '='.
        if _match.lower() == _name.lower():
          return _value
  return('')

def val(_string):
  # Returns the integer value of a string or zero if not numeric.
  try:
    return int(_string)
  except:
    return 0

# cookies are deleted after the date specified in the Expires or after the period specified in the Max-Age attribute:
# Expires=Thu, 31 Oct 2021 07:28:00 GMT;
# Max-Age=2592000

print ('Content-Type: text/html')
print ()
if "HTTP_COOKIE" in os.environ:
  print (os.environ['HTTP_COOKIE'])
if not get_cookie('first'): # Don't redefine value if aleady defined.
  print ('Set-Cookie: First=' + time.asctime(time.localtime()))
# if not get_cookie('Expires'):
#   seconds_to_add = 60 * 1
#   current_time_tuple = time.localtime()
#   future_time_seconds = time.mktime(current_time_tuple) + seconds_to_add
#   future_time_tuple = time.localtime(future_time_seconds)
#   print ('Set-Cookie: Expires=' + time.asctime(future_time_tuple)) # Expires cookies
print ('Set-Cookie: Last=' + time.asctime(time.localtime()))
print ('Set-Cookie: Count=' + str(val(get_cookie('Count')) + 1)) # Increment counter
print ()# End of header

print("<!DOCTYPE html>")
print("<html lang=\"en\">")
print("<head>")
print("    <meta charset=\"UTF-8\">")
print("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">")
print("</head>")
print('<body>')
print('<div class="home_bnt">')
print('<a href="/index.html">H O M E</a>')
print('</div>')
print ('<table border="0">')
if get_cookie('first'): # Only dispay value if cookie is defined.
  print ('<tr><td>Time First Visit</td><td>:</td><td>' + get_cookie('first') + '</tr>')
if get_cookie('last'): # Only dispay value if cookie is defined.
  print ('<tr><td>Time Last Visit</td><td>:</td><td>' + get_cookie('last') + '</tr>')
# if get_cookie('Expires'): # Only dispay value if cookie is defined.
#   print ('<tr><td>Cookies will be expired </td><td>:</td><td>' + get_cookie('Expires') + '</tr>')
print ('<tr><td>Counter</td><td>:</td><td>' + str(val(get_cookie('count'))) + '</tr>')
print ('</table>')
print('</body>')
print('</html>')
