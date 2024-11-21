/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecutor.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 23:20:26 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/20 23:28:02 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file CommandExecutor.cpp
 *
 * Define every server command.
 *
 */

# include "CommandExecutor.hpp"
#include "Channel.hpp"
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

	void	join(Server& serv, Client& client, Message& msg) {
		if (msg.getParam().size() == 0)
			throw (); //!< ERR_NEEDMOREPARAMS

		std::vector<std::string>::const_iterator	channel_it;
		std::vector<std::string>					channel_name;
		std::vector<std::string>					key;
		Channel										ch;


		//<! Get channel list and key list
		channel_name = AParser::getChannelList(msg.getParam()[0]);
		key = (msg.getParam().size > 1 ? AParser::getKeyList(msg.getParam()[1]) : std::vector<std::string>());
		
		for (channel_it = channel_name.begin(); channel_it != channel_name.end(); channel_it++) {
			/**
			 * Check if channel exist
			 *   - if not create it
			 *   - if yes :
			 *     - Check for errors :
			 *     - If no error :
			 *       - Add client to channel
			 *       - Send appropriate response
			 */
			ch = serv.findChannel(*channel_it);
			if ()

		}
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
