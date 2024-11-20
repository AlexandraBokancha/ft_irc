/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 23:00:49 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/20 09:25:05 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include <string>

/* ************************************************************************** */
/* *                       Constructors && Destructors                      * */
/* ************************************************************************** */
Message::Message( void )
	: _prefix(""), _command(""), _param()
{
	return ;
}

Message::Message( const Message& rhs )
	: _prefix(rhs._prefix), _command(rhs._command), _param(rhs._param)
{
	return ;
}

Message::Message( const char *buf, int& i, const int len ) {
	//! Setting default value in case of error
	this->_prefix = "";
	this->_command = "";
	this->_param = std::list<std::string>();

	this->_init(buf, i, len);
	return ;
}

Message::~Message()
{
	return ;
}

/* ************************************************************************** */
/* *                             Operator Overload                          * */
/* ************************************************************************** */
Message&	Message::operator=( const Message& rhs ) {
	if (this != &rhs) {
		this->_prefix = rhs._prefix;
		this->_command = rhs._command;
		this->_param = rhs._param;
	}
	return (*this);
}

							//! FOR TESTING PURPOSE 

std::ostream&	Message::printParam( std::ostream& os ) const {
	for (std::list<std::string>::const_iterator it = this->_param.begin(); it != this->_param.end(); it++) {
		os << "[ " << *it << " ] ";
	}
	return (os);
}

std::ostream&	operator<<(std::ostream& os, const Message& rhs) {
	os << "Message:" << std::endl
		<< "  - Prefix : " << rhs.getPrefix()
		<< "  - Command: " << rhs.getCommand()
		<< "  - Param  : ";
	rhs.printParam(os);
	os << std::endl;
	return (os);
}

/* ************************************************************************** */
/* *                            Getters and Setters                         * */
/* ************************************************************************** */
std::string	Message::getPrefix( void ) const {
	return (this->_prefix);
}

std::string	Message::getCommand( void ) const {
	return (this->_command);
}

std::list<std::string>	Message::getParam( void ) const {
	return (this->_param);
}

/* ************************************************************************** */
/* *                              Member function                           * */
/* ************************************************************************** */
/**
 * @brief Check if c is a special char according to RFC2812
 *
 * RFC2812: 2000 IRC Protocol
 */
static int	isspecial(const char c) {
	switch (c) {
		case LEFT_CURLY_BRACE :
		case RIGHT_CURLY_BRACE :
		case LEFT_BRACKET :
		case RIGHT_BRACKET :
		case CARET :
		case BACKTICK :
		case BACKSLASH :
		case UNDERSCORE :
		case PIPE :
			return (1);
		default :
			return (0);
	}
}

/**
 * @brief Check if end of message is reached
 *
 * Check if buf[i] currently pointing on CRLF (IRC termination character)
 * Check for various error at this moment
 *
 * @param buf The message buffer
 * @param i The index in the buffer buf
 * @param len The buffer len
 */
int	crlf(const char *buf, int& i, const int len) {
	if (i >= len - 1)
		throw (Message::IncompleteMessageException());
	if (buf[i] == CR && buf[i + 1] == LF)
		return (1);
	return (0);
}

/**
 * @brief Get the <prefix> part of the message
 *
 * @param buf The message buffer
 * @param i The index in buffer
 * @param len The buffer len
 */
void Message::_parsePrefix(const char *buf, int& i, const int len) {
	int	type = -1; //!< 0: nickname | 1: servername | -1: can be both

	i = 1;
	if (isspecial(buf[i])) //<! nickname parsing
		type = NICKNAME_PREFIX;
	else if (std::isdigit(buf[i]))
		type = SERVER_PREFIX;
	else if (std::isalpha(buf[i]))
		type = -1;
	else
		throw (Message::InvalidPrefixException());

	while (i < len && buf[i] != SPACE) {
		if (buf[i] == POINT) { //!< Nickname only character encontered
			if (type == NICKNAME_PREFIX)
				throw (Message::InvalidPrefixException());
			type = SERVER_PREFIX;
		}
		if (isspecial(buf[i])) { //!< Server only char
			if (type == SERVER_PREFIX)
				throw (Message::InvalidPrefixException());
			type = NICKNAME_PREFIX;
		}
		if (!std::isalnum(buf[i] && buf[i] != MINUS))
			throw (Message::InvalidPrefixException());
		i++;
	}
	if (type == NICKNAME_PREFIX && std::isdigit(buf[1]))
		throw (Message::InvalidPrefixException());
	if (type == SERVER_PREFIX && isspecial(buf[1]))
		throw (Message::InvalidPrefixException());
	this->_prefix.assign(buf, buf + i); //!< Return prefix 
}

/**
 * @brief Get the <command> part of the message
 *
 * @param buf The message buffer
 * @param i The index in buffer
 * @param len The buffer len
 */
void	Message::_parseCommand(const char *buf, int& i, const int len) {
	int	cmd_len = 0;
	std::string	sep = " \r"; //!< Defining SPACE and CR as separator

	if (std::isdigit(buf[i])) {
		for (cmd_len = 0; cmd_len < 3; cmd_len++) {
			if (!isdigit(buf[i + cmd_len]))
				throw (Message::InvalidCommandException());
		}
	}
	else if (std::isalpha(buf[i])) {
		for (cmd_len = 0; i + cmd_len < len && sep.find(buf[i + cmd_len]) == std::string::npos; cmd_len++) {
			if (!std::isalpha(buf[i + cmd_len]))
				throw (Message::InvalidCommandException());
		}
	}
	else
		throw (Message::InvalidCommandException());
	this->_command.assign(buf + i, buf + i + cmd_len);
	i += cmd_len;
}

/**
 * @brief Extract one param from the message
 *
 * This function extract only one parameter
 *
 * @param buf The message buffer
 * @param i The index in buffer
 * @param len The buffer len
 */
void	Message::_parseOneParam(const char *buf, int& i, const int len) {
	int	param_len;

	for (param_len = 0; param_len + i < len && buf[i + param_len] != SPACE
			&& buf[i + param_len] != CR; param_len++) {
		if (buf[i + param_len] == LF || buf[i + param_len] == 0)
			throw (Message::InvalidParamException());
	}
	// param_len--;
	this->_param.push_back(std::string().assign(buf + i, buf + i + param_len));
	i += param_len;
	return ;
}

/**
 * @brief Get the <param> part of the message
 *
 * Extract the parameters list fromn message
 *
 * @param buf The message buffer
 * @param i The index in buffer
 * @param len The buffer len
 */
void	Message::_parseParam(const char *buf, int& i, const int len) {
	std::string	param;

	while (i < len && buf[i] != CR) {
		if (buf[i] != SPACE)
			throw (Message::InvalidParamException());
		i++;
		if (buf[i] == COLON) { //!< Trailling param
			this->_parseOneParam(buf, ++i, len);
			return ;
		}
		this->_parseOneParam(buf, i, len); //!< Middle param
		// i++;
	}
}

/**
 * @brief Init the Message at construction
 *
 * Parse the buffer as a Message throwing error when needed
 *
 * @param buf The buffer containg the message to parse
 * @param len The message len
 */
void	Message::_init(const char *buf, int& i, const int len) {
	if (len == 0) //! security but shouldn't happen
		return ;
	
	//! get message prefix
	if (buf[0] == ':') {
		this->_parsePrefix(buf, i, len);
		if (buf[i++] != SPACE)
			throw (Message::InvalidMessageException());
	}

	//! Get command
	this->_parseCommand(buf, i, len);
	if (crlf(buf, i, len)) //!< End of mesage reached
		return ;

	//! Get comand parameters
	this->_parseParam(buf, i, len);
	if (!crlf(buf, i, len))
		throw (Message::InvalidMessageException());
	i += 2; //!< Skip CRLF
	return ;
}

/* ************************************************************************** */
/* *                                 Exceptions                             * */
/* ************************************************************************** */
const char*	Message::InvalidMessageException::what() const throw() {
	return ("Invalid message");
}

const char*	Message::InvalidPrefixException::what() const throw() {
	return ("Invalid prefix");
}

const char*	Message::InvalidNicknamePrefixException::what() const throw() {
	return ("Invalid nickname prefix");
}

const char*	Message::InvalidServerNamePrefixException::what() const throw() {
	return ("Invalid servername prefix");
}

const char*	Message::InvalidCommandException::what() const throw() {
	return ("Invalid command");
}

const char*	Message::InvalidParamException::what() const throw() {
	return ("Invalid parameter");
}

const char*	Message::IncompleteMessageException::what() const throw() {
	return ("Incomplete message: Missing CRLF");
}
