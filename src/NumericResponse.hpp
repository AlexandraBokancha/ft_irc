/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NumericResponse.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 11:30:30 by dbaladro          #+#    #+#             */
/*   Updated: 2024/12/09 15:47:30 by alexandra        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file NumericResponse.hpp
 *
 * Contain all irc numeric response format and message
 */

#ifndef NUMERIC_RESPONSE_HPP
# define NUMERIC_RESPONSE_HPP

//! GENERAL

//! RPL
# define RPL_WELCOME			"001 %s :Welcome to the Inernet Relay Network, %s!%s@%s"
# define RPL_UMODEIS            "221 %s :%s"
# define RPL_WHOISUSER          "311 %s %s %s * :%s"
# define RPL_CHANNELMODEIS      "324 %s %s %s"
# define RPL_UNIQOPIS           "325 %s %s"
# define RPL_NOTOPIC            "331 %s %s :No topic is set"
# define RPL_TOPIC              "332 %s %s :%s"
# define RPL_INVITING           "341 %s %s"
# define RPL_NAMERPLY           "353 %s = %s :%s"
# define RPL_ENDOFNAMES         "366 %s %s :End of /NAMES list"
# define RPL_INFO				"371 %s %s"
# define RPL_ENDOFINFO			"374 %s :End of /INFO"
# define RPL_YOUREOPER			"381 %s :You are now an IRC operator"
# define RPL_TIME				"391 %s %s %s"

//! ERRORS
# define ERR_NOSUCHNICK         "401 %s :No such nick/channel"
# define ERR_NOSUCHCHANNEL		"403 %s %s :No such channel"
# define ERR_CANNOTSENDTOCHAN   "404 %s %s :Cannot send to channel"
# define ERR_TOOMANYCHANNELS	"405 %s %s :You have joined too many channels"
# define ERR_TOOMANYTARGETS 	"407 %s :%d recipients. %s"
# define ERR_NORECIPIENT        "411 %s :No recipient given (%s)"
# define ERR_NOTEXTTOSEND       "412 %s :No text to send"
# define ERR_NONICKNAMEGIVEN	"431 %s :No nickname given"
# define ERR_ERRONEUSNICKNAME	"432 %s %s :Erroneus nickname"
# define ERR_NICKNAMEINUSE		"433 %s %s :Nickname is already in use" 
# define ERR_USERNOTINCHANNEL   "441 %s %s %s :They aren't on that channel"
# define ERR_NOTONCHANNEL		"442 %s :You're not on that channel"
# define ERR_USERONCHANNEL      "443 %s %s %s :is already on channel"
# define ERR_NEEDMOREPARAMS		"461 %s %s :Not enough parameters"
# define ERR_ALREADYREGISTRED	"462 %s :You may not reregister."
# define ERR_PASSWDMISMATCH		"464 %s :Password incorrect."
# define ERR_KEYSET             "467 %s :Channel key already set"
# define ERR_CHANNELISFULL  	"471 %s %s :Cannot join channel (+l)"
# define ERR_UNKNOWNMODE        "472 %s %c :is unknown mode char to me"
# define ERR_INVITEONLYCHAN 	"473 %s %s :Cannot join channel (+i)"
# define ERR_BADCHANNELKEY  	"475 %s %s :Cannot join channel (+k)"
# define ERR_NOCHANMODES        "477 %s :Channel doesn't support modes"
# define ERR_NOPRIVILEGES       "481 %s :Permission Denied- You're not an IRC opertator"
# define ERR_CHANOPRIVSNEEDED   "482 %s %s :You're not channel operator"
# define ERR_NOOPERHOST			"491 %s :No O-lines for your host"
# define ERR_UMODEUNKNOWNFLAG   "501 %s :Unknown MODE flag"
# define ERR_USERSDONTMATCH     "502 %s :Cannot change mode for other users"

//! MISC
# define CMD_ERROR				"ERROR :%s"

#endif // !NUMERIC_RESPONSE_HPP
