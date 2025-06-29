## Webserv - HTTP Server in C++98

#### Collaboratively developed with [dzhoka](https://github.com/dzhoka) a fully functional HTTP web server in C++ following the HTTP/1.1 protocol

## 🌟 Overview
Webserv is a high-performance HTTP server built from scratch in C++98. This project provides deep insights into web server architecture, HTTP protocol implementation, and network programming. The server supports modern web features while maintaining compatibility with standard web browsers.

### Key Highlights

- **HTTP/1.1 Compliant**: Full implementation of HTTP/1.1 protocol </br>
- **Non-blocking I/O**: Efficient handling of multiple simultaneous connections </br>
- **CGI Support**: Execute dynamic scripts (PHP, Python, etc.) </br>
- **Virtual Hosts**: Multiple websites on a single server </br>
- **File Upload**: Handle POST requests with file uploads </br>
- **Browser Compatible**: Works with modern web browsers </br>

## ✨ Features
### Core Functionality

✅ HTTP Methods: GET, POST, DELETE </br>
✅ Static File Serving: HTML, CSS, JS, images, and more </br>
✅ Directory Listing: Configurable auto-indexing </br>
✅ Error Pages: Custom error pages with proper status codes </br>
✅ Virtual Hosts: Multiple server configurations </br>
✅ Port Binding: Listen on multiple ports simultaneously </br>

### Advanced Features

✅ CGI Integration: Support for PHP, Python, and other CGI scripts </br>
✅ File Uploads: Handle multipart/form-data uploads </br>
✅ Redirections: HTTP redirects (301, 302, etc.) </br>
✅ Request Body Limiting: Configurable client body size limits </br>
✅ Non-blocking I/O: Using epoll() </br>
✅ Chunked Transfer: Handle chunked request encoding </br>

### Bonus Features

🎯 Session Management: Cookie-based sessions </br>
🎯 Multiple CGI: Support for multiple CGI interpreters </br>

## 🚀 Installation
### Clone the Repository
```bash
git clone https://github.com/lh-lena/42_webserv webserv
cd webserv
```
### Compile the Project
```bash
make
```
### Available Make Targets
```bash
make all      # Compile the project
make clean    # Remove object files
make fclean   # Remove object files and executable
make re       # Recompile everything
```

## Configuration file

This configuration file defines the behavior of our `webserv` web server. It uses a structure similar to Nginx, allowing for flexible server and location configurations.

```

http {  # The main HTTP block, containing server configurations.

 server {  # Defines a virtual server.

      listen [ip:port];                       # Binds the server to a specified IP address and port. Default: 127.0.0.1:8080
      listen [port];

      server_name [hostname];                 # Defines the server name. Example: localhost
      index [index.ext];                      # Specifies the default files to serve if a directory is requested. Example: index.html
      root [relative_path];                   # Sets the root directory for serving files.
      client_max_body_size [xM];              # Limits the maximum size of client request bodies to x megabytes. Default: 5M
      error_page <status code(s)> [path];     # Defines a custom error pages indicate code and a path to this file. Example: 404 /error_pages/404.html
      cgi_extension [.php .py];               # Specifies the file extension for CGI scripts.


      location [path] {  # Defines specific configurations for different URL paths.
        autoindex [on];                  # Enables or disables the display of directory listings, use `on` or `off`. Default: off
        alias [relative_path]            # Replace specified location path name by alias path
        return <status code> [path/url]; # Returns a specific HTTP status code and URL/Path, effectively redirecting the client.
      }


      location /upload {
        root ./var/www/uploads;        # Root directory for upload requests.
        allow_methods GET POST DELETE; # Restricts the HTTP methods allowed for a location. Default: GET
        upload_dir /var/uploads;       # Defines the directory where uploads will be stored.
      }


      location \*[.php] {  # Extention search for cgi scripts.
        root ./usr/lib/cgi-bin; # Root directory for specified location only.
        allow_methods GET POST; # Allowed HTTP methods for specified location only.
      }

 }

}
```

Understanding `webserv` context:

- **Global Context**: `http`,`worker_connections`
- **HTTP Context**: Configuration related to handling HTTP requests :  `server`
- **Server Context**: Configuration for a specific virtual server: `server_name`, `listen`, `root`,  `error_page` , `client_max_body_size` , `index`, `cgi_extension`, `upload_dir`, `allow_methods`
- **Location Context**: Configuration for handling specific URL paths or locations within a server: `root`, `alias`, `error_page`, `allowed_methods`, `autoindex`, `return`, `client_max_body_size`, `index`, `autoindex`, `allowed_methods`, `upload_dir`
