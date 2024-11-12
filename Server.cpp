/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 11:10:53 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/12 22:29:38 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/* FOR TESTING PURPOSE */
#include <asm-generic/socket.h>
#include <csignal>
#include <cstdio>
#include <sys/poll.h>
#include <sys/socket.h>
# include "log.hpp"

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
	close(this->_socket);
	std::cout << "Deleting Server object closing socket " << BLU << this->_socket << RESET << std::endl;
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

void	Server::pollPushBack(int fd, short events) {
	struct pollfd	new_socket;
	
	new_socket.fd = fd;
	new_socket.events = events;
	// new_socket.revents = revents;
	this->_pollFd.push_back(new_socket);
}

/**
 * @brief Create server socket and bind it.
 *
 * Create a server socket, set socketAddress and bind it.
 */
void	Server::startServer( void ) {
	this->_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket == -1)
		throw (std::runtime_error(std::string("socket: ") + std::strerror(errno)));

	log("Socket %d created", this->_socket);

	this->_socketAdress_len = sizeof(this->_socketAddress);
	std::memset(&(this->_socketAddress), 0, this->_socketAdress_len);
	this->_socketAddress.sin_family = AF_INET;
	this->_socketAddress.sin_port = htons(this->_port);
	this->_socketAddress.sin_addr.s_addr = inet_addr("0.0.0.0");
	int	reusable_addr = 1;
	if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, &reusable_addr, sizeof(reusable_addr)) < 0)
		throw (std::runtime_error(std::string("bind: ") + std::strerror(errno)));

	log("AdressSocket created.");

	if (bind(this->_socket, (struct sockaddr *)&this->_socketAddress,
		this->_socketAdress_len) < 0)
		throw (std::runtime_error(std::string("bind: ") + std::strerror(errno)));


	log("Binded socket %d.", this->_socket);

	if (listen(this->_socket, 256) < 0)
		throw (std::runtime_error(std::string("listen: ") + std::strerror(errno)));

	log("Socket %d is listening on port %d.", this->_socket, this->_port);

	this->pollPushBack(this->_socket, POLLIN); //!< Add server to poll
}

void	Server::waitClient( void ) {
	int					client_socket;
	struct sockaddr_in	client_addr;
	int					addr_len = sizeof(client_addr);

	log("Server waiting for connection...");

	//! Perform poll check until error
	while (poll(this->_pollFd.data(), this->_pollFd.size(), -1) > 0) {

		//! Check if new client is trying to connect
		if (this->_pollFd[0].revents == POLLIN) {
			//! Accept new client
			client_socket = accept(this->_socket, (struct sockaddr*)&client_addr, reinterpret_cast<socklen_t *>(&addr_len));
			if (client_socket < 0) {
				war_log("%sINTERNAL ERROR%s: %saccept%s: %s", RED, RESET, MAG, RESET, std::strerror(errno));
				break ;
			}
			log("Accepted client on: %d", inet_ntoa(client_addr.sin_addr));
			this->pollPushBack(client_socket, POLLIN); //!< Add client to poll
		}

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
