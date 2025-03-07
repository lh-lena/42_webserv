#!/usr/bin/env python3
import os
import http.cookies
import cgi

# Parse CGI input
form = cgi.FieldStorage()
action = form.getvalue("action", "")
cookie_name = form.getvalue("name", "")
cookie_value = form.getvalue("value", "")

# Get existing cookies
cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))

print("Content-Type: text/plain")
if action == "set":
    # Set a new cookie
    cookie[cookie_name] = cookie_value
    cookie[cookie_name]["path"] = "/"  # Make it available site-wide
    print(f"Set-Cookie: {cookie.output(header='', sep='')}")
    print("\nCookie set successfully.")

elif action == "get":
    # Display all stored cookies
    if cookie:
        print("\nStored Cookies:\n")
        for key, morsel in cookie.items():
            print(f"{key}: {morsel.value}")
    else:
        print("\nNo cookies found.")

elif action == "delete":
    # Delete a cookie
    if cookie_name in cookie:
        cookie[cookie_name] = ""
        cookie[cookie_name]["Expires"] = "Thu, 01 Jan 1970 00:00:00 GMT"
        cookie[cookie_name]["Max-Age"] = 0
        print(f"Set-Cookie: {cookie.output(header='', sep='')}")
        print("\nCookie deleted successfully.")
    else:
        print("\nCookie not found.")

print()
