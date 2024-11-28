/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecutor.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 23:20:26 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/28 15:38:36 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file CommandExecutor.cpp
 *
 * Define every server command.
 *
 */

# include "CommandExecutor.hpp"
#include "AParser.hpp"
# include "Channel.hpp"
# include "NumericResponse.hpp"
# include "Server.hpp"
# include "Client.hpp"

# include "log.hpp"
#include <cstring>
#include <string>

//! Anonymous namespace: Everything declared here is only accesible in this file
namespace {
	/**
	 * @brief Handle client password verification during registration
	 *
	 * If the client is already registered, it sends an `ERR_ALREADYREGISTRED` response.
	 * If the password matches the server's password, the registration succeeds.
	 * Otherwise, it sends an `ERR_PASSWDMISMATCH` response and throws an exception. 
	 * If the `PASS` command is missing parameters, an `ERR_NEEDMOREPARAMS` response is sent.
	 *
	 * @param serv Actual server
	 * @param client Client who send comand
	 * @param msg Msg sent to server from client
	 */
	void	pass(Server& serv, Client& client, Message& msg) {
		if (client.getConnected() == true)
			return ;
		if (client.getRegistred() == true){
			serv.respond(client.getFd(), ERR_ALREADYREGISTRED, client.getNickname().c_str());
			return ;
			// throw std::runtime_error("Client is already registred");
		}
		if (msg.getParam().size() >= 1){
			if (serv.comparePassword(msg.getParam()[0]) == 0) {
				// client.setPassword(const Message &msg, const std::string &serverPasswd);
				// this->_validPass = true;
				client.setConnected();
				success_log("Password confirmed");
				// return ;
			}
			else{
				serv.respond(client.getFd(), ERR_PASSWDMISMATCH);
				// throw std::runtime_error("Wrong password");
			}
		}
		else{
			serv.respond(client.getFd(), ERR_NEEDMOREPARAMS, msg.getCommand().c_str());
			// throw std::runtime_error("Wrong PASS cmd format");
		}
		return ;
	}

	/**
	 * @brief Handle client nickname assignment during registration
	 *
	 * Conditions checked:
	 * - The nickname must be unique among connected clients.
	 * - The nickname must not exceed 9 characters in length.
	 * 
	 * @param serv Actual server
	 * @param client Client who send comand
	 * @param msg Msg sent to server from client
	 */
	void	nick(Server& serv, Client& client, Message& msg) {
		if (msg.getParam().size() >= 1) {
			if (serv.findClient(msg.getParam()[0]) != NULL) {
				serv.respond(client.getFd(), ERR_NICKNAMEINUSE, msg.getParam()[0].c_str());
				return ;
			}
			if (!AParser::isNickname(msg.getParam()[0])) { //! Invalid nickname
				serv.respond(client.getFd(), ERR_ERRONEUSNICKNAME, msg.getParam()[0].c_str());
				return ;
			}
			client.setNickname(msg.getParam()[0]);
			success_log("NICK %s enregistred", client.getNickname().c_str());
			if (!client.getUsername().empty()) {
				client.setRegistred();
				serv.respond(client.getFd(), RPL_WELCOME, client.getNickname().c_str(), client.getNickname().c_str(), client.getUsername().c_str(), client.getHostname().c_str());
			}
		}
		else
			serv.respond(client.getFd(), ERR_NONICKNAMEGIVEN);
		return ;
	}

	/**
	 * @brief Handle client username assignment during registration
	 *
	 * IRC USER CMD
	 * 
	 * @param serv Actual server
	 * @param client Client who send comand
	 * @param msg Msg sent to server from client
	 */
	void	user(Server& serv, Client& client, Message& msg) {
		if (client.getRegistred()){
			serv.respond(client.getFd(), ERR_ALREADYREGISTRED);
			return (war_log("Client is already registered"));
		}
		if (msg.getParam().size() == 4){
			std::vector<std::string> tmp = msg.getParam();
			client.getUsername() = tmp[0];
			client.getHostname() = tmp[1];
			client.getServername() = tmp[2];
			client.getRealname() = tmp[3];
			if (!client.getNickname().empty()) {
				client.setRegistred();
				serv.respond(client.getFd(), RPL_WELCOME, client.getNickname().c_str(), client.getNickname().c_str(), client.getUsername().c_str(), client.getHostname().c_str());
			}
			// client.getValidUser() = true;
		}
		else
			serv.respond(client.getFd(), ERR_NEEDMOREPARAMS, msg.getCommand().c_str());
	}

	/**
	 * @brief Handle MODE command
	 *
	 * IRC MODE cmd
	 * 
	 * @param serv Actual server
	 * @param client Client who send comand
	 * @param msg Msg sent to server from client
	 */
	void	mode(Server& serv, Client& client, Message& msg) {
		if (msg.getParam().size() == 0) { //!< No parameters
			serv.respond(client.getFd(), ERR_NEEDMOREPARAMS, msg.getCommand().c_str());
			return ;
		}
		if (msg.getParam()[0].compare(client.getNickname()) != 0) { //!< Nickname not matching user
			serv.respond(client.getFd(), ERR_USERSDONTMATCH);
			return ;
		}
		if (msg.getParam().size() == 1) { //!< Send User mode
			serv.respond(client.getFd(), RPL_UMODEIS, client.getNickname().c_str(), client.getModeStr().c_str());
			return ;
		}

		int	sign = 0; //<! MODE sign ('-' < 0 | ??? == 0 | '+' > 0)
		int flag = client.getMode();
		std::vector<std::string>::const_iterator it;
		std::vector<std::string>	mode_list = AParser::getModeList(msg.getParam()[1]);
		for (it = mode_list.begin(); it != mode_list.end(); it++) {
			sign = (it->begin()[0] == '+' ? 1 : -1);
			for (std::string::const_iterator s_it = it->begin() + 1; s_it != it->end(); s_it++) {
				switch (*s_it) {
					case 'i' :
						flag = (sign > 0 ? flag | INVISIBLE : flag & ~INVISIBLE);
						break ;
					case 'w' :
						flag = (sign > 0 ? flag | WALLOPS : flag & ~WALLOPS);
						break ;
					case 'r' :
						flag = (sign > 0 ? flag | RESTRICTED_USER : flag);
						break ;
					case 'o' :
						flag = (sign > 0 ? flag : flag & ~OPERRATOR);
						break ;
					case 'O' :
						flag = (sign > 0 ? flag : flag & ~LOCAL_OPERATOR);
						break ;
					default :
						serv.respond(client.getFd(), ERR_UMODEUNKNOWNFLAG);
						return ;
				}
			}
		}
		log("Changed client %d mode", client.getFd());
		client.setMode(flag);
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
	 * @brief IRC PART command
	 *
	 * Perform IRC PART command and respond
	 * Quit the channel specified in parameterd
	 *
	 * @param serv Actual server
	 * @param client Client who send comand
	 * @param msg Msg sent to server from client
	 */
	void	part( Server& serv, Client& client, Message& msg) {
		std::vector<std::string>					channel_name;
		std::vector<std::string>::const_iterator	channel_it;
		Channel										*channel;
		std::string									part_msg;

		if (msg.getParam().size() == 0) { //!< No parameter
			serv.respond(client.getFd(), ERR_NEEDMOREPARAMS, msg.getCommand().c_str());
			return ;
		}

		part_msg = (msg.getParam().size() > 1 ? msg.getParam()[1] : client.getNickname());
		channel_name = AParser::getChannelList(msg.getParam()[0]);
		for (channel_it = channel_name.begin(); channel_it != channel_name.end(); channel_it++) {
			channel = serv.findChannel(*channel_it);
			if (!channel) { //!< Channel not found
				serv.respond(client.getFd(), ERR_NOSUCHCHANNEL, channel_it->c_str());
				continue ;
			}
			if (!channel->getClient(client.getFd())) { //!< Client not in channel
				serv.respond(client.getFd(), ERR_NOTONCHANNEL, channel->getName().c_str());
				continue ;
			}
			//! Remov efrom channel
			channel->removeClient(client.getFd());
			// BROADCAST TO CHANNEL
			if (channel->isEmpty()) //!< Remove channel
				serv.delChannel(*channel);
		}
	}

	/**
	 * @brief IRC TIME command
	 *
	 * Perform IRC TIME command and respond
	 *
	 * @param serv Actual server
	 * @param client Client who send comand
	 * @param msg Msg sent to server from client
	 */
	void	time( Server& serv, Client& client, Message& msg) {
		(void) msg;
		std::time_t now = std::time(NULL);
		std::string timeStr = std::ctime(&now);

		serv.respond(client.getFd(), RPL_TIME, timeStr.c_str());
	}

	/**
	 * @brief IRC INFO command
	 *
	 * Perform IRC INFO command and respond
	 *
	 * @param serv Actual server
	 * @param client Client who send comand
	 * @param msg Msg sent to server from client
	 */
	void	info( Server& serv, Client& client, Message& msg) {
		(void) msg;
		std::vector<std::string> infoMessages;
		
		infoMessages.push_back("Welcome to IRC Server!");
		infoMessages.push_back("Version : ft_irc 1.0.0");
		infoMessages.push_back("Supported commands : JOIN, INFO, PRIVMSG, etc.");
		
		for (std::vector<std::string>::const_iterator it = infoMessages.begin(); it \
				!= infoMessages.end(); ++it) {
			serv.respond(client.getFd(), RPL_INFO, it->c_str());
		}
		serv.respond(client.getFd(), RPL_ENDOFINFO);
	}

	/**
	 * @brief IRC PONG command
	 *
	 * Perform IRC PONG command and respond
	 *
	 * @param serv Actual server
	 * @param client Client who send comand
	 * @param msg Msg sent to server from client
	 */
	void	pong( Server& serv, Client& client, Message& msg) {
		(void) msg;
		serv.respond(client.getFd(), "PONG :localhost");
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
		commandMap.push_back(std::make_pair("NICK", nick));
		commandMap.push_back(std::make_pair("USER", user));
		commandMap.push_back(std::make_pair("JOIN", join));
		commandMap.push_back(std::make_pair("PART", part));
		commandMap.push_back(std::make_pair("MODE", mode));
		commandMap.push_back(std::make_pair("TIME", time));
		commandMap.push_back(std::make_pair("INFO", info));
		commandMap.push_back(std::make_pair("PING", pong));

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
		std::string	cmd = msg.getCommand();

		if (!client.getConnected() && cmd.compare("PASS") != 0) //! No password specified
			return ;
		if (!client.getRegistred()
			&& !(cmd.compare("NICK") == 0 || cmd.compare("USER") == 0 || cmd.compare("PASS") == 0)) //! USER not registered
			return ;
		for (it = commandMap.begin(); it != commandMap.end(); it++) {
			if (it->first.compare(cmd) == 0) {
				it->second(serv, client, msg);
				return ;
			}
		}
		//!< Ignored command
		log("Unknown command: %M", &msg);
	}
}
