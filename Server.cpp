/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 11:10:53 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/13 14:24:37 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/* FOR TESTING PURPOSE */
#include <cstdio>
# include "log.hpp"

/* ************************************************************************** */
/* *                             Signal Handling                            * */
/* ************************************************************************** */
int g_signal = 0;

void	signal_handler(int signum) {
	if (signum == SIGINT)
		g_signal = signum;
}

void	set_signal( void ) {
	signal(SIGINT, signal_handler);
}

/* ************************************************************************** */
/* *                       Constructors && Destructors                      * */
/* ************************************************************************** */
Server::Server( void )
	: _port(8080), _passwd("0000")
{
	return ;
}

Server::Server(const int port, const std::string password )
	: _port(port), _passwd(password)
{
	std::cout << "Created Server object using port " << port << std::endl;
	return ;
}

Server::Server( Server const &rhs )
	: _port(rhs._port), _passwd(rhs._passwd)
{
	return ;
}

Server::~Server() {
	log("Closing socket %d.", this->_pollFd[0].fd);
	close(this->_socket);
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
 * Init server variable and signals, create socket, bind it and start listaning
 */
void	Server::startServer( void ) {
	this->_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket == -1)
		throw (std::runtime_error(std::string("socket: ") + std::strerror(errno)));
	this->pollPushBack(this->_socket, POLLIN); //!< Add server to poll

	success_log("Socket %d created", this->_socket);

	this->_socketAdress_len = sizeof(this->_socketAddress);
	std::memset(&(this->_socketAddress), 0, this->_socketAdress_len);
	this->_socketAddress.sin_family = AF_INET;
	this->_socketAddress.sin_port = htons(this->_port);
	this->_socketAddress.sin_addr.s_addr = inet_addr("0.0.0.0");
	int	reusable_addr = 1;
	if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, &reusable_addr, sizeof(reusable_addr)) < 0)
		throw (std::runtime_error(std::string("setsockopt: ") + std::strerror(errno)));

	success_log("AdressSocket created: %d.", this->_socketAddress.sin_addr);

	if (bind(this->_socket, (struct sockaddr *)&this->_socketAddress,
		this->_socketAdress_len) < 0)
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
	std::cout << std::endl;
	log("%s======= SHUTDOWN SIGNAL RECEIVED =======%s", WHT, RESET);
	if (this->_pollFd.size() <= 1)
		return ;

	log("Disconecting every client...");
	for (unsigned long int i = this->_pollFd.size() - 1; i > 0; --i) {
		log("Closing socket %d.", this->_pollFd[i].fd);
		close(this->_pollFd[i].fd);
		this->_pollFd.pop_back();
	}
	log("Ending server...");
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
		buffer[buffer_size - 1] = '\0';
		log("Recevied from client on socket %d: %s", this->_pollFd[i].fd, buffer);
		return ;
	}
	if (buffer_size == 0)
		war_log("Received no data on socket %d", this->_pollFd[i].fd);
	if (buffer_size == -1)
		err_log("Could not received data on socket %d: %srecv%s: %s.", this->_pollFd[i].fd, MAG, RESET, std::strerror(errno));
	disconnectClient(i);
}

/**
 * @brief Treat the event caught by poll
 *
 * This function will treat the event caught by poll.
 *
 * It either catch a POLLIN event and will read the client Msg.
 * Or it received on error and will display a log according to the error
 *
 * @param i The pollFd index
 */
void	Server::receiveEvent( long unsigned int& i ) {
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

	log("Server waiting for connection...");

	//! Perform poll check until error or signal catch
	while (poll(this->_pollFd.data(), this->_pollFd.size(), -1) > 0) {

		if (this->_pollFd[0].revents == POLLIN) //!< New client connecting
			this->acceptNewClient();

		//! Check if data has been received on client socket
		for (long unsigned int i = 1; i < this->_pollFd.size(); i++) {
			if (this->_pollFd[i].fd > 0)
				this->receiveEvent(i);
			else if (this->_pollFd[i].fd == 0) { //! Case unused fd
				war_log("Removed unused fd from pollfd [%sShouldn't happened%s]", YEL, RESET);
				disconnectClient(i);
			} else
				war_log("Why tha F*** socket = %d?", this->_pollFd[i].fd);
		}
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
