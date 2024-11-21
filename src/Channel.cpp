/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 19:35:50 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/20 23:08:46 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include <list>
#include <string>

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
