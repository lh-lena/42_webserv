* cat 404.html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>404 Not Found</title>
</head>
<body>
    <h1>Not Found</h1>
    <p>The requested URL /PageList.html was not found on this server.</p>
</body>
</html>

* c3a5c2%  ls
error_pages  LICENSE	 README.md	     web_server.cpp
index.html   nginx.conf  tcp_server_pr1.cpp

* c3a5c2% GET /error_pages/404.html HTTP/1.1
<html><body><h1>403 Forbidden</h1>
Request forbidden by administrative rules.
</body></html>

* c3a5c2% HEAD /error_pages/404.html HTTP/1.1
404 File `/error_pages/404.html' does not exist
Client-Date: Wed, 16 Oct 2024 08:38:03 GMT

403 Forbidden
Cache-Control: no-cache
Connection: close
Content-Length: 93
Content-Type: text/html
Client-Date: Wed, 16 Oct 2024 08:38:03 GMT
Client-Peer: 5.22.145.16:80
Client-Response-Num: 1

* c3a5c2% POST /error_pages/404.html HTTP/1.1
Please enter content (application/x-www-form-urlencoded) to be POSTed:
<p>New info</p>
;
<html><body><h1>403 Forbidden</h1>
Request forbidden by administrative rules.
</body></html>

* GET www.google.com HPPT/1.1 
<!doctype html><html itemscope="" itemtype="http://schema.org/WebPage" lang="de"><head><meta content="text/html; charset=UTF-8" http-equiv="Content-Type"><meta content="/images/branding/googleg/1x/googleg_standard_color_128dp.png" itemprop="image"><title>Google</title><.....


* nc 127.0.0.1 5500       
GET /error_page/404.html

HTTP/1.1 404 Not Found
Vary: Origin
Access-Control-Allow-Credentials: true
Content-Security-Policy: default-src 'none'
X-Content-Type-Options: nosniff
Content-Type: text/html; charset=utf-8
Content-Length: 158
Date: Wed, 30 Oct 2024 10:59:56 GMT
Connection: close

<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<title>Error</title>
</head>
<body>
<pre>Cannot GET /error_page/404.html</pre>
</body>
</html>
