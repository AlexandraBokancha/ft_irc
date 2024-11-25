/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecutor.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 23:20:26 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/25 01:49:26 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file CommandExecutor.cpp
 *
 * Define every server command.
 *
 */

# include "CommandExecutor.hpp"
# include "Channel.hpp"
# include "NumericResponse.hpp"
# include "Server.hpp"
# include "Client.hpp"

# include "log.hpp"
#include <cstring>

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
	 * @brief IRC JOIN command
	 *
	 * Perform IRC JOIN command and respond
	 *
	 * @param serv Actual server
	 * @param client Client who send comand
	 * @param msg Msg sent to server from client
	 */
	void	join(Server& serv, Client& client, Message& msg) {
		if (msg.getParam().size() == 0) {
			serv.respond(client.getFd(), ERR_NEEDMOREPARAMS, msg.getCommand().c_str());
			war_log("sent ERR_NEEDMOREPARAMS to Client %d", client.getFd());
			return ;
		}

		std::vector<std::string>					channel_name;
		std::vector<std::string>::const_iterator	channel_it;
		std::vector<std::string>					key;
		std::vector<std::string>::const_iterator	key_it;
		Channel										*ch;
		int											channel_mode;


		//<! Get channel list and key list
		channel_name = AParser::getChannelList(msg.getParam()[0]);
		key = (msg.getParam().size() > 1 ? AParser::getKeyList(msg.getParam()[1]) : std::vector<std::string>());
		key_it = key.begin();
		
		for (channel_it = channel_name.begin(); channel_it != channel_name.end(); channel_it++) {
			if (channel_it->size() <= 1) //!< Invalid channel name
				continue ;

			//! Error Checking
			ch = serv.findChannel(*channel_it);
			if (ch == NULL) { //! Create new channel
				Channel	new_channel(&client, *channel_it);
				serv.addChannel(new_channel);
				serv.respond(client.getFd(), "JOIN %s", channel_it->c_str());
				serv.printChannel();
				continue; ;
			}

			if (ch->getClient(client.getFd())) //!< Client already in channel
				continue ;

			//!< TOOMANYTARGET ERROR
			if (ch->getName()[0] == '!' && channel_it->c_str()[0] == '!'
					&& std::strcmp(ch->getName().c_str() + 6, channel_it->c_str() + 1) == 0) {
				serv.respond(client.getFd(), ERR_TOOMANYTARGETS, channel_it->c_str(), 471, "Safe channel already exist");
				continue ;
			}

			channel_mode = ch->getMode();
			if (channel_mode & INVITE_ONLY) { //!< Invite only channel
				serv.respond(client.getFd(), ERR_INVITEONLYCHAN, channel_it->c_str());
				continue ;
			}
			if (ch->isFull()) { //!< User limit set and reached
				serv.respond(client.getFd(), ERR_CHANNELISFULL, channel_it->c_str());
				continue ;
			}
			if (key_it != key.end()) { //!< Invalid key
				if (!ch->validPassword(*key_it)) {
					serv.respond(client.getFd(), ERR_BADCHANNELKEY, channel_it->c_str());
					continue ;
				}
				key_it++;
			}
			if (client.getJoinedChannel() >= MAX_CHANNEL_PER_CLIENT) { //!< Maximum channel joined
				serv.respond(client.getFd(), ERR_TOOMANYCHANNELS, channel_it->c_str());
				continue ;
			}

			//! SUCCESS
			//! add client to server
			ch->addClient(&client);
			serv.respond(client.getFd(), "JOIN %s", channel_it->c_str());

			return ;
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
		commandMap.push_back(std::make_pair("JOIN", join));

		return (commandMap);
	}

	std::vector< std::pair< std::string, void(*)(Server&, Client&, Message&) > > commandMap = InitCommandMap();
}

namespace CommandExecutor {

	/**
	 * @brief Server command executor
	 *
	 * This function is used to perform every command (PASS, NICK, JOIN...)
	 * For simplicity and clarity it is the only one accesible from namespace CommandExecutor
	 *
	 * @param serv Actual server
	 * @param client Client who send comand
	 * @param msg Msg sent to server from client
	 */
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
