/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NumericResponse.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 11:30:30 by dbaladro          #+#    #+#             */
/*   Updated: 2024/12/02 21:26:28 by dbaladro         ###   ########.fr       */
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
# define ERR_NEEDMOREPARAMS		"461 %s :Not enough parameters"
# define ERR_ALREADYREGISTRED	"462 %s :You may not reregister."
# define ERR_PASSWDMISMATCH		"464 * :Password incorrect." // avec "*" a la place de nick car pass est 
                                                              // une premiere cmd a executer (logiquement 
                                                              // on peut pas avoir le nick)
# define ERR_NOPRIVILEGES       "481 :Permission Denied- You're not an IRC opertator"
# define ERR_NOSUCHNICK         "401 %s :No such nick/channel"

//! NICK
# define ERR_NONICKNAMEGIVEN	"431 :No nickname given"
# define ERR_ERRONEUSNICKNAME	"432 * %s :Erroneus nickname"
# define ERR_NICKNAMEINUSE		"433 * %s :Nickname is already in use" 

//! JOIN
# define ERR_NOSUCHCHANNEL		"403 %s :No such channel"
# define ERR_TOOMANYCHANNELS	"405 %s :You have joined too many channels"
# define ERR_TOOMANYTARGETS 	"407 %s :%d recipients. %s"
# define ERR_CHANNELISFULL  	"471 %s :Cannot join channel (+l)"
# define ERR_INVITEONLYCHAN 	"473 %s :Cannot join channel (+i)"
# define ERR_BADCHANNELKEY  	"475 %s :Cannot join channel (+k)"

//! PART
# define ERR_NOTONCHANNEL		"442 %s :You're not on that channel"

//! SUCCESS_RESPONSE
# define RPL_WELCOME			"001 %s :Welcome to the Inernet Relay Network, %s!%s@%s"

//! TIME
# define RPL_TIME				"391 %s"

//! INFO
# define RPL_INFO				"371 %s"
# define RPL_ENDOFINFO			"374 : End of /INFO"

//! OPER
# define ERR_NOOPERHOST			"491 :No O-lines for your host"
# define RPL_YOUREOPER			"381 :You are now an IRC operator"

// MODE
# define ERR_UMODEUNKNOWNFLAG   "501 :Unknown MODE flag"
# define ERR_USERSDONTMATCH     "502 :Cannot change mode for other users"
# define RPL_UMODEIS            "221 %s :%s"

# define ERR_NOCHANMODES        "477 %s :Channel doesn't support modes"
# define ERR_USERNOTINCHANNEL   "441 %s %s :They aren't on that channel"
# define ERR_KEYSET             "467 %s :Channel key already set"
# define ERR_CHANOPRIVSNEEDED   "482 %s :You're not channel operator"
# define ERR_UNKNOWNMODE        "472 %s :is unknown mode char to me for %s"


# define RPL_CHANNELMODEIS      "324 %s %s"
# define RPL_UNIQOPIS           "325 %s %s"
# define RPL_BANLIST            "367 %s %v"
# define RPL_ENDOFBANLIST       "368 %s :End of channel ban list"
# define RPL_INVITELIST         "346 %s %s"
# define RPL_ENDOFINVITELIST    "347 %s :End of channel invite list"
# define RPL_EXCEPTLIST         "348 %s %s"
# define RPL_ENDOFEXCEPTLIST    "349 %s :End of channel exception list"

#endif // !NUMERIC_RESPONSE_HPP
