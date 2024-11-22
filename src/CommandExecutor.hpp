/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecutor.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 09:11:20 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/22 00:07:42 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


/**
 * @file CommandExecuto.hpp
 *
 * This file contain only execute definition.
 * Every command are defined in the cpp file.
 * Like this only the execute function has to be used,
 * so you don't have to memorize every every Command
 */

#ifndef COMMANDEXECUTOR_HPP
# define COMMANDEXECUTOR_HPP

# include <utility>
# include <vector>
# include <iterator>
# include "AParser.hpp"

class Server;
class Client;
class Message;

namespace CommandExecutor {
	void	execute(Server& serv, Client& client, Message& msg);

	void	pass(Server& serv, Client& client, Message& msg);
}
#endif // !COMMANDEXECUTOR_HPP
