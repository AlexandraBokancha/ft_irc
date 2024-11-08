#ifndef SERVER_HPP
# define SERVER_HPP
# include <iostream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <cstdlib>

class Server
{
    public:
        Server(void);
        Server(const Server& other);
        Server &operator=(const Server &other);
        ~Server();
};

#endif

