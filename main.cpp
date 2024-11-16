#include "Server.hpp"
#include "log.hpp"
// # include "Client.hpp"

/* sous UNIX les sockets sont de simples descripteurs de fichiers 
stream-sockets are reliable two-way connected communication stream */

/* TCP makes sure your data arrives sequentially and error-free */

/* Port number is a 16-bit number that's like the local address for the connection
All ports below 1024 are RESERVED, you can use a port up to 65535.

From "UNIX network programming":

Port should be greater than 1023 (we do not need a reserved port), 
less than 49152 (to avoid conflict with the ‘‘correct’’ range of ephemeral ports),
and it should not conflict with any registered port.
*/

/* non-blocking calls don't block the thread until they finish
their work; instead, this type of system call returns immediately
after completing its job without having any effect on what's happening
in other threads */

/**
 * @brief Parse the input argument
 * Parse the program argument throwing an error if arguments are invalid 
 * and saving the <port> and  <password> into the last two arguments.
 *
 * @param ac The number of argument
 * @param av Argument list @param port A reference to the port
 * @param passwd A reference to the password
 */
void	parse_arg(int ac, char **av, int& port, std::string& passwd) {
	int					tmp_port;
	char				invalid_arg;
	std::stringstream	ss;

	if (ac != 3)
		throw (Server::InvalidArgException());	
	ss.clear();
	ss.str(std::string(av[1]));
	if (!(ss >> tmp_port))
		throw (Server::InvalidPortException());
	if (ss >> invalid_arg)
		throw (Server::InvalidPortException());
	if (tmp_port < 0 || tmp_port > 65535)
		throw (Server::InvalidPortException());
	port = tmp_port;
	passwd = std::string(av[2]);
}

/**
 * @brief The main function
 *
 * This function :
 *     Parse arguments
 *     Start server
 *     Run   server
 *     Stop  server
 *
 * And it catch errors obviously
 */
int	main(int ac, char **av) {
	int				port;
	std::string		passwd;

	try {
		parse_arg(ac, av, port, passwd);
	} catch (Server::InvalidPortException& e ) {
		fatal_log((std::string(e.what()) + ": " + av[1]).c_str());
		return (1);
	} catch (std::exception& e) {
		fatal_log(e.what());
		return (1);
	}

	Server	irc_serv(port, passwd); //!< Creating the server
	try {
		irc_serv.startServer(av[1]);
		irc_serv.runServer();
	} catch (std::exception& e) {
		fatal_log(e.what());
		return (irc_serv.stopServer(), 1);
	}
	return (irc_serv.stopServer(), 0);
}
