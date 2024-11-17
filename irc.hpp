/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 12:19:34 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/17 12:28:09 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file irc.hpp
 * @brief Irc commad response
 *
 * This file contain irc server respones and code
 */

#ifndef IRC_HPP
# define IRC_HPP

//! Server response upon succesful registration
# define RPL_WELCOME "Welcome to the Internet Relay Network <nick>!<user>@<host>"
# define RPL_YOURHOST "Your host is <servername>, running version <ver>"
# define RPL_CREATED "This server was created <date>"
# define RPL_MYINFO "<servername> <version> <available user modes> <available channel modes>"


#endif // !IRC_HPP
