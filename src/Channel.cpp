/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 19:35:50 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/23 17:36:14 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"

/* ************************************************************************** */
/* *                       Constructors && Destructors                      * */
/* ************************************************************************** */
Channel::Channel( void )
	: _name(""), _topic(""), _client(std::list<Client*>()), _operator(NULL),
		_mode(0), _userLimit(10), _password("")
{
		return ;
}

Channel::Channel( const Channel& rhs )
{
		*this = rhs;
		return ;
}


Channel::Channel( Client* creator, const std::string& name )
	: _name(name), _topic(""), _client(1, creator), _operator(creator),
		_mode(0), _userLimit(0), _password("")
{
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
	}
	return (*this);
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
	return (this->_mode & L && this->_client.size() >= this->_userLimit);
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
