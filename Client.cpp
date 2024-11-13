#include "Client.hpp"

// Default constructor
Client::Client(void)
{
    std::cout << "Default constructor called" << std::endl;
    return ;
}
Client::Client(int socket) : _connectedSocket(socket){};

// Copy constructor
Client::Client(const Client &other)
{
    std::cout << "Copy constructor called" << std::endl;
    (void) other;
    return ;
}

// Assignment operator overload
Client &Client::operator=(const Client &other)
{
    std::cout << "Assignment operator called" << std::endl;
    (void) other;
    return (*this);
}

// Destructor
Client::~Client(void)
{
   // std::cout << "Destructor called" << std::endl;
    return ;
}

