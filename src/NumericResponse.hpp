/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NumericResponse.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 11:30:30 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/25 21:32:47 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file NumericResponse.hpp
 *
 * Contain all irc numeric response format and message
 */

#ifndef NUMERICRESPONSE_HPP
# define NUMERICRESPONSE_HPP

// PASS

// # define ERR_NEEDMOREPARAMS(command) (std::string(":localhost 461 *") + command + std::string(" :Not enough parameters.\r\n"))
// # define ERR_ALREADYREGISTRED(nick) (":localhost 462 " + nick + " :You may not reregister.\r\n")
// # define ERR_PASSWDMISMATCH() (":localhost 464 * :Password incorrect.\r\n") // avec "*" a la place de nick car pass est 
                                                                            // une premiere cmd a executer (logiquement 
                                                                            // on peut pas avoir le nick)
# define ERR_NOSUCHCHANNEL   "403 %s :No such channel"
# define ERR_TOOMANYCHANNELS "405 %s :You have joined too many channels"
# define ERR_TOOMANYTARGETS  "407 %s :%d recipients. %s"
# define ERR_NEEDMOREPARAMS  "461 %s :Not enough parameters"
# define ERR_CHANNELISFULL   "471 %s :Cannot join channel (+l)"
# define ERR_INVITEONLYCHAN  "473 %s :Cannot join channel (+i)"
# define ERR_BADCHANNELKEY   "475 %s :Cannot join channel (+k)"

//! PART
# define ERR_NOTONCHANNEL "442 %s :You're not on that channel"

// NICK
# define ERR_NONICKNAMEGIVEN() (":localhost 431 :No nickname given\r\n")
# define ERR_NICKNAMEINUSE(nick) (std::string(":localhost 433 * ") + nick + std::string(" :Nickname is already in use\r\n")) 
# define ERR_ERRONEUSNICKNAME(nick) (std::string(":localhost 432 * ") + nick + std::string(" :Erroneus nickname"))

#endif // !NUMERICRESPONSE_HPP
