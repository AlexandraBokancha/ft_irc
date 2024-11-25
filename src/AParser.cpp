/* ************************************************************************** */
/*                                                                            */ /*                                                        :::      ::::::::   */
/*   AParser.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:05:37 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/22 22:55:04 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AParser.hpp"
#include "log.hpp"

/* ************************************************************************** */
/* *                            Function definition                         * */
/* ************************************************************************** */

							//! CHARACTER ANALYSIS

/**
 * @brief Check if c is a special char according to RFC2812
 *
 * RFC2812: 2000 IRC Protocol
 *
 *  @param c Character to be compared
 *  @return True if c is a special char, else False
 */
bool		AParser::isSpecial(const char& c) {
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
			return (true);
		default :
			return (false);
	}
}

/**
 * @brief Check if c is a shorthost name char according to RFC2812
 *
 * RFC2812: 2000 IRC Protocol
 *
 *  @param c Character to be compared
 *  @return True if c is a valid shorthost char, else False
 */
bool		AParser::isShortHost( const char& c ) {
	return (std::isalnum(c) || c == MINUS);
}

/**
 * @brief Check if char c is a valid char for nickname
 *
 * Used for Message parsing
 *
 * @param c The character to check
 * @return true: c is a valid char in nickname, else false
 *
 *  @param c Character to be compared
 *  @return True if c is a valid nickname char, else False
 */
bool		AParser::isNickname( const char& c ) {
	if (!(std::isalnum(c) || isSpecial(c || c == MINUS)))
		return (false);
	return (true);
}

/**
 * @brief Check if c is a username char according to RFC2812
 *
 * RFC2812: 2000 IRC Protocol
 *
 *  @param c Character to be compared
 *  @return True if c is a valid username char, else False
 */
bool		AParser::isUsername(const char& c) {
	static const std::string	banned = " \r\n@\0";

	return (banned.find(c) == std::string::npos);
}

/**
 * @brief Check if c is a paramm char according to RFC2812 ( nospcrlfcl )
 *
 * RFC2812: 2000 IRC Protocol
 *
 *  @param c Character to be compared
 *  @return True if c is a valid parameter char, else False
 */
bool		AParser::isParam( const char &c ) {
	switch (c) {
		case SPACE :
		case CR :
		case LF :
		case '\0' :
		case ':' :
			return (false);
		default :
			return (true);
	}
}

							//! MESSAGE ANALYSIS

/**
 * @brief Return shortname according to RFC 2812
 *
 * Used to parse messages, return Shortname or throw error
 *
 * @param buf Message buffer
 * @param i Buffer index
 * @param len Buffer len
 * @return The shortname string if found, else throw error
 */
std::string	AParser::getShortHost(const char *buf, int &i, const int len) {
	int	start = i;

	if (buf[start] == MINUS)
		throw (AParser::InvalidPrefixException());
	i++;
	while (i < len && AParser::isShortHost(buf[i]))
		i++;
	if (buf[i - 1] == MINUS)
		throw (AParser::InvalidPrefixException());
	return (std::string(buf).substr(start, i - start));
}

/**
 * @brief Return Hostname according to RFC 2812
 *
 * Used to parse messages, return Shortname or throw error
 *
 * @param buf Message buffer
 * @param i Buffer index
 * @param len Buffer len
 * @return The shortname string if found, else throw error
 */
std::string	AParser::getHostname( const char* buf, int& i, const int len) {
	std::string	hostname;

	while (i < len && AParser::isShortHost(buf[i])) {
		hostname.append(AParser::getShortHost(buf, i, len));
		if (buf[i] == POINT) {
			hostname.append(1, buf[i++]);
			continue ;
		}
	}
	return (hostname);
}

/**
 * @brief Return Servername according to RFC 2812
 *
 * Used to parse messages, return Shortname or throw error
 *
 * @param buf Message buffer
 * @param i Buffer index
 * @param len Buffer len
 * @return The shortname string if found, else throw error
 */
std::string	AParser::getServername( const char* buf, int& i, const int len) {
	return (getHostname(buf, i, len));
}

/**
 * @brief Return Nickname according to RFC 2812
 *
 * Used to parse messages, return Shortname or throw error
 *
 * @param buf Message buffer
 * @param i Buffer index
 * @param len Buffer len
 * @return The shortname string if found, else throw error
 */
std::string	AParser::getNickname(const char *buf, int &i, const int len) {
	int start = i;

	if (!(std::isalpha(buf[start]) || isSpecial(buf[start])))
		throw (AParser::InvalidPrefixException());
	i++;
	while (i < len && AParser::isNickname(buf[i]))
		i++;
	return (std::string(buf).substr(start, i - start));
}

/**
 * @brief Return Username according to RFC 2812
 *
 * Used to parse messages, return Shortname or throw error
 *
 * @param buf Message buffer
 * @param i Buffer index
 * @param len Buffer len
 * @return The shortname string if found, else throw error
 */
std::string	AParser::getUsername( const char* buf, int&i, const int len) {
	int start = i;

	while (i < len && AParser::isUsername(buf[i]))
		i++;
	if (i == start)
		throw (AParser::InvalidPrefixException());
	return (std::string(buf).substr(start, i - start));
}

/**
 * @brief get prefic of type servername
 *
 * used in  message parsing
 *
 * @param buf message buffer
 * @param i position in buffer
 * @param len bufer len
 * @return a string containing the prefix, else an error have been thrown
 */
std::string	AParser::getPrefixServer( const char* buf, int&i, const int len) {
	return (AParser::getServername(buf, i, len));
}

/**
 * @brief get prefic of type ( nickname [ [ !user ] @host ])
 *
 * used in  message parsing
 *
 * @param buf message buffer
 * @param i position in buffer
 * @param len bufer len
 * @return a string containing the prefix, else an error have been thrown
 */
std::string	AParser::getPrefixClient(const char *buf, int& i, const int len) {
	int			start = i; //!< Check if fields are empty
	std::string	prefix = "";

	if (AParser::isNickname(buf[i]))
		prefix = AParser::getNickname(buf, i, len);
	if (i < len && buf[i] == '!')
		prefix.append("!" + AParser::getUsername(buf, ++i, len));
	if (i < len && buf[i] == '@')
		prefix.append("@" + AParser::getHostname(buf, ++i, len));
	if (buf[i] != SPACE)
		throw (AParser::InvalidPrefixException());
	return (prefix);
}

/**
 * @brief get prefic of type nickname!user@host
 *
 * used in  message parsing
 *
 * @param buf message buffer
 * @param i position in buffer
 * @param len bufer len
 * @return a string containing the prefix, else an error have been thrown
 */
std::string	AParser::getPrefix(const char *buf, int &i, const int len) {
	if (buf[i] != ':')
		throw (AParser::InvalidPrefixException()); //!< Invalid prefix

	int	ref;
	i++;
	if (isSpecial(buf[i]))
		return (AParser::getNickname(buf, i, len));
	if (std::isdigit(buf[i]))
		return (AParser::getServername(buf, i, len));
	while (i < len && buf[i] != SPACE) {
		ref = i;
		if (isSpecial(buf[i]) || buf[i] == '!' || buf[i] == '@')
			return (std::string(buf).substr(1, ref - 1) + getPrefixClient(buf, i, len));
		if (buf[i] == POINT)
			return (std::string(buf).substr(1, ref - 1) + getPrefixServer(buf, i, len));
		if (!std::isalnum(buf[i]) && buf[i] != MINUS)
			throw (AParser::InvalidPrefixException());
		i++;
	}
	if (buf[i] != SPACE || i <= 1)
		throw (AParser::InvalidPrefixException());
	return (std::string(buf).substr(1, i));
}

/**
 * @brief get command part of message
 *
 * used in  message parsing
 *
 * @param buf message buffer
 * @param i position in buffer
 * @param len bufer len
 * @return a string containing the command, else an error have been thrown
 */
std::string	AParser::getCommand( const char* buf, int& i, const int len ) {
	int			cmd_len = 0;
	std::string	cmd = "";
	std::string	sep = " \r";

	if (std::isdigit(buf[i])) {
		for (cmd_len = 1; cmd_len < 3; cmd_len++) {
			if (!std::isdigit(buf[i + cmd_len]))
				throw (AParser::InvalidCommandException());
		}
	} else if (std::isalpha(buf[i])) {
		for (cmd_len = 1; i + cmd_len <= len && sep.find(buf[i + cmd_len]) == std::string::npos; cmd_len++) {
			if (!std::isalpha(buf[i + cmd_len]))
				throw (AParser::InvalidCommandException());
		}
	} else
		throw (AParser::InvalidCommandException());
	cmd = std::string(buf).substr(i, cmd_len);
	i += cmd_len;
	return (cmd);
}

/**
 * @brief get a middle param ( According to RFC 2812)
 *
 * used in  message parsing
 *
 * @param buf message buffer
 * @param i position in buffer
 * @param len bufer len
 * @return a string containing the param, else an error have been thrown
 */
std::string	AParser::getMiddleParam( const char* buf, int& i, const int len ) {
	int			param_len;
	std::string	sep = " \r";
	std::string	param;

	for (param_len = 0; param_len + i < len && sep.find(buf[i + param_len]) == std::string::npos; param_len++) {
		if (!AParser::isParam(buf[i + param_len]) && buf[i + param_len] != ':')
			throw (AParser::InvalidParameterException());
	}
	param = std::string(buf).substr(i, param_len);
	i += param_len;
	return (param);
}

/**
 * @brief get a trailing param ( According to RFC 2812)
 *
 * used in  message parsing
 *
 * @param buf message buffer
 * @param i position in buffer
 * @param len bufer len
 * @return a string containing the param, else an error have been thrown
 */
std::string	AParser::getTrailingParam( const char* buf, int& i, const int len ) {
	int			param_len;
	std::string	param;

	for (param_len = 0; param_len + i < len && buf[i + param_len] != CR; param_len++) {
		if (buf[i + param_len] == SPACE || buf[i + param_len] == ':')
			continue ;
		if (!AParser::isParam(buf[i + param_len]))
			throw (AParser::InvalidParameterException());
	}
	param = std::string(buf).substr(i, param_len);
	i += param_len;
	return (param);
}

/**
 * @brief get the param list in a message( According to RFC 2812)
 *
 * used in  message parsing
 *
 * @param buf message buffer
 * @param i position in buffer
 * @param len bufer len
 * @return a vector<string> containing the message parameter param
 */
std::vector<std::string>	AParser::getParam( const char* buf, int& i, const int len ) {
	std::vector<std::string> param;

	while (i < len && buf[i] != CR) {
		if (buf[i] != SPACE)
			throw (AParser::InvalidParameterException());
		i++;
		if (buf[i] == ':')
			param.push_back(AParser::getTrailingParam(buf, ++i, len));
		else
			param.push_back(AParser::getMiddleParam(buf, i, len));
	}
	return (param);
}

							//! MISC

/**
 * @brief Extract channel name from channel list
 *
 * Used in JOIN channel parsing and maybe others...
 *
 * @param arg String containing the list of channel
 * @param pos Position in arg
 * @return String containg channel name with prefix or thrown error
 */
std::string	AParser::getChannelName( const std::string& arg, std::string::const_iterator& pos ) {
	const std::string	banned = " \a\n\r\0";
	int					start = pos - arg.begin();
	int					skip = (std::string("&#!+").find(*pos) == std::string::npos ? 1 : 0);

	for ( ; pos != arg.end() && *pos != ','; pos++) {
		if (banned.find(*pos) != std::string::npos)
			throw (AParser::IllegalChannelNameException()); //!< Invalid channel name
	}
	if (skip)
		return ("");
	return (arg.substr(start, (pos - arg.begin()) - start));
}

/**
 * @brief Extract channel list fronm messgae parameter
 *
 * Used in JOIN channel parsing and maybe others...
 *
 * @param arg String containing the list of channel
 * @return Vector containg every channel
 */
std::vector<std::string>	AParser::getChannelList( const std::string& arg ) {
	std::vector<std::string>	channelList;
	std::string::const_iterator	it = arg.begin();

	while (it != arg.end()) {
		try {
			channelList.push_back(getChannelName(arg, it));
		}
		catch (std::exception& e) {
			war_log(e.what());
		}
		for( ; it != arg.end() && *it == ','; it++) {} //!< Skip coma
	}
	return (channelList);
}

/**
 * @brief Extract key from key list
 *
 * Used in JOIN key parsing and maybe others...
 *
 * @param arg String containing the key list
 * @param pos Position in arg
 * @return String containg key
 */
std::string	AParser::getKey( const std::string& arg, std::string::const_iterator& pos ) {
	const std::string	banned = " \t\v\f\n\r\0";
	int					start = pos - arg.begin();
	int					len;

	for ( ; pos != arg.end() && *pos != ','; pos++) {
		if (banned.find(*pos) != std::string::npos) {
			for ( ; pos != arg.end() && *pos != ','; pos++) {} //!< Skip to next key
			return ("");
		}
	}
	return (arg.substr(start, (pos - arg.begin() - start > 24 ? 24 : pos - arg.begin() - start)));
}

/**
 * @brief Extract key list fronm messgae parameter
 *
 * Used in JOIN key parsing and maybe others...
 *
 * @param arg String containing the key list
 * @return Vector containg every key
 */
std::vector<std::string>	AParser::getKeyList(const std::string &arg) {
	std::vector<std::string>	channelList;
	std::string::const_iterator	it = arg.begin();

	while (it != arg.end()) {
		channelList.push_back(getKey(arg, it));
		for( ; it != arg.end() && *it == ','; it++) {} //!< Skip coma
	}
	return (channelList);
}

/* ************************************************************************** */
/* *                            Exception handler                           * */
/* ************************************************************************** */
const char*	AParser::InvalidPrefixException::what() const throw() {
	return ("Invalid prefix");
}

const char*	AParser::InvalidCommandException::what() const throw() {
	return ("Invalid command");
}

const char*	AParser::InvalidParameterException::what() const throw() {
	return ("Invalid parameter");
}

const char*	AParser::IllegalChannelNameException::what() const throw() {
	return ("Illegal channel name");
}
