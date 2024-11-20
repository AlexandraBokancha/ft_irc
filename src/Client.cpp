/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 09:34:22 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/20 19:26:32 by alexandra        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

/* ************************************************************************** */
/* *                       Constructors && Destructors                      * */
/* ************************************************************************** */
Client::Client( void ) {
	std::memset(&this->_netId, 0, sizeof(this->_netId));
	this->_nickname = "";
	this->_hostname = "";
	this->_username = "";
	this->_fd = NULL;
	this->_isRegistred = false;
	return ;
}

Client::Client( const Client& rhs )
	: _netId(rhs._netId), _nickname(rhs._nickname), _hostname(rhs._hostname), 
		_username(rhs._username), _fd(rhs._fd) {
	return ;
}

Client::~Client() {
	return ;
}

/* ************************************************************************** */
/* *                            Getters and Setters                         * */
/* ************************************************************************** */
struct sockaddr_in	Client::getNetId( void ) const {
	return (this->_netId);
}

std::string	Client::getHostname( void ) const {
	return (this->_hostname);
}

std::string	Client::getUsername( void ) const {
	return (this->_username);
}

std::string	Client::getNickname( void ) const {
	return (this->_nickname);
}

bool Client::getRegistred( void ) const{
	return (this->_isRegistred);
}

int*	Client::getFd( void ) const {
	return (this->_fd);
}

void Client::setRegistred( int status ){
	this->_isRegistred = status;
}

void	Client::setFd( int *fd ) {
	this->_fd = fd;
}

void	Client::setNetId( struct sockaddr_in netId) {
	this->_netId = netId;
}

void	Client::setUsername( Message *obj ){
		
}

/**
 * @brief Handle client nickname assignment during registration
 *
 * Validates and assigns a nickname to the client based on the IRC protocol rules.
 * If the nickname is already in use, it sends an `ERR_NICKNAMEINUSE` response.
 * If the nickname is too long or contains invalid characters, it sends an `ERR_ERRONEUSNICKNAME` response.
 * If no nickname is provided, it sends an `ERR_NONICKNAMEGIVEN` response.
 *
 * Conditions checked:
 * - The nickname must be unique among connected clients.
 * - The nickname must not exceed 9 characters in length.
 * 
 * Successful registration logs the event.
 * Throws a runtime exception if the nickname is invalid or rejected.
 *
 * @param obj Pointer to the Message object containing the `NICK` command and parameters.
 * @param clients A vector of connected clients for nickname uniqueness verification.
 */
void	Client::setNickname( Message *obj, std::vector<Client> clients ){
	if (obj->getCommand() == "NICK"){ // a enlever ce "if" apres
		if (obj->getParam().size() == 1 || obj->getParam().size() == 2){
			for (int i = 0; i < clients.size(); ++i){
				if (clients[i].getNickname() == obj->getParam().front()){
					std::string msg = ERR_NICKNAMEINUSE(obj->getParam().front());
					write(*_fd, msg.c_str(), msg.length());
					throw std::runtime_error("Nick rejected");
					}
				}
			if (obj->getParam().front().length() <= 9){ // est-ce qu'il y a dautres chose a check ? 
				this->_nickname = obj->getParam().front();
				success_log("NICK of %d enregistred", getFd());
				return ;
			}
			else{
				std::string msg = ERR_ERRONEUSNICKNAME(obj->getParam().front());
				write(*_fd, msg.c_str(), msg.length());
				throw std::runtime_error("Nick rejected");
			}
		}	
		else{
			write(*_fd, ERR_NONICKNAMEGIVEN(), sizeof(ERR_NONICKNAMEGIVEN()));
			throw std::runtime_error("Nick rejected");
		}
	}	
}


/**
 * @brief Handle client password verification during registration
 *
 * If the client is already registered, it sends an `ERR_ALREADYREGISTRED` response.
 * If the password matches the server's password, the registration succeeds.
 * Otherwise, it sends an `ERR_PASSWDMISMATCH` response and throws an exception. 
 * If the `PASS` command is missing parameters, an `ERR_NEEDMOREPARAMS` response is sent.
 *
 * @param obj Pointer to the Message object
 * @param serverPasswd The server’s correct password
 */
void	Client::setPassword( Message *obj, std::string serverPasswd){
	if (this->_isRegistred == true){
		std::string msg = ERR_ALREADYREGISTRED(this->_nickname); // faire une fonction qui envoie le msg au client
		write(*_fd, msg.c_str(), msg.length());
		return ;
	}
	if (obj->getCommand() == "PASS"){ // a voir si je laisse ce "if" (si on fait le tableau des pointeurs sur les fonctions)
		if (obj->getParam().size() == 1){
			if (obj->getParam().front() == serverPasswd){
				this->_password = obj->getParam().front();
				success_log("Password confirmed");
				return ;
			}
			else{
				write(*_fd, ERR_PASSWDMISMATCH(), sizeof(ERR_PASSWDMISMATCH()));
				throw std::runtime_error("Wrong password");
			}
		}
		else{
			std::string msg = ERR_NEEDMOREPARAMS(obj->getCommand());
			write(*_fd, msg.c_str(), msg.length());
			throw std::runtime_error("Wrong PASS cmd format");
		}
	}	
}

/* ************************************************************************** */
/* *                             Operator Overload                          * */
/* ************************************************************************** */
Client&	Client::operator=( Client const& rhs ) {
	if (this != &rhs) {
		this->_netId = rhs._netId;
		this->_nickname = rhs._nickname;
		this->_hostname = rhs._hostname;
		this->_username = rhs._username;
		this->_fd = rhs._fd;
	}
	return (*this);
}

std::ostream&	operator<<( std::ostream& os, const struct sockaddr_in& rhs ) {
	char	ip[INET6_ADDRSTRLEN];

	inet_ntop(AF_INET, &rhs, ip, sizeof(ip));
	os << rhs.sin_family << " " << ip << ":" << ntohs(rhs.sin_port) << std::endl;
	return (os);
}

std::ostream&	operator<<( std::ostream& os, const Client& rhs ) {
	os << "Client:" << std::endl
		<< "  - addr: " << rhs.getNetId() << std::endl
		<< "  - HOST: " << rhs.getHostname() << std::endl
		<< "  - USER: " << rhs.getUsername() << std::endl
		<< "  - NICK: " << rhs.getNickname() << std::endl;
		// << "  - SOCK: " << (rhs.getFd() == NULL ? 0 : *rhs.getFd()) << std::endl;
	return (os);
}

