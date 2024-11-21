/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AParser.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:05:37 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/20 22:19:55 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AParser.hpp"
#include "log.hpp"
#include <string>

/* ************************************************************************** */
/* *                            Function definition                         * */
/* ************************************************************************** */
std::string	AParser::getChannelName( const std::string& arg, std::string::const_iterator& pos ) {
	const std::string	banned = " \a\n\r\0";

	for (pos; pos != arg.end() && *pos != ','; pos++) {
		if (banned.find(*pos))
			throw (AParser::IllegalChannelNameException()); //!< Invalid channel name
	}
	return (arg.substr(pos - arg.begin()));
}

std::vector<std::string>	AParser::getChannelList( const std::string& arg ) {
	std::vector<std::string>	channelList;
	std::string::const_iterator	it;	

	for (it = arg.begin(); it != arg.end(); it++) {
		try {
			channelList.push_back(getChannelName(arg, it));
		}
		catch (std::exception& e) {
			war_log(e.what());
		}
		for(it; it != arg.end() && *it == ','; it++) {} //!< Skip coma
	}
	return (channelList);
}

std::string	AParser::getKey( const std::string& arg, std::string::const_iterator& pos ) {
	const std::string	banned = " \t\v\f\n\r\0";
	int					len;

	for (pos; pos != arg.end() && *pos != ','; pos++) {
		if (banned.find(*pos)) {
			for (pos; pos != arg.end() && *pos != ','; pos++) {} //!< Skip to next key
			return ("");
		}
	}
	return (arg.substr((pos - arg.begin() > 24 ? 24 : pos - arg.begin())));
}

std::vector<std::string>	AParser::getKeyList(const std::string &arg) {
	std::vector<std::string>	channelList;
	std::string::const_iterator	it;	

	for (it = arg.begin(); it != arg.end(); it++) {
		channelList.push_back(getKey(arg, it));
		for(it; it != arg.end() && *it == ','; it++) {} //!< Skip coma
	}
	return (channelList);

}

/* ************************************************************************** */
/* *                            Exception handler                           * */
/* ************************************************************************** */
const char*	AParser::IllegalChannelNameException::what() const throw() {
	return ("Illegal channel name");
}

