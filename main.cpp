# include "Server.hpp"
# include "Client.hpp"

/* sous UNIX les sockets sont de simples descripteurs de fichiers */

int main(){
    
//    const char *password;
    int port = 8080; 
    /* AF_INET = IPv4 protocol, SOCK_STREAN = TCP type socket*/
    int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0){
        std::cout << "Error: invalid socket" << std::endl;
        exit(-1);
    }
   
    /* defining server address */
    sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port); // convert the unsigned int from machine byte to network byte
    serverAddress.sin_addr.s_addr = INADDR_ANY; // l'adresse locale

    /* binding the server socket */
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
        std::cout << "Error: socket connection failed" << std::endl;
        exit(-1);
    }

    /* listening for connections */
    listen(serverSocket, SOMAXCONN);

    std::cout << "Server starting on port " << port << std::endl;

    /* accepting a client connection */

    sockaddr clientAddress = {} ;
    socklen_t len = sizeof(clientAddress);  
    int clientSocket = accept(serverSocket, &clientAddress, &len);
    if (clientSocket < 0)
        std::cout << "Error: invalid socket" << std::endl;
    sockaddr_in from;
    socklen_t addrlen = sizeof(from);
    if (getsockname(clientSocket, (struct sockaddr*)&clientAddress, &addrlen) < 0){
        std::cout << "Impossible to have an IP of a new client" << std::endl;
        close(clientSocket);
    }
    Client client(clientSocket, inet_ntoa(from.sin_addr), ntohs(from.sin_port));
    
    /* receiving data from the client with cmd recv() */
    while (true){
        char buffer[1024] = {0};
        if (recv(clientSocket, buffer, sizeof(buffer), 0) == 0){
            std::cout << "Connexion has been closed" << std::endl;
            exit(0);
        }
        else
            std::cout << "Message from client: " << buffer << std::endl;
    }
    
    /* closing the server socket */
    close(clientSocket);
    close(serverSocket);
    return 0;
}