/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 11:10:53 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/16 22:31:46 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/* FOR TESTING PURPOSE */
# include "log.hpp"

/* ************************************************************************** */
/* *                             Signal Handling                            * */
/* ************************************************************************** */
int g_signal = 0;

void	signal_handler(int signum) {
	if (signum == SIGINT || signum == SIGQUIT) {
		std::cout << std::endl;
		log("%s======= SHUTDOWN SIGNAL RECEIVED =======%s", BLU, RESET);
		g_signal = signum;
	}
}

void	set_signal( void ) {
	signal(SIGQUIT, signal_handler);
	signal(SIGINT, signal_handler);
}

/* ************************************************************************** */
/* *                       Constructors && Destructors                      * */
/* ************************************************************************** */
Server::Server( void )
	: _port(8080), _passwd("0000")
{
	this->_serverInfo = NULL;
	this->_socket = -1;
	return ;
}

Server::Server(const int port, const std::string password )
	: _port(port), _passwd(password)
{
	std::cout << "Created Server object using port " << port << std::endl;
	this->_serverInfo = NULL;
	this->_socket = -1;
	return ;
}

Server::Server( Server const &rhs )
	: _port(rhs._port), _passwd(rhs._passwd)
{
	this->_serverInfo = NULL;
	this->_socket = -1;
	return ;
}

Server::~Server() {
	return ;
}

/* ************************************************************************** */
/* *                             Operator Overload                          * */
/* ************************************************************************** */
Server& Server::operator=( Server const & rhs ) {
	if (this != &rhs) {
		*this = rhs;
	}
	return (*this);
}

/* ************************************************************************** */
/* *                              Member function                           * */
/* ************************************************************************** */

/**
 * @brief Add new <struct pollfd> to server poll
 *
 * Add new element to poll
 *
 * @param fd The socket file descriptor to add
 * @param events The poll event mask
 */
void	Server::pollPushBack(int fd, short events) {
	struct pollfd	new_socket;
	
	new_socket.fd = fd;
	new_socket.events = events;
	new_socket.revents = 0;
	this->_pollFd.push_back(new_socket);
}

/**
 * @brief Disconnect client and remove it from poll
 *
 * This function remove the client-index from poll.
 * It does it by first closing the client socket,
 * then it remove all of it's data from poll
 *
 * BEWARE:
 *     The index is reduced by one and this behavioue shouldn't be changed
 *     For the moment !
 *
 * @param index	The client index in the pollFd
 */
void	Server::disconnectClient( long unsigned int& index ) {
	if (index == 0)
		return ;
	log("Closing socket %d.", this->_pollFd[index].fd);
	close(this->_pollFd[index].fd);
	this->_pollFd.erase(this->_pollFd.begin() + index);
	index--;

}

                           /* SERVER HANDLING */
/**
 * @brief Start the server
 *
 * Init server variable and signals, create socket, bind it and start listening
 */
void	Server::startServer( const char *port_str ) {
	struct addrinfo	hints;
	struct addrinfo	*servInfo;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;		 //!< Use AF_UNSPEC for both ipv4 ipv6 interfaces
	hints.ai_socktype = SOCK_STREAM; //!< TCP stream socket
	hints.ai_flags = AI_PASSIVE;	 //!< <=> INADDR_ANY <=> "0.0.0.0"
									 //!< Used by application that indent to accept
									 //!< connexion on any of the host net adresses
	if (getaddrinfo(NULL, port_str, &hints, &servInfo) != 0)
		throw (std::runtime_error(std::string("getaddrinfo: ") + std::strerror(errno)));
	this->_serverInfo = servInfo;


	this->_socket = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol);
	if (this->_socket == -1)
		throw (std::runtime_error(std::string("socket: ") + std::strerror(errno)));
	this->pollPushBack(this->_socket, POLLIN); //!< Add server to poll
	int	reusable_addr = 1;
	if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &reusable_addr, sizeof(reusable_addr)) < 0)
		throw (std::runtime_error(std::string("setsockopt: ") + std::strerror(errno)));

	success_log("Socket %d created", this->_socket);

	if (bind(this->_socket, this->_serverInfo->ai_addr, this->_serverInfo->ai_addrlen) < 0)
		throw (std::runtime_error(std::string("bind: ") + std::strerror(errno)));

	success_log("Binded socket %d.", this->_socket);

	if (listen(this->_socket, 256) < 0)
		throw (std::runtime_error(std::string("listen: ") + std::strerror(errno)));

	log("Socket %d is listening on port %d.", this->_socket, this->_port);

	set_signal();
}

/**
 * @brief Stop the server
 *
 * Stop the server by disconnecting every client and closing associated sockets
 * Should be called just before server desctructor / End of program
 */
void		Server::stopServer( void ) {
	if (this->_pollFd.size() > 1) {
		log("Disconecting every client...");
		unsigned long int i = this->_pollFd.size() - 1;
		while (i > 0)
			disconnectClient(i);
	}
	log("%sStopping server%s", WHT, RESET);
	if (this->_socket != -1) {
		log("Closing socket %d.", this->_socket);
		close(this->_socket);
	}
	if (this->_serverInfo != NULL) {
		log("Freeing server informations");
		freeaddrinfo(this->_serverInfo);
	}
	log("Server stopped!");
}

/**
 * @brief Accept a new client connecting to the server
 *
 * Accept new client after a POLLIN event have been caught on the Server socket
 */
void	Server::acceptNewClient( void ) {
	int					client_socket;
	struct sockaddr_in	client_addr;
	int					addr_len = sizeof(client_addr);

		client_socket = accept(this->_socket, (struct sockaddr*)&client_addr,
			reinterpret_cast<socklen_t *>(&addr_len));
		if (client_socket < 0) //!< accept error
			return (err_log("%sInternal error%s: %saccept%s: %s",
					RED, RESET, MAG, RESET, std::strerror(errno)));
		success_log("accepted client on: %s", inet_ntoa(client_addr.sin_addr));
		this->pollPushBack(client_socket, POLLIN); //!< add client to poll
}


/**
 * @brief Send the complete message to Client
 *
 * Protected send ensuring that the full message real the client at one time
 *
 * @param socket The client socket
 * @param buf The buffer containing the message to send
 * @param len The message len
 *
 * @return 0 on SUCCESS; -1 on ERROR
 */
int Server::sendMsg(int socket, const char *buf, int len) const {
    int total = 0; // bytes sent
    int left = len; // bytes left to sent
    int b;

    while (total < len){
        b = send(socket, buf + total, left, MSG_DONTWAIT);
        if (b == -1) {
			err_log("send(): %s.", std::strerror(errno));
			break;
		}
        total += b;
        left -= b;
    }
    // *len = total; //!< Why
    return (b == -1 ? -1 : 0); // return -1 on error, 0 on success
}

/**
 * @brief Send msg to all client except the one specified by fd
 *
 * This function is needed to broadcast the irc message when needed
 *
 * @param buffer The buffer containing the message to send
 * @param len The message len
 * @param fd The client exluded from receiving the message (the sender)
 */
void	Server::broadcast(const char *buffer, int len, int fd) const {
	int	total = 0; // bytes sent
	int	send_result;

	for (unsigned long int i = 1; i < this->_pollFd.size(); i++) {
		if (this->_pollFd[i].fd == fd)
			continue ;

		log("Sending back msg received on socket %d to %d", fd, this->_pollFd[i].fd);

		if (sendMsg(this->_pollFd[i].fd, buffer, len) == -1)
			err_log("Msg could not be sent on socket %d", this->_pollFd[i].fd);
	}
}

/**
 * @brief Receive client message after POLLIN event
 *
 * This function read the Msg send byy the client after POLLIN event
 * IT SHOULD BE UPDATED ACCORDINGLY
 *
 * @param i The pollFd index
 */
void	Server::receiveMsg( long unsigned int& i ) {
	char	buffer[512]; //<! 512 = max irc message size
	
	int buffer_size = recv(this->_pollFd[i].fd, buffer, 512 - 1, MSG_DONTWAIT);

	if (buffer_size > 0) { //!< Received msg
		buffer[buffer_size] = '\0';
		log("Recevied from client on socket %d: %s", this->_pollFd[i].fd, buffer);
		broadcast(buffer, buffer_size, this->_pollFd[i].fd);
		return ;
	}
	if (buffer_size == 0)
		log("Client disconnected on socket %d", this->_pollFd[i].fd);
	if (buffer_size == -1)
		err_log("Could not received data on socket %d: %srecv%s: %s.", this->_pollFd[i].fd, MAG, RESET, std::strerror(errno));
	disconnectClient(i);
}

/**
 * @brief Check if an event have been received and treat it if received
 *
 * This function check if an event have been received on the revents field
 *
 * It either catch a POLLIN event and will read the client Msg.
 * Or it received on ERROR and will display a log according to the error
 * Or it do nothing
 *
 * @param i The pollFd index
 */
void	Server::checkEvent( long unsigned int& i ) {
	//! Receive nothing
	if (this->_pollFd[i].revents == 0)
		return ;

	//! Receive data
	if (this->_pollFd[i].revents & POLLIN)
		return (this->receiveMsg(i));	

	//! received error
	if (this->_pollFd[i].revents & POLLHUP)
		err_log("Client hang up on socket %d.", this->_pollFd[i].fd);
	if (this->_pollFd[i].revents & POLLERR)
		err_log("Error on socket %d.", this->_pollFd[i].fd);
	if (this->_pollFd[i].revents & POLLNVAL)
		err_log("Invalid request on socket %d.", this->_pollFd[i].fd);
	disconnectClient(i);
}

/**
 * @brief Run the server
 */
void	Server::runServer( void ) {
	int	status;

	log("Server waiting for connection...");

	//! Perform poll check until error or signal catch
	while ((status = poll(this->_pollFd.data(), this->_pollFd.size(), -1)) > 0) {

		if (this->_pollFd[0].revents == POLLIN) //!< New client connecting
			this->acceptNewClient();

		//! Check if data has been received on client socket
		for (long unsigned int i = 1; i < this->_pollFd.size(); i++) {
			if (this->_pollFd[i].fd > 0)
				this->checkEvent(i);
			else if (this->_pollFd[i].fd == 0) { //! Case unused fd
				war_log("Removed unused fd from pollfd [%sShouldn't happened%s]", YEL, RESET);
				disconnectClient(i);
			} else
				war_log("Why tha F*** socket = %d?", this->_pollFd[i].fd);
		}
        if (status == 0)
            fatal_log("poll(): Timeout");
        if (status < 0)
			fatal_log("poll(): %s.", std::strerror(errno));
	}
}

/* ************************************************************************** */
/* *                             Class  Exception                           * */
/* ************************************************************************** */
const char*	Server::InvalidArgException::what() const throw() {
	return ("Use the program as folow: ./ircserv <port> <password>");
}

const char*	Server::InvalidPortException::what() const throw() {
	return ("ircserv: Invalid port");
}
