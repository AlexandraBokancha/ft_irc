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
# include <csignal>

class Server
{
    public:

        Server(int socket, int port);
        Server(const Server& other);
        Server &operator=(const Server &other);
        ~Server();
    
		/* getters */
        std::vector<struct pollfd>  getFdsVec();
        int                         getFdsSize();
        int                         getSocket();

		/* function to establish a socket which
		can accept calls. use of non-blocking socket for i/o operations.*/
		void						establishSocket();

		/* main function using poll() for handle multiple file descriptors (clients)*/
		void						createPoll();

		/* function that accept() a new connection, 
		and then handle recv() and send() operations */
        void                        getConnection(int i);
		
		/* function inside getConnection used to accept and
		create a new socket fd of a user trying to connect
		to a server */
		void						acceptNewConnection();

		/* function receiveing and sending data from a client(user) 
		connected to a server */
		void						clientRoutine(int i);

		/* improved send() function to handle big data 
		send by a user */
        int							sendAll(int socket, char *buf, int *len);

		static void					signalHandler(int signum);
        
        void                        closeFds();

    private:

        std::vector<struct pollfd>  _fds;
        static bool                 _signal; 
        int                         _listeningSocket;
		int							_port;
		
};

#endif

