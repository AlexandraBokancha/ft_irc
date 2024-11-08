#ifndef CLIENT_HPP
# define CLIENT_HPP
# include <iostream>

class Client
{
    public:
        Client(void);
        Client(int socket, const std::string & ip, int port);
        Client(const Client& other);
        Client &operator=(const Client &other);
        ~Client();
   
    private:

        int                 _socket;
        const std::string   _ip;
        int                 _port;
};

#endif

