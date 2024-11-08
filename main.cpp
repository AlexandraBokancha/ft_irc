#include "Server.hpp"

/* sous UNIX les sockets sont de simples descripteurs de fichiers */

int main(){
    
//    const char *password;
    int port = 8080; 

    /* AF_INET = IPv4 protocol, SOCK_STREAN = TCP type socket*/
    int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0)
        std::cout << "Socket error" << std::endl;
   
    /* defining server address */
    sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port); // convert the unsigned int from machine byte to network byte
    serverAddress.sin_addr.s_addr = INADDR_ANY; // listen to all the avaliables IPs

    /* binding the server socket */
    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    /* listening for connections */
    listen(serverSocket, SOMAXCONN);

    std::cout << "Server starting on port " << port << std::endl;
    /* accepting a client connection */
    int clientSocket = accept(serverSocket, NULL, NULL);

    /* receiving data from the client */
    char buffer[1024] = {0};
    recv(clientSocket, buffer, sizeof(buffer), 0);
    std::cout << "Message from client: " << buffer << std::endl;

    /* closing the server socket */
    close(serverSocket);
    return 0;
}