# include "Server.hpp"
# include "Client.hpp"

/* sous UNIX les sockets sont de simples descripteurs de fichiers */

/* non-blocking calls don't block the thread until they finish
their work; instead, this type of system call returns immediately
after completing its job without having any effect on what's happening
in other threads */

void setUserInfo(Client & obj){
    char buffer[1024] = {0};

  // send();
  // CMD = /NICK -> PARSING
    write(obj.getSocket(), "Enter your nickname: \n", 23);
    recv(obj.getSocket(), buffer, sizeof(buffer), 0);
    write(obj.getSocket(), "Enter your username: \n", 23);
    recv(obj.getSocket(), buffer, sizeof(buffer), 0);
}


int main(){
    
    // const char *password;
    Server irc;
    int port = 4242; 

    /* AF_INET = IPv4 protocol, SOCK_STREAN = TCP type socket*/

    int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0){
        std::cout << "Error: invalid socket" << std::endl;
        exit(-1); // < remplacer par l'exception ?>
    }
   
    /* defining server address */
    sockaddr_in serverAddress; // struct is used to represent an IPv4 address and port number combination

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port); // convert the int from machine byte to network byte
    serverAddress.sin_addr.s_addr = INADDR_ANY; // listen to all ips

/* when you first create the socket descriptor with socket(),
   the kernel sets it to blocking. If you don't want a socket
   to be blocking, you habe to make a call to fcntl()*/
   
    // fcntl () -> performs operations on the open fd
    // F_SETFL -> set the file status flags to the value specified by arg
    // O_NONBLOCK -> the bit that enables nonblocking mode for the file.

    /* binding the server socket */
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
        std::cout << "Error: socket connection failed" << std::endl;
        exit(-1); // < remplacer par l'exception ?>
    }

    /* listening for connections */
    if (listen(serverSocket, SOMAXCONN) < 0){
        std::cout << "Error: listen() falied" << std::endl;
        exit(-1);
    }

    // poll() -> wait for some event on a file descriptor
    // with a non-blocking file descriptor you can wait
    // for ANY of a set of fds to become useable -> https://stackoverflow.com/questions/3360797/non-blocking-socket-with-poll

    struct pollfd newFds;
  
    newFds.fd = serverSocket;
    newFds.events = POLLRDNORM; // "normal data may be read without blocking"
    newFds.revents = POLLOUT; // "normal data may be written without blocking"
    irc.pushPollfd(newFds);

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

    // map/vector container for clients ?

    Client client(clientSocket, inet_ntoa(from.sin_addr), ntohs(from.sin_port));
    
    //setUserInfo(client);

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