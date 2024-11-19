/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecutor.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 23:20:26 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/19 22:08:52 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file CommandExecutor.cpp
 *
 * Define every server command.
 *
 */

# include "CommandExecutor.hpp"
# include "log.hpp"

//! Anonymous namespace: Everything declared here is only accesible in this file
namespace {

	/**
	 * @brief PASS command gandler
	 */
	void	pass(Server& serv, Client&, Message& msg) {
		// if (msg.getParam().size() == 0) {
		// 	//! NEED MORE PARAM
		// 	// 461: ERR_NEEDMOREPARAMS
		// 	return ;
		// }
		// if (serv->comparePassword(msg.getParam()) != 0) //!< Invalid password
		// 	// SEND ERROR
		// 	// 464: ERR_PASSWDMISMATCH
		// else {
		// 	// VALID PASSWORD
		// 	// NOTHING, continue auth process (wait for NICK, USER message )
		// }
		success_log("In PASS command");
	}

	/**
	 * @brief Init the commandMap
	 *
	 * This function is needed because of cpp98 restriction.
	 * We cannot initalize our vector directly with a list
	 * In cpp11 we would just do :
	 *     std::vector< std::pair< std::string, void(*)(Server&, Client&, Message&) > > commandMap =
	 *     {
	 *	       { "PASS", passFunction },
	 *	       { "NICK", nickfunction },
	 *         ...
	 *     }
	 */
	std::vector< std::pair< std::string, void(*)(Server&, Client&, Message&) > > InitCommandMap( void ) {
		std::vector< std::pair< std::string, void(*)(Server&, Client&, Message&) > > commandMap;
		
		commandMap.push_back(std::make_pair("PASS", pass));

		return (commandMap);
	}

	std::vector< std::pair< std::string, void(*)(Server&, Client&, Message&) > > commandMap = InitCommandMap();
}

namespace CommandExecutor {

	void	execute(Server& serv, Client& client, Message& msg) {
		std::vector< std::pair< std::string, void(*)(Server&, Client&, Message&) > >::const_iterator it;
		for (it = commandMap.begin(); it != commandMap.end(); it++) {
			if (it->first.compare(msg.getCommand()) == 0) {
				it->second(serv, client, msg);
				return ;
			}
		}
		//!< Ignored command
		log("Unknown command: %M", &msg);
	}

}
