/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 09:34:22 by dbaladro          #+#    #+#             */
/*   Updated: 2024/12/11 15:49:28 by alexandra        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Client.hpp"
#include <string>

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
	this->_servOper = false;
	this->_joinedChannel = 0;
	this->_fd = -1;
	this->_mode = 0;
	this->_buffer = "";
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
	this->_servOper = rhs._servOper;
	this->_joinedChannel = rhs._joinedChannel;
	this->_fd = rhs._fd;
	this->_mode = 0;
	this->_buffer = rhs._buffer;
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

std::string Client::getPrefix( void ) const {
	return (this->_nickname + "!" + this->_username + "@" + this->_hostname);
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

int	Client::getJoinedChannel( void ) const {
	return (this->_joinedChannel);
}

int	Client::getMode( void ) const {
	return (this->_mode);
}

const std::string & Client::getBuffer( void ) const{
	return (this->_buffer);
}

void	Client::setHostname( const std::string& hostname ) {
	this->_hostname = hostname;
}

void	Client::setUsername( const std::string& username ) {
	this->_username = username;
}

void	Client::setNickname( const std::string& nick ) {
	this->_nickname = nick;
}

void	Client::setServername( const std::string& servername ) {
	this->_servername = servername;
}

void	Client::setRealname( const std::string& realname ) {
	this->_realname = realname;
}

void	Client::setRegistred( void ) {
	this->_registred = true;
}

void	Client::setConnected( void ) {
	this->_connected = true;
}

void	Client::setDisconnected( void ) {
	this->_connected = false;
}

void	Client::setNetId( struct sockaddr_in netId) {
	this->_netId = netId;
}

void	Client::setFd( int fd ) {
	this->_fd = fd;
}

void	Client::setOperator( void ) {
	this->_servOper = true;
}

void	Client::setBuffer( const char *buffToAdd ) {
	this->_buffer += buffToAdd;
}

void	Client::setMode(const short mode) {
	this->_mode = mode;
}

void	Client::setJoinedChannel( const int i ) {
	this->_joinedChannel = i;
}

void	Client::addJoinedChannel( void ){
	this->_joinedChannel++;	
}

void Client::cleanBuffer( size_t n_pos ) {
	if (n_pos >= this->_buffer.size()){
		this->_buffer.clear();
	}
	else{
		this->_buffer = this->_buffer.substr(n_pos);
	}
}

std::string	Client::modeToString( void ) const {
	std::string	result = "+";
	if (this->_mode & INVISIBLE)
		result += "i";
	if (this->_mode & WALLOPS)
		result += "w";
	if (this->_mode & OPERATOR)
		result += "o";
	if (this->_mode & LOCAL_OPERATOR)
		result += "O";
	if (this->_mode & RESTRICTED_USER)
		result += "r";
	return ((result.length() == 1 ? "" : result));
}

bool	Client::isServOperator( void ) const {
	return (this->_servOper);
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
		this->_registred = rhs._registred;
		this->_servOper = rhs._servOper;
		this->_joinedChannel = rhs._joinedChannel;
		this->_fd = rhs._fd;
		this->_mode = rhs._mode;
		this->_buffer = rhs._buffer;
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
		<< "  - SERVERNAME: " << rhs.getServername() << std::endl
		<< "  - MODE: << " << std::bitset<8>(rhs.getMode()) << std::endl;
		// << "  - SOCK: " << (rhs.getFd() == NULL ? 0 : *rhs.getFd()) << std::endl;
	return (os);
}
