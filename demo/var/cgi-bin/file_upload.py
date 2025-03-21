#!/usr/bin/env python3

import cgi
import json

form = cgi.FieldStorage()

print("Content-Type: application/json")
response = {}
response = {"status": "success", "message": f"Login successful! Welcome, {form}."}

print()
print(json.dumps(response, indent=4))
print()
