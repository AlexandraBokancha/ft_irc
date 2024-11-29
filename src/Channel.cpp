/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 19:35:50 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/29 19:37:50 by alexandra        ###   ########.fr       */
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
	for (std::vector<Client*>::const_iterator it = _client.begin(); it != _client.end(); it++) {
		std::cout << "  [ " << (*it)->getNickname() << " ] : " << (*it)->getFd() << std::endl;
	}
	std::cout << std::endl;
}

/* ************************************************************************** */
/* *                       Constructors && Destructors                      * */
/* ************************************************************************** */
Channel::Channel( void )
	: _name(""), _topic(""), _client(std::vector<Client*>()), _operator(std::vector<Client*>()),
		_mode(0), _channelMode(0), _userLimit(10), _password("")
{
		return ;
}

Channel::Channel( const Channel& rhs )
{
		*this = rhs;
		return ;
}


Channel::Channel( Client* creator, const std::string& name )
	: _name(name), _topic(""), _client(1, creator), _operator(1, creator),
		_mode(0), _channelMode(0), _userLimit(0), _password("")
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
	this->_topic = "";
	this->_client = (std::vector<Client*>(1, creator));
	this->_operator = (name[0] == '+' ? std::vector<Client*>() : std::vector<Client*>(1, creator));
	this->_mode = 0;
	this->_channelMode = 0;
	this->_userLimit = 0;
	this->_password = "";
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
		this->_channelMode = rhs._channelMode;
		this->_userLimit = rhs._userLimit;
		this->_password = rhs._password;
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

int Channel::getChannelMode( void ) const { 
	return (this->_channelMode);
}

Client*		Channel::getClient( const int client_socket ) const {
	std::vector<Client*>::const_iterator it;

	for (it = this->_client.begin(); it != this->_client.end(); it++) {
		if (client_socket == (*it)->getFd())
			return (*it);
	}
	return (NULL);
}

Client* Channel::getClient( const std::string & nick ) const{
	std::vector<Client*>::const_iterator it;

	for (it = this->_client.begin(); it != this->_client.end(); it++){
		if (nick == (*it)->getNickname())
			return (*it);
	}
	return (NULL);
}

const std::vector<Client *>& Channel::getClients( void ) const{
	return (this->_client);
}


std::string	Channel::getTopic( void ) const {
	return (this->_topic);
}

void Channel::setTopic( const std::string & topic ){
	this->_topic = topic;
}

/* ************************************************************************** */
/* *                        Public member functions                         * */
/* ************************************************************************** */
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
	this->_client.push_back(client);
}

/**
 * @brief Remove client from channel
 *
 * Remove client only if he is in channel, else do nothing
 *
 * @param Client address
 */
void		Channel::removeClient( int client_sock ) {
	std::vector<Client*>::iterator it;

	//! remove operator
	for (it = this->_operator.begin(); it != this->_operator.end(); it++) {
		if ((*it)->getFd() == client_sock) {
			log("removing operator %d on channel %s", client_sock, this->getName().c_str());
			this->_operator.erase(it);
			it--;
			break ;
		}

	}

	//! remove client
	for (it = this->_client.begin(); it != this->_client.end(); it++) {
		if ((*it)->getFd() == client_sock) {
			log("removing client %d on channel %s", client_sock, this->getName().c_str());
			this->_client.erase(it);
			return ;
		}
	}
}
