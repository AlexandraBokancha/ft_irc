#include "Bot.hpp"

// Default constructor
Bot::Bot(void) : _port(6667), _hostname("localhost"), _username("ircBot"), _nickname("bot"){
    std::cout << "Bot is connected to " << _hostname << " on port " << _port << std::endl;
}

// Copy constructor
Bot::Bot(const Bot &other)
{
    std::cout << "Copy constructor called" << std::endl;
    (void) other;
    return ;
}

// Assignment operator overload
Bot &Bot::operator=(const Bot &other)
{
    std::cout << "Assignment operator called" << std::endl;
    (void) other;
    return (*this);
}

// Destructor
Bot::~Bot(void)
{
    std::cout << "Destructor called" << std::endl;
    return ;
}

