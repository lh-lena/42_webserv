1. create a socket
2. bind the socket to a IP / port
3. Mark the socket for listening include
4. accept a call
5. close the listening socket
6. while receiving display message, echo message
7. close the socket


**socket** 
- create an endpoint for communication

int socket(int domain, int type, int protocol);

- https://man7.org/linux/man-pages/man2/socket.2.html

- https://www.ibm.com/docs/en/i/7.4?topic=programming-socket-application-design-recommendations


**getaddrinfo,freeaddrinfo,gai_strerror**
- network address and service translation
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
- https://man7.org/linux/man-pages/man3/getaddrinfo.3.html


### How to Use Telnet to Test HTTP and HTTPS Service
- https://r00t4bl3.com/post/how-to-use-telnet-to-test-http-and-https-service


get started:

 - https://www.geeksforgeeks.org/socket-programming-cc/

