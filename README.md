## Webserv - HTTP Server in C++98
> "This is when you finally understand why a URL starts with HTTP"

A fully functional HTTP/1.1 server implementation written in C++98, capable of serving static websites, handling CGI scripts, processing file uploads, and supporting multiple virtual hosts.
#### Collaboratively developed with [dzhoka](https://github.com/dzhoka)

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

## 📖 Usage
### Basic Usage
```bash
# Run with default configuration
./webserv

# Run with custom configuration file
./webserv config/serv.conf

# Run with specific configuration
./webserv /path/to/your/config.conf
```
### Quick Start Example
```bash
# 1. Compile the server
make

# 2. Run with example configuration
./webserv config/serv.conf

# 3. Open your browser and visit:
# http://localhost:8080
```

## ⚙️ Configuration
### The server uses Nginx-inspired configuration syntax. Here's a comprehensive configuration example:

#### Basic Configuration

```nginx

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

#### Understanding `webserv` context:

| Directive             | Context           | Description                | Example                               |
| :-------------------- | :---------------- | :------------------------- | :------------------------------------ |
| `worker_connections`  | `global`          | Max simultaneous connections | `worker_connections 1024;`            |
| `listen`              | `server`          | Bind address and port      | `listen 8080;`                        |
| `server_name`         | `server`          | Virtual host names         | `server_name example.com;`            |
| `root`                | `server`, `location` | Document root              | `root ./www;`                         |
| `index`               | `server`, `location` | Default files              | `index index.html;`                   |
| `error_page`          | `server`, `location` | Custom error pages         | `error_page 404 /404.html;`           |
| `client_max_body_size` | `server`, `location` | Request body limit         | `client_max_body_size 10M;`           |
| `allow_methods`       | `location`        | Allowed HTTP methods       | `allow_methods GET POST;`             |
| `autoindex`           | `location`        | Directory listing          | `autoindex on;`                       |
| `alias`               | `location`        | Path aliasing              | `alias ./assets/;`                    |
| `return`              | `location`        | HTTP redirections          | `return 301 /new-path;`               |
| `upload_dir`          | `location`        | Upload directory           | `upload_dir ./uploads;`               |
| `cgi_extension`       | `server`          | CGI file extensions        | `cgi_extension .php .py;`             |

## 🔍 Compliance & Standards
### HTTP/1.1 Compliance
✅ Request/Response format </br>
✅ Status codes (1xx, 2xx, 3xx, 4xx, 5xx) </br>
✅ Headers handling </br>
✅ Persistent connections </br>
✅ Chunked transfer encoding </br>
✅ RFC 7230: HTTP/1.1 Message Syntax and Routing </br>
✅ RFC 7231: HTTP/1.1 Semantics and Content </br>
✅ RFC 7232: HTTP/1.1 Conditional Requests </br>

### CGI Compliance
✅ RFC 3875: Common Gateway Interface (CGI) Version 1.1 </br>
✅ Environment variable handling </br>
✅ Input/Output redirection </br>
✅ Error handling </br>


### Browser Compatibility
✅ Chrome </br>
✅ Firefox </br>
✅ Safari </br>
