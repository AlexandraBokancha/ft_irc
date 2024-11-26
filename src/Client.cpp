/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 09:34:22 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/26 11:49:57 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/* ************************************************************************** */
/* *                       Constructors && Destructors                      * */
/* ************************************************************************** */
Client::Client( void ) {
	std::memset(&this->_netId, 0, sizeof(this->_netId));
	this->_hostname = "";
	this->_username = "";
	this->_nickname = "";
	this->_servername = "";
	this->_realname = "";
	this->_connected = false;
	this->_registred = false;
	this->_fd = -1;
	return ;
}

Client::Client( const Client & rhs ) {
	this->_hostname = rhs._hostname;
	this->_username = rhs._username;
	this->_nickname = rhs._nickname;
	this->_servername = rhs._servername;
	this->_realname = rhs._realname;
	this->_connected = rhs._connected;
	this->_registred = rhs._registred;
	this->_fd = rhs._fd;
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

std::string Client::getRealname( void ) const {
	return (this->_realname);
}

std::string Client::getServername( void ) const {
	return (this->_servername);
}

bool Client::getConnected( void ) const {
	return (this->_connected);
}

bool Client::getRegistred( void ) const {
	return (this->_registred);
}

int	Client::getFd( void ) const {
	return (this->_fd);
}

void Client::setConnected( void ){
	this->_connected = true;
}

void Client::setRegistred( void ){
	this->_registred = true;
}

void	Client::setNetId( struct sockaddr_in netId) {
	this->_netId = netId;
}

void	Client::setFd( int fd ) {
	this->_fd = fd;
}

void	Client::setNickname( const std::string& nick ) {
	this->_nickname = nick;
}

int		Client::getJoinedChannel( void ) const {
	return (this->_joinedChannel);
}


/* ************************************************************************** */
/* *                             Operator Overload                          * */
/* ************************************************************************** */
Client&	Client::operator=( Client const& rhs ) {
	if (this != &rhs) {
		this->_netId = rhs._netId;
		this->_hostname = rhs._hostname;
		this->_username = rhs._username;
		this->_nickname = rhs._nickname;
		this->_servername = rhs._servername;
		this->_realname = rhs._realname;
		this->_connected = rhs._connected;
		this->_realname = rhs._registred;
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
		<< "  - NICK: " << rhs.getNickname() << std::endl
		<< "  - USER: " << rhs.getUsername() << std::endl
		<< "  - REALNAME: " << rhs.getRealname() << std::endl
		<< "  - SERVERNAME: " << rhs.getServername() << std::endl;
		// << "  - SOCK: " << (rhs.getFd() == NULL ? 0 : *rhs.getFd()) << std::endl;
	return (os);
}
