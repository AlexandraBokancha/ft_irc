#ifndef CLIENT_HPP
# define CLIENT_HPP
# include <iostream>

class Client
{
    public:
        Client(void);
        Client(const Client& other);
        Client &operator=(const Client &other);
        ~Client();
};

#endif

