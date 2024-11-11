# include "Server.hpp"
# include "Client.hpp"

/* sous UNIX les sockets sont de simples descripteurs de fichiers 
stream-sockets are reliable two-way connected communication stream */

/* TCP makes sure your data arrives sequentially and error-free */

/* Port number is a 16-bit number that's like the local address for the connection
All ports below 1024 are RESERVED, you can use a port up to 65535 */

/* non-blocking calls don't block the thread until they finish
their work; instead, this type of system call returns immediately
after completing its job without having any effect on what's happening
in other threads */


int main(int ac, char *av[]){
    (void) ac;
    // const char *password;
    int port = atoi(av[1]); 

    /* AF_INET = IPv4 protocol, SOCK_STREAN = TCP type socket*/

    Server irc(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));

    /* defining server address */
    struct sockaddr_in serverAddress; // struct is used to represent an IPv4 address and port number combination

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port); // convert the int from machine (host) byte to network byte
    serverAddress.sin_addr.s_addr = INADDR_ANY; // listen to all ips
    memset(&(serverAddress.sin_zero), '\0', sizeof(serverAddress.sin_zero));
   
    /* to prevent bind() from failed when a socket that was connected
     is still hanging around in the kernel, and it's hogging the port */
    int yes = 1;
    if (setsockopt(irc.getSocket(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1){
        std::cout << "Error: setsockopt() failed" << std::endl;
        exit(-1);
    }

    /* when you first create the socket descriptor with socket(),
    the kernel sets it to blocking. If you don't want a socket
    to be blocking, you have to make a call to fcntl()

    * fcntl () -> performs operations on the open fd
    * F_SETFL -> set the file status flags to the value specified by arg
    * O_NONBLOCK -> the bit that enables nonblocking mode for the file.
    */
    if (fcntl(irc.getSocket(), F_SETFL, O_NONBLOCK) == -1){
        std::cout << "Error: failed to set a non-blocking flag for the socket" << std::endl;
        exit(-1); // < remplacer par l'exception ?>
    }

    /* binding the server socket */
    if (bind(irc.getSocket(), (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1){
        std::cout << "Error: socket connection failed" << std::endl;
        exit(-1); // < remplacer par l'exception ?>
    }

    /* listening for connections */
    if (listen(irc.getSocket(), SOMAXCONN) == -1){
        std::cout << "Error: listen() falied" << std::endl;
        exit(-1); // < remplacer par l'exception ?>
    }

    /* poll() -> wait for some event on a file descriptor.
        With a non-blocking file descriptor you can wait
        for ANY of a set of fds to become useable -> https://stackoverflow.com/questions/3360797/non-blocking-socket-with-poll
        It saves CPU time for your programm.
    */
    struct pollfd newFds;
  
    newFds.fd = irc.getSocket();
    newFds.events = POLLIN; // "alert me when data is ready to recv() on this socket"
    newFds.revents = 0; // "set to 0 or "not yet ready to read"
    irc.pushPollfd(newFds);

    std::cout << "Server starting on port " << port << std::endl;

    /* accepting a client connection */
    for (;;){
        if (poll(&(irc.getFds()), irc.getFdsSize(), -1) == -1){
            std::cout << "Error: poll() failed";
            exit(-1);
        }
        // est-ce que le socket de server a recu au moins 1 connexion
        if (irc.getFds().revents == POLLIN){
            irc.getConnection(0);  
            std::cout << "Nouvelle connexion" << std::endl;
        }    

        std::vector<struct pollfd> tmpFds = irc.getFdsVec();
        // run through the existing connections looking for data to read
        for (int i = 1; i < irc.getFdsSize(); ++i){
            // check if someone's ready to read
            if (tmpFds[i].revents == POLLIN){
                irc.getConnection(i);
            }  
        }   
    }

    /* closing the server socket */
    close(irc.getSocket());
    return 0;
}