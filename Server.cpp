/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 11:10:53 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/13 12:25:59 by dbaladro         ###   ########.fr       */
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
 * @brief Add new <styruct pollfd> to server poll
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
	log("%s======= SHUTDOWN SIGNAL RECEIVED =======%s", GRN, RESET);
	log("Disconecting every client...");
	for (unsigned long i = this->_pollFd.size() - 1; i > 0; --i) {
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
 * @brief Run the server
 */
void	Server::runServer( void ) {

	log("Server waiting for connection...");

	//! Perform poll check until error or signal catch
	while (poll(this->_pollFd.data(), this->_pollFd.size(), -1) > 0) {

		if (this->_pollFd[0].revents == POLLIN) //!< New client connecting
			this->acceptNewClient();

		//! Check if data has been received on client socket
		for (std::size_t i = 1; i < this->_pollFd.size(); i++) {
			if (this->_pollFd[i].fd > 0) {
				if (this->_pollFd[i].revents & POLLIN) {
					char	buffer[512]; //! 512 = max irc message size

					int buffer_size = recv(this->_pollFd[i].fd, buffer, 512 - 1, MSG_DONTWAIT);
					if (buffer_size == -1) {
						err_log("Could not received data on socket %d: %srecv%s: %s.", this->_pollFd[i].fd, MAG, RESET, std::strerror(errno));
						// this->_pollFd.erase(this->_pollFd.begin() + i);
						// i--;
					} else if (buffer_size == 0) {
						war_log("Received no data on socket %d", this->_pollFd[i].fd);
						log("Closing socket %d", this->_pollFd[i].fd);
						close(this->_pollFd[i].fd);
						this->_pollFd.erase(this->_pollFd.begin() + i);
						i--;
					} else {
						buffer[buffer_size - 1] = '\0';
						log("Recevied from client on socket %d: %s", this->_pollFd[i].fd, buffer);
					}
					continue ;
				}
				if (this->_pollFd[i].revents & POLLHUP) {
					err_log("Disconnected client on socket %d.", this->_pollFd[i].fd);
					close(this->_pollFd[i].fd);
					this->_pollFd.erase(this->_pollFd.begin() + i);
					i--;
					continue ;
				}
				if (this->_pollFd[i].revents & POLLERR) {

					err_log("Error on socket %d.", this->_pollFd[i].fd);
					close(this->_pollFd[i].fd);
					this->_pollFd.erase(this->_pollFd.begin() + i);
					i--;
					continue ;
				}
				if (this->_pollFd[i].revents & POLLNVAL) {
					err_log("Invalid request on socket %d.", this->_pollFd[i].fd);
					close(this->_pollFd[i].fd);
					this->_pollFd.erase(this->_pollFd.begin() + i);
					i--;
					continue;
				}
				continue ;
			}
			if (this->_pollFd[i].fd == 0) { //! Case unused fd
				war_log("Removed unused fd from pollfd [%sShouldn't happened%s]", YEL, RESET);
				this->_pollFd.erase(this->_pollFd.begin() + i);
				i--;
				continue ;
			}
			war_log("WTF happened on socket %d", this->_pollFd[i].fd);
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
