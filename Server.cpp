#include "Server.hpp"

Server::Server(int socket, int port) : _listeningSocket(socket), _port(port){
	// parser port + password ?
    if (_listeningSocket < 0){
        std::cout << "Error: invalid socket" << std::endl;
        exit(-1);
    }
}
// Copy constructor
Server::Server(const Server &other)
{
    std::cout << "Copy constructor called" << std::endl;
    (void) other;
    return ;
}

// Assignment operator overload
Server &Server::operator=(const Server &other)
{
    std::cout << "Assignment operator called" << std::endl;
    (void) other;
    return (*this);
}

// Destructor
Server::~Server(void)
{
  //  std::cout << "Destructor called" << std::endl;
    return ;
}

std::vector<struct pollfd> Server::getFdsVec(){
    return this->_fds;
}

int Server::getFdsSize(){
    return this->_fds.size();
}

int Server::getSocket(){
    return this->_listeningSocket;
}

int Server::sendAll(int socket, char *buf, int *len){
    int total = 0; // bytes sent
    int left = *len; // bytes left to sent
    int b;

    while (total < *len){
        b = send(socket, buf + total, left, 0);
        if (b == -1)
            break;
        total += b;
        left -= b;
    }
    *len = total;
    return b == -1 ? -1 : 0; // return -1 on error, 0 on success
}

void Server::clientRoutine(int i){

        /* if not the listener -> it's a regular client.
		receiving data from the client with cmd recv() */
        
        char buffer[1024] = {0};
        int nbytes = recv(_fds[i].fd, buffer, sizeof(buffer), 0);
        if (nbytes <= 0){
            if (nbytes == 0){
                std::cout << "Connexion has been closed" << std::endl;
            }
            else{
                std::cout << "Error: recv() failed" << std::endl;
            }
            close(_fds[i].fd);
            _fds[i].fd = -1;  // delete from _fds[i] ? 
        }
        else{
            // send to everyone
            std::cout << "Message from client: " << buffer << std::endl;
            for (size_t j = 0; j < _fds.size(); ++j){
                int dest_fd = _fds[j].fd;
                // send()s i ce n'est pas un serveur, et si ce n'est pas lui-meme
                if (dest_fd != _listeningSocket && dest_fd != _fds[i].fd){
                    // send() can return less of data ??
                    if (sendAll(dest_fd, buffer, &nbytes) == -1)
                        std::cout << "Error: send() failed" << std::endl;
                }
            }
        }
}

void Server::acceptNewConnection(){
        // if listener is ready to read, handle new connection
        struct pollfd newFd; 
        struct sockaddr_storage clientAddress = {};
        socklen_t len = sizeof(clientAddress);  
        // accept() will return a new socket fd
        int connectedSocket = accept(_listeningSocket, (struct sockaddr *)&clientAddress, &len);
        if (connectedSocket < 0){
            std::cout << "Error: accept() failed" << std::endl;
			exit(-1);
        }
        newFd.fd = connectedSocket;
        newFd.events = POLLIN;
        _fds.push_back(newFd);
}


/* ???
The connected socket is closed each time through the
loop, but the listening socket remains open for the life of the server.*/
void Server::getConnection(int i){
    if (_fds[i].fd == _listeningSocket){
    	Server::acceptNewConnection();
    }
    else{
		Server::clientRoutine(i);
    }
}

void Server::createPoll(){
	
	/* poll() -> wait for some event on a file descriptor.
        With a non-blocking file descriptor you can wait
        for ANY of a set of fds to become useable -> https://stackoverflow.com/questions/3360797/non-blocking-socket-with-poll
        It saves CPU time for your programm.
    */
    struct pollfd newFds;
  
    newFds.fd = _listeningSocket;
    newFds.events = POLLIN; // "alert me when data is ready to recv() on this socket"
    newFds.revents = 0; // "set to 0 or "not yet ready to read"
    _fds.push_back(newFds);

    std::cout << "Server starting on port " << _port << std::endl;

    /* main loop accepting a client connection */
    for(;;) {
        if (_signal == true){
            break;
        }
        int events = poll(&_fds[0], _fds.size(), -1);
        if (events == -1 && !_signal){
            std::cout << "Error: poll() failed";
            exit(-1);
        }
        else if (events == 0){
            std::cout << "Error: poll() timed out" << std::endl;
            exit(-1);
        }
        // est-ce que le socket de server a recu au moins 1 connexion
        if (_fds[0].revents == POLLIN){
            Server::getConnection(0);  
            std::cout << "Nouvelle connexion" << std::endl;
        }    

        std::vector<struct pollfd> tmpFds = _fds;
        // run through the existing connections looking for data to read
        for (size_t i = 1; i < _fds.size(); ++i){
            // check if someone's ready to read
            if (tmpFds[i].revents == POLLIN){
                Server::getConnection(i);
            }  
        }   
    }
}

void Server::establishSocket(){
	/* defining server address */
    struct sockaddr_in serverAddress; // struct is used to represent an IPv4 address and port number combination

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(_port); // convert the int from machine (host) byte to network byte
    serverAddress.sin_addr.s_addr = INADDR_ANY; // listen to all ips
    memset(&(serverAddress.sin_zero), '\0', sizeof(serverAddress.sin_zero));
   
    /* to prevent bind() from failed when a socket that was connected
     is still hanging around in the kernel, and it's hogging the port */
    int yes = 1;
    if (setsockopt(_listeningSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1){
        std::cout << "Error: setsockopt() failed" << std::endl;
        exit(-1);
    }

    /* when you first create the socket descriptor with socket(),
    the kernel sets it to blocking. If you don't want a socket
    to be blocking, you have to make a call to fcntl()

    * fcntl () -> performs operations on the open fd
    * F_SETFL -> set the file status flags to the value specified by arg
    * O_NONBLOCK -> the bit that enables nonblocking mode for the file.
    */

    if (fcntl(_listeningSocket, F_SETFL, O_NONBLOCK) == -1){
        std::cout << "Error: failed to set a non-blocking flag for the socket" << std::endl;
        exit(-1);
    }

    /* bind() assigns a local protocol address to a socket */
    if (bind(_listeningSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1){
        std::cout << "Error: socket connection failed" << std::endl;
        exit(-1);
    }

    /*  listening for connections 
        SOMAXCONN -> the number of connections allowed on the incoming queue
    */
    if (listen(_listeningSocket, SOMAXCONN) == -1){
        std::cout << "Error: listen() falied" << std::endl;
        exit(-1);
    }
}

bool Server::_signal = false; // default

void Server::signalHandler(int signum){
	(void) signum;
    std::cout << "Signal caught" << std::endl;
    Server::_signal = true; // to stop a server
}

void Server::closeFds(){
    for (size_t i = 0; i < _fds.size(); ++i){
        if (i != 0 && _fds[i].fd != -1){
            close(_fds[i].fd); // close connected socket
            std::cout << "Client " << _fds[i].fd << " disconnected" << std::endl; // to test
        }
    }
    close(_fds[0].fd); // close the server
    std::cout << "Server closed" << std::endl;
}