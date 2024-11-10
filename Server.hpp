#ifndef SERVER_HPP
# define SERVER_HPP
# include <iostream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <cstdlib>
# include <cstring>
# include <poll.h>
# include <vector>

class Server
{
    public:
        Server(void);
        Server(const Server& other);
        Server &operator=(const Server &other);
        ~Server();
        
        void pushPollfd(struct pollfd & fd){
            this->_fds.push_back(fd);
        }

    private:
        std::vector<struct pollfd> _fds; 
};

#endif

