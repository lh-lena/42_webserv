## Webserv

#### Collaboratively developed with [dzhoka](https://github.com/dzhoka) a fully functional HTTP web server in C++ following the HTTP/1.1 protocol

Capable of handling concurrent client requests. 
Implemented features such as request parsing, response generation, connection management, and load balancing. 
Gained experience with networking concepts like socket programming, non-blocking I/O, and multi-threading, while ensuring high performance and scalability.

## Configuration file

This configuration file defines the behavior of our `webserv` web server. It uses a structure similar to Nginx, allowing for flexible server and location configurations.

```
http {  # The main HTTP block, containing server configurations.

 server {  # Defines a virtual server.

      listen [ip:port];                       # Binds the server to a specified IP address and port. Default: 127.0.0.1:8080
      server_name [hostname];                 # Defines the server name. Example: localhost
      index [index.ext];                      # Specifies the default files to serve if a directory is requested. Example: index.html
      root [relative_path];                   # Sets the root directory for serving files.
      client_max_body_size [xM];              # Limits the maximum size of client request bodies to x megabytes. Default: 5M
      error_page <status code(s)> [path];     # Defines a custom error pages indicate code and a path to this file. Example: 404 /error_pages/404.html
      cgi_extension [.php .py];               # Specifies the file extension for CGI scripts.


  location [path] {  # Defines specific configurations for different URL paths.
        autoindex [on];        # Enables or disables the display of directory listings, use `on` or `off`. Default: off
    }


  location /upload {
        root ./var/www/uploads;        # Root directory for upload requests.
        allow_methods GET POST DELETE; # Restricts the HTTP methods allowed for a location. Default: GET
        upload_directory /var/uploads; # Defines the directory where uploads will be stored.
    }


    location \*[.php] {  # Extention search for cgi scripts.
        root ./usr/lib/cgi-bin; # Root directory for specified location only.
        allow_methods GET POST; # Allowed HTTP methods for specified location only.
    }

 }

}
```
