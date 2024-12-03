/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 19:35:50 by dbaladro          #+#    #+#             */
/*   Updated: 2024/12/03 12:30:34 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"

//! FOR TESTING PURPOSE
# include "log.hpp"
# include <iostream>
#include <ostream>
#include <string>
void	Channel::printChannel( void ) const {
	std::cout << "[ " << this->_name << " ]" << std::endl;
	for (std::vector<Client*>::const_iterator it = _operator.begin(); it != _operator.end(); it++) {
		std::cout << "  [ @" << (*it)->getNickname() << " ] : " << (*it)->getFd() << std::endl;
	}
	for (std::vector<std::pair<Client*, int > >::const_iterator it = _client.begin(); it != _client.end(); it++) {
		std::cout << "  [ " << it->first->getNickname() << " ] : " << it->first->getFd() << std::endl;
	}
	std::cout << std::endl;
}

/* ************************************************************************** */
/* *                       Constructors && Destructors                      * */
/* ************************************************************************** */
Channel::Channel( void )
	: _name(""), _topic(""), _client(std::vector<std::pair<Client*, int> >()),
		_operator(std::vector<Client*>()), _mode(0), _userLimit(10), _password(""),
		_banmask()
{
		return ;
}

Channel::Channel( const Channel& rhs )
{
		*this = rhs;
		return ;
}


Channel::Channel( Client* creator, const std::string& name )
	: _name(name), _topic(""), _client(), _operator(),
		_mode(0), _userLimit(0), _password(""), _banmask()
{
	this->_name = name;
	if (name[0] == '!') {
		time_t now = time(NULL);
		std::string	result = "";

		for (int i = 0;i < 5; i++) {
			result += "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"[now & 36];
			now /= 36;
		}
		this->_name = ("!" + std::string(result.rbegin(), result.rend()) + name.substr(1, name.size()));
	}
	this->_operator = (name[0] == '+' ? std::vector<Client*>() : std::vector<Client*>(1, creator));
	return ;
}

Channel::~Channel()
{
	return ;
}

/* ************************************************************************** */
/* *                            operator overload                           * */
/* ************************************************************************** */
Channel&	Channel::operator=( const Channel& rhs ) {
	if (this != &rhs) {
		this->_name = rhs._name;
		this->_topic = rhs._topic;
		this->_client = rhs._client;
		this->_operator = rhs._operator;
		this->_mode = rhs._mode;
		this->_userLimit = rhs._userLimit;
		this->_password = rhs._password;
		this->_banmask = rhs._banmask;
	}
	return (*this);
}

std::ostream&	operator<<( std::ostream& os, const Channel& rhs ) {
	rhs.printChannel();
	return (os);
}

/* ************************************************************************** */
/* *                            Getters and Setters                         * */
/* ************************************************************************** */
std::string	Channel::getName( void ) const {
	return (this->_name);
}

int			Channel::getMode( void ) const {
	return (this->_mode);
}

std::string	Channel::modeToString( const int mode  ) const {
	std::string	result = "+";
	char		buffer[100];

	if (mode & CHN_I)
		result.append("i");
	if (mode & CHN_T)
		result.append("t");
	if (mode & CHN_L)
		result.append("l");
	if (mode & CHN_K)
		result.append("k");
	if (mode & CHN_L) {
		sprintf(buffer, "%d", this->_userLimit);
		result.append(" " + std::string(buffer));
	}
	if (mode & CHN_K) {
		sprintf(buffer, "%s", this->_password.c_str());
		result.append(" " + std::string(buffer));
	}
	return ((result.length() == 0 ? "" : result));
}

Client*		Channel::getClient( const int client_socket ) const {
	std::vector<std::pair<Client*, int > >::const_iterator it;

	for (it = this->_client.begin(); it != this->_client.end(); it++) {
		if (client_socket == it->first->getFd())
			return (it->first);
	}
	return (NULL);
}

Client*		Channel::getClient( const std::string& nick ) const {
	std::vector<std::pair<Client*, int > >::const_iterator it;

	for (it = this->_client.begin(); it != this->_client.end(); it++) {
		if (nick == it->first->getNickname())
			return (it->first);
	}
	return (NULL);
}

int			Channel::getUserMode( const Client* client ) const {
	std::vector <std::pair <Client*, int> >::const_iterator it;

	for (it = this->_client.begin(); it != this->_client.end(); it++) {
		if (client == it->first)
			return (it->second);
	}
	return (-1);
}

std::vector<std::string>	Channel::getBanMask( void ) const {
	return (this->_banmask);
}

std::vector<std::string>	Channel::getExceptionMask( void ) const {
	return (this->_exception);
}

std::vector<std::string>	Channel::getInviteMask( void ) const {
	return (this->_invite);
}

void		Channel::setPassword( const std::string& password ) {
	this->_password = password;
}

void		Channel::setLimit( const int limit ) {
	this->_userLimit = limit;
}
/* ************************************************************************** */
/* *                        Public member functions                         * */
/* ************************************************************************** */

/**
 * @brief Add or remove channel mode
 *
 * Add or remove one channel mode
 *
 * @param sign Mode sign ('+' or '-')
 * @param mode Channel mode to modify
 */
void		Channel::changeMode( const char sign, const int mode ) {
	if (sign == PLUS)
		this->_mode &= mode;
	if (sign == MINUS)
		this->_mode |= ~mode;
}

/**
 * @brief Add or remove channel user mode
 *
 * Add or remove channel user mode.
 * If user was fouund
 *
 * @param nick Client nick
 * @param sign Mode sign ('+' or '-')
 * @param mode Channel mode to modify
 */
void		Channel::changeUserMode( const std::string& nick,  const char sign, const int mode ) {
	std::vector< std::pair< Client*, int > >::iterator it;

	for (it = _client.begin(); it != _client.end(); it++) {
		if (nick == it->first->getNickname())
			break ;
	}
	if (it == _client.end())
		return ;
	if (sign == PLUS)
		it->second &= mode;
	if (sign == MINUS)
		it->second |= ~mode;
	return ;
}

/**
 * @brief Check if channel is full when trying to join it
 *
 * Check if the channel can accept a new user
 *
 * @return True when yes, False when channel has a limit that has been reached
 */
bool		Channel::isFull( void ) const {
	return (this->_mode & L && static_cast<int>(this->_client.size()) >= this->_userLimit);
}

/**
 * @brief Check if channel is empty
 *
 * Check if channel has to be removed
 *
 * @return True when channel empty, else false
 */
bool		Channel::isEmpty( void ) const {
	return (this->_client.size() == 0);
}

/**
 * @brief Check if the inputed password is the right one
 *
 * No need to get the password outside of a join tentative
 *
 * @param password The client inputed password
 * @return True when password is valid, else false
 */
bool		Channel::validPassword( const std::string& password ) const {
	if (this->_password.size() == 0) //!< No password set
		return (true);
	if (this->_password.compare(password) == 0)
		return (true);
	return (false);
}

/**
 * @brief Add new client to server
 *
 * Used on JOIN command
 *
 * @param Client pointer to add
 */
void		Channel::addClient( Client* const& client ) {
	this->_client.push_back(std::pair<Client*, int>(client, 0));
}

/**
 * @brief Remove client from channel
 *
 * Remove client only if he is in channel, else do nothing
 *
 * @param Client address
 */
void		Channel::removeClient( int client_sock ) {
	//! remove operator
	for (std::vector<Client*>::iterator it = this->_operator.begin(); it != this->_operator.end(); it++) {
		if ((*it)->getFd() == client_sock) {
			log("removing operator %d on channel %s", client_sock, this->getName().c_str());
			this->_operator.erase(it);
			it--;
			break ;
		}

	}

	//! remove client
	for (std::vector<std::pair<Client*, int> >::iterator it = this->_client.begin(); it != this->_client.end(); it++) {
		if (it->first->getFd() == client_sock) {
			log("removing client %d on channel %s", client_sock, this->getName().c_str());
			this->_client.erase(it);
			return ;
		}
	}
}

