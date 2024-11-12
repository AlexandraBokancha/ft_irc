#include "Server.hpp"

// Default constructor
Server::Server(void){
}

Server::Server(int socket) : _socket(socket){
    if (_socket < 0){
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
    std::cout << "Destructor called" << std::endl;
    return ;
}

std::vector<struct pollfd> Server::getFdsVec(){
    return this->_fds;
}

int Server::getFdsSize(){
    return this->_fds.size();
}

int Server::getSocket(){
    return this->_socket;
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


void Server::getConnection(int i){
    if (_fds[i].fd == _socket){
       
        // if listener is ready to read, handle new connection
        struct pollfd newFd; 
        struct sockaddr_storage clientAddress = {};
        socklen_t len = sizeof(clientAddress);  
        // accept() will return a new socket fd
        int clientSocket = accept(_socket, (struct sockaddr *)&clientAddress, &len);
        if (clientSocket < 0){
            std::cout << "Error: accept() failed" << std::endl;
        }
        newFd.fd = clientSocket;
        newFd.events = POLLIN;
        _fds.push_back(newFd);
    }
    else{
        // if not the listener -> it's a regular client
        
        /* receiving data from the client with cmd recv() */
        
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
            _fds[i].fd = -1;  // delete from __fds[i] ? 
        }
        else{
            // send to everyone
            std::cout << "Message from client: " << buffer << std::endl;
            for (int j = 0; j < getFdsSize(); ++j){
                int dest_fd = _fds[j].fd;
                // si ce n'est pas un serveur, et si ce n'est pas lui-meme
                if (dest_fd != _socket && dest_fd != _fds[i].fd){
                    // send() can return less of data ??
                    if (sendAll(dest_fd, buffer, &nbytes) == -1)
                        std::cout << "Error: send() failed" << std::endl;
                }
            }
        }
    }
}