#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <string.h>

using namespace std;
// https://www.youtube.com/watch?v=cNdlrbZSkyQ
// AF_INET version 4
int main()
{
    // create a socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        cerr << "Can't create a socket!";
        return -1;
    }

    // bind the socket to a IP / port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    // to run on any IP address on this machine
    inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr); // convert a number to array of integers 127.0.0.1 
    if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1)
    {
        cerr << "Can't bind to IP/port";
        return -2;
    }
    // Mark the socket for listening include
    if (listen(listening, SOMAXCONN) == -1)
    {
        cerr << "Can't listen!";
        return -3;
    }
    // accept a call
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    int clientSocket = accept(listening,
                            (sockaddr*)&client,
                            &clientSize);
    if (clientSocket == -1)
    {
        cerr << "Problem with client connecting!";
        return -4;
    }

    // close the listening socket
    close(listening);
    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client,
                            sizeof(client),
                            host,
                            NI_MAXHOST,
                            service,
                            NI_MAXSERV,
                            0);
    if (result)
    {
        cout << host << " connected on " << service << endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on " << ntohs(client.sin_port) << endl;
    }

    // while receiving display message, echo message

    char buf[4096];
    while (true)
    {
        // clear the buffer
        memset(buf, 0, 4096);
        int bytesRecv = recv(clientSocket, buf, 4096, 0);
        // wait for a message
        if (bytesRecv == -1)
        {
            cerr << "There was a connection issue" << endl;
            break;
        }
        if (bytesRecv == 0)
        {
            cout << "The client disconnected" << endl;
            break;
        }

        // display message
        cout << "Received: " << string(buf, 0, bytesRecv) << endl;

        // resend message
        send(clientSocket, buf, bytesRecv + 1, 0);
    }
    // close the socket

    close(clientSocket);
}

