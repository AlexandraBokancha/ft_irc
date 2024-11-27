/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecutor.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 23:20:26 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/27 14:17:47 by alexandra        ###   ########.fr       */
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
#include <string>

//! Anonymous namespace: Everything declared here is only accesible in this file
namespace {
	/**
	 * @brief Handle client password verification during registration
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
		}
		if (msg.getParam().size() >= 1){
			if (serv.comparePassword(msg.getParam()[0]) == 0) {
				client.setConnected();
				success_log("Password confirmed");
			}
			else{
				serv.respond(client.getFd(), ERR_PASSWDMISMATCH);
			}
		}
		else{
			serv.respond(client.getFd(), ERR_NEEDMOREPARAMS, msg.getCommand().c_str());
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
		}
		else
			serv.respond(client.getFd(), ERR_NEEDMOREPARAMS, msg.getCommand().c_str());
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
			//! Remove from channel
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
		serv.respond(client.getFd(), "PONG");
	}

	/* ************************************************************************** */
	/* *                         IRC Operator's commands                        * */
	/* ************************************************************************** */

	/** @brief IRC OPER command
	 * 
		OPER message is used by a normal user to obtain operator privileges.
		The combination of <user> and <password> are required to gain Operator privileges.
			
		If the client sending the OPER command supplies the correct password
		for the given user, the server then informs the rest of the network
		of the new operator by issuing a "MODE +o" for the clients nickname.
		The OPER message is client-server only.
	*/
	void oper( Server & serv, Client & client, Message & msg ){
		if (msg.getParam().size() < 2){
			serv.respond(client.getFd(), ERR_NEEDMOREPARAMS, msg.getCommand().c_str());
			return (war_log("ERR_NEEDMOREPARAMS sent to Client %d", client.getFd()));
		}
		
		std::vector<std::string> tmp = msg.getParam();	
		
		if (tmp[0] != serv.getOpUser()){
			serv.respond(client.getFd(), ERR_NOOPERHOST);
			return(war_log("ERR_NOOPERHOST sent to Client %d", client.getFd()));
		}
		
		if (tmp[1] != serv.getOpPasswd()){
			serv.respond(client.getFd(), ERR_PASSWDMISMATCH);
			return (war_log("ERR_PASSWDMISMATCH sent to Client %d", client.getFd()));
		}
		
		client.setOperator(); // !< this client is now Server operator
		serv.respond(client.getFd(), RPL_YOUREOPER);
		
		success_log("MODE +o %s", client.getNickname().c_str());
	}
	
	/** @brief IRC KILL command
	 * 
	 * <nickname> <comment>
	 * 
	 * normally this command is used if there is a dublicate entry in 
	 * the list of valid nicknames
	 * 
	 * NOTE:
	 * It is recommended that only Operators be allowed to kill other users
	 * with KILL message. In an ideal world not even operators would need
	 * to do this and it would be left to servers to deal with.
	 * 
	 */
	void kill( Server & serv, Client & client, Message & msg){
		if (!client.isOperator()){
			serv.respond(client.getFd(), ERR_NOPRIVILEGES);
			return (war_log("ERR_NOPRIVILEGES sent to Client %d", client.getFd()));
		}
		if (msg.getParam().size() < 2){
			serv.respond(client.getFd(), ERR_NEEDMOREPARAMS, msg.getCommand().c_str());
			return (war_log("ERR_NEEDMOREPARAMS sent to Client %d", client.getFd()));
		}
		std::vector<std::string> tmp = msg.getParam();
		if (serv.findClient(tmp[0]) == NULL){
			serv.respond(client.getFd(), ERR_NOSUCHNICK, tmp[0].c_str());
			return (war_log("ERR_NOSUCHNICK sent to Client %d", client.getFd()));
		}
		
		success_log("KILL %s. Comment: %s", tmp[0].c_str(), tmp[1].c_str());
		
		int index = serv.findClientIndex(tmp[0]);
		serv.disconnectClient(index); //!< this client was deconnected from the server
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
		commandMap.push_back(std::make_pair("time", time)); // irssi l'envoie en minuscule
		commandMap.push_back(std::make_pair("info", info)); // irssi l'envoie en minuscule
		commandMap.push_back(std::make_pair("PING", pong)); // PONG reagit a la cmd PING envoye par le client
		commandMap.push_back(std::make_pair("OPER", oper));
		commandMap.push_back(std::make_pair("kill", kill));

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
