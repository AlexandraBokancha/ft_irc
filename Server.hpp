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
# include <fcntl.h>

class Server
{
    public:
        Server(void);
        Server(int socket);
        Server(const Server& other);
        Server &operator=(const Server &other);
        ~Server();
        
        void pushPollfd(struct pollfd & fd){
            this->_fds.push_back(fd);
        }

        struct pollfd & getFds(){
            return this->_fds[0];
        }

        std::vector<struct pollfd>  getFdsVec();
        int                         getFdsSize();
        int                         getSocket();
        void                        getConnection(int i);

    private:

        std::vector<struct pollfd>  _fds; 
        int                         _socket;

        int sendAll(int socket, char *buf, int *len);
};

#endif

