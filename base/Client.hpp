#ifndef CLIENT_HPP
# define CLIENT_HPP
# include <iostream>

class Client
{
    public:
        Client(void);
        Client(int socket);
        Client(const Client& other);
        Client &operator=(const Client &other);
        ~Client();


    private:

        std::string                 _nickName;
        std::string                 _userName;
        int                         _userSocket;
};

#endif

