/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 09:34:22 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/25 16:52:19 by alexandra        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

void replyToClient(std::string msg, int *fd); // a voir comment on modifie cette fonction

/* ************************************************************************** */
/* *                       Constructors && Destructors                      * */
/* ************************************************************************** */
Client::Client( void ) {
	std::memset(&this->_netId, 0, sizeof(this->_netId));
	this->_password = "";
	this->_hostname = "";
	this->_username = "";
	this->_nickname = "";
	this->_servername = "";
	this->_realname = "";
	this->_isRegistred = false;
	this->_validPass = false;
	this->_validNick = false;
	this->_validUser = false;
	this->_fd = NULL;
	//std::cout << "Default constructor called" << std::endl;
	return ;
}

Client::Client( const Client & rhs )
	: _netId(rhs._netId), _nickname(rhs._nickname), _password(rhs._password),_hostname(rhs._hostname), 
	_username(rhs._username), _fd(rhs._fd), _isRegistred(rhs._isRegistred),
	_realname(rhs._realname), _servername(rhs._servername), _validNick(rhs._validNick), _validPass(rhs._validPass),
	_validUser(rhs._validNick) {
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

std::string Client::getPassword( void ) const {
	return (this->_password);
}

std::string Client::getRealname( void ) const {
	return (this->_realname);
}

std::string Client::getServername( void ) const {
	return (this->_servername);
}

bool Client::getRegistred( void ) const {
	return (this->_isRegistred);
}

bool Client::getValidPass( void ) const {
	return (this->_validPass);
}

bool Client::getValidUser( void ) const {
	return (this->_validUser);
}

bool Client::getValidNick( void ) const {
	return (this->_validNick);
}

int*	Client::getFd( void ) const {
	return (this->_fd);
}

void Client::setRegistred( bool status ){
	this->_isRegistred = status;
}

void	Client::setNetId( struct sockaddr_in netId) {
	this->_netId = netId;
}

void	Client::setFd( int *fd ) {
	this->_fd = fd;
}


int	Client::setUsername( const Message & msg ){
	if (msg.getCommand() == "USER"){ // a enlever ce if apres le merge
		if (this->_isRegistred){
			replyToClient(ERR_ALREADYREGISTRED(this->_nickname), _fd);
			throw std::runtime_error("Client is already registred");
		}
		if (msg.getParam().size() == 4){
			std::list<std::string> tmp = msg.getParam();
			std::list<std::string>::const_iterator it = tmp.begin();
			this->_username = *it++;
			this->_hostname = *it++;
			this->_servername = *it++;
			this->_realname = *it;
			this->_validUser = true;
			return (1);
		}
		else{
			replyToClient(ERR_NEEDMOREPARAMS(msg.getCommand()), _fd);
		}
	}
	return (0);
}

/**
 * @brief Handle client nickname assignment during registration
 *
 * Conditions checked:
 * - The nickname must be unique among connected clients.
 * - The nickname must not exceed 9 characters in length.
 * 
 * @param msg Pointer to the Message object
 * @param clients A vector of connected clients
 */
int	Client::setNickname( const Message & msg, const std::vector<Client> & clients ){
	if (msg.getCommand() == "NICK"){ // a enlever ce "if" apres le merge
		if (msg.getParam().size() == 1 || msg.getParam().size() == 2){
			for (int i = 0; i < clients.size(); ++i){
				if (clients[i].getNickname() == msg.getParam().front()){
					replyToClient(ERR_NICKNAMEINUSE(msg.getParam().front()), _fd);
					throw std::runtime_error("Nick is in use");
				}
			}
			if (msg.getParam().front().length() <= 9){ // est-ce qu'il y a dautres chose a check ? 
				this->_nickname = msg.getParam().front();
				this->_validNick = true;
				success_log("NICK %s enregistred", this->_nickname.c_str());
				return (1);
			}
			else{
				replyToClient(ERR_ERRONEUSNICKNAME(msg.getParam().front()), _fd);
				throw std::runtime_error("Nick rejected");
			}
		}	
		else{
			replyToClient(ERR_NONICKNAMEGIVEN(), _fd);
			throw std::runtime_error("Nick rejected");
		}
	}
	return (0);
}


/**
 * @brief Handle client password verification during registration
 *
 * If the client is already registered, it sends an `ERR_ALREADYREGISTRED` response.
 * If the password matches the server's password, the registration succeeds.
 * Otherwise, it sends an `ERR_PASSWDMISMATCH` response and throws an exception. 
 * If the `PASS` command is missing parameters, an `ERR_NEEDMOREPARAMS` response is sent.
 *
 * @param msg Pointer to the Message object
 * @param serverPasswd The server’s correct password
 */
int	Client::setPassword( const Message & msg, const std::string & serverPasswd){
	if (msg.getCommand() == "PASS"){ // a enlever ce "if" apre le merge
		if (this->_isRegistred){
			replyToClient(ERR_ALREADYREGISTRED(this->_nickname), _fd);
			throw std::runtime_error("Client is already registred");
		}
		if (msg.getParam().size() == 1){
			if (msg.getParam().front() == serverPasswd){
				this->_password = msg.getParam().front();
				this->_validPass = true;
				success_log("Password confirmed");
				return (1);
			}
			else{
				replyToClient(ERR_PASSWDMISMATCH(), _fd);
				throw std::runtime_error("Wrong password");
			}
		}
		else{
			replyToClient(ERR_NEEDMOREPARAMS(msg.getCommand()), _fd);
			throw std::runtime_error("Wrong PASS cmd format");
		}
	}	
	return (0);
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
		this->_password = rhs._password;
		this->_servername = rhs._servername;
		this->_realname = rhs._realname;
		this->_isRegistred = rhs._isRegistred;
		this->_validPass = rhs._validPass;
		this->_validNick = rhs._validNick;
		this->_validUser = rhs._validUser;
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
		<< "  - PASSWORD: " << rhs.getPassword() << std::endl
		<< "  - NICK: " << rhs.getNickname() << std::endl
		<< "  - USER: " << rhs.getUsername() << std::endl
		<< "  - REALNAME: " << rhs.getRealname() << std::endl
		<< "  - SERVERNAME: " << rhs.getServername() << std::endl;
		// << "  - SOCK: " << (rhs.getFd() == NULL ? 0 : *rhs.getFd()) << std::endl;
	return (os);
}

