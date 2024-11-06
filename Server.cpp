#include "Server.hpp"

// Default constructor
Server::Server(void)
{
    std::cout << "Default constructor called" << std::endl;
    return ;
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

