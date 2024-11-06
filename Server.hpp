#ifndef SERVER_HPP
# define SERVER_HPP
# include <iostream>

class Server
{
    public:
        Server(void);
        Server(const Server& other);
        Server &operator=(const Server &other);
        ~Server();
};

#endif

