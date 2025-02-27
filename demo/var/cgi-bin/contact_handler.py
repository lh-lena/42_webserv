#!/usr/bin/python3

import json
import sys
import os

print("Content-Type: application/json\n")

try:
    request_method = os.environ.get("REQUEST_METHOD", 0)
    # Read JSON input from stdin (sent from JavaScript)
    content_length = os.environ.get("CONTENT_LENGTH", 0)
    if content_length:
        data = sys.stdin.read(int(content_length))
        form_data = json.loads(data)
    else:
        raise ValueError("No input data received")

    # Extract values
    name = form_data.get("name", "").strip()
    email = form_data.get("email", "").strip()
    message = form_data.get("message", "").strip()

    # Basic validation
    if not name or not email or not message:
        print(json.dumps({"success": False, "error": "Invalid input. All fields are required."}))
        sys.exit()

    # Save data to a file
    with open("messages.txt", "a") as file:
        file.write(f"Name: {name}\nEmail: {email}\nMessage: {message}\n\n")

    # Send success response
    print(json.dumps({"success": True, "message": f"Thank you, {name}, for your message!"}))

except Exception as e:
    print(json.dumps({"success": False, "error": str(e)}))

