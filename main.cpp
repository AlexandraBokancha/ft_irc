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

    Server irc(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP), port);

	irc.establishSocket();
	irc.createPoll();
 
    /* closing the server socket */
    close(irc.getSocket());
    return 0;
}