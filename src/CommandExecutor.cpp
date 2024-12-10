/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecutor.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 23:20:26 by dbaladro          #+#    #+#             */
/*   Updated: 2024/12/09 22:28:39 by alexandra        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file CommandExecutor.cpp
 *
 * Define every server command.
 *
 */

# include "CommandExecutor.hpp"
# include "AParser.hpp"
# include "Channel.hpp"
# include "NumericResponse.hpp"
# include "Server.hpp"
# include "Client.hpp"

# include "log.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <ostream>
#include <string>
#include <algorithm>

//! Anonymous namespace: Everything declared here is only accesible in this file
namespace {

	/* ************************************************************************** */
	/* *                         IRC Connection Registration                    * */
	/* ************************************************************************** */
	
	/**
	 * @brief Handle client password verification during registration
	 *
	 * @param serv Actual server
	 * @param client Client who send comand
	 * @param msg Msg sent to server from client
	 */
	void	pass( Server& serv, Client& client, Message& msg ) {
		if (client.getRegistred() == true) {
			serv.respond(NULL, client.getFd(), ERR_ALREADYREGISTRED, client.getNickname().c_str());
			return ;
		}
		if (msg.getParam().size() >= 1) {
			if (serv.comparePassword(msg.getParam()[0]) == 0) {
				client.setConnected();
				success_log("Password confirmed");
			}
			else {
				serv.respond(NULL, client.getFd(), ERR_PASSWDMISMATCH, "*");
				client.setDisconnected();
				return ;
			}
		}
		else {
			serv.respond(NULL, client.getFd(), ERR_NEEDMOREPARAMS, client.getNickname().c_str(), msg.getCommand().c_str());
			return ;
		}
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
	void	nick( Server& serv, Client& client, Message& msg ) {
		if (msg.getParam().size() >= 1) {
			if (serv.findClient(msg.getParam()[0]) != NULL) { //! Nickname is already in used by another client
				serv.respond(NULL, client.getFd(), ERR_NICKNAMEINUSE, client.getNickname().c_str(), msg.getParam()[0].c_str());
				return ;
			}
			if (!AParser::isNickname(msg.getParam()[0])) { //! Invalid nickname
				serv.respond(NULL, client.getFd(), ERR_ERRONEUSNICKNAME, "*", msg.getParam()[0].c_str());
				return ;
			}
			client.setNickname(msg.getParam()[0]);
			success_log("NICK %s enregistred", client.getNickname().c_str());
			if (!client.getUsername().empty()) {
				client.setRegistred();
				serv.respond(NULL, client.getFd(), RPL_WELCOME, client.getNickname().c_str(), client.getNickname().c_str(), \
					client.getUsername().c_str(), client.getHostname().c_str());
			}
		}
		else {
			serv.respond(NULL, client.getFd(), ERR_NONICKNAMEGIVEN, client.getNickname().c_str());
			return ;
		}
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
	void	user( Server& serv, Client& client, Message& msg ) {
		if (client.getRegistred()) {
			serv.respond(NULL, client.getFd(), ERR_ALREADYREGISTRED, client.getNickname().c_str());
			return (war_log("Client is already registered"));
		}
		if (msg.getParam().size() == 4) {
			std::vector<std::string> tmp = msg.getParam();
			client.setUsername(tmp[0]);
			client.setHostname(tmp[1]);
			client.setServername(tmp[2]);
			client.setRealname(tmp[3]);
			success_log("USER %s enregistred", client.getUsername().c_str());
			if (!client.getNickname().empty()) {
				client.setRegistred();
				serv.respond(NULL, client.getFd(), RPL_WELCOME, client.getNickname().c_str(), client.getNickname().c_str(), \
					client.getUsername().c_str(), client.getHostname().c_str());
			}
		}
		else {
			serv.respond(NULL, client.getFd(), ERR_NEEDMOREPARAMS, client.getNickname().c_str(), msg.getCommand().c_str());
			return ;
		}
	}

	/**
	 * @brief Handle user MODE command
	 *
	 * IRC user MODE cmd
	 * 
	 * @param serv Actual server
	 * @param client Client who send comand
	 * @param msg Msg sent to server from client
	 */
	void	mode_user( Server& serv, Client& client, Message& msg ) {
		if (msg.getParam()[0].compare(client.getNickname()) != 0) { //!< Nickname not matching user
			serv.respond(NULL, client.getFd(), ERR_USERSDONTMATCH, client.getNickname().c_str());
			return ;
		}
		if (msg.getParam().size() == 1) { //!< Send User mode
			serv.respond(NULL, client.getFd(), RPL_UMODEIS, client.getNickname().c_str(), client.modeToString().c_str());
			return ;
		}

		int	sign = 0; //<! MODE sign ('-' < 0 | ??? == 0 | '+' > 0)
		int flag = client.getMode();
		std::vector<std::string>					mode_list = AParser::getModeList(msg.getParam()[1]);
		std::vector<std::string>::const_iterator	it;
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
						flag = (sign > 0 ? flag : flag & ~OPERATOR);
						break ;
					case 'O' :
						flag = (sign > 0 ? flag : flag & ~LOCAL_OPERATOR);
						break ;
					default :
						serv.respond(NULL, client.getFd(), ERR_UMODEUNKNOWNFLAG, client.getNickname().c_str());
						return ;
				}
			}
		}
		log("Changed client %d mode to %d", client.getFd(), flag);
		client.setMode(flag);
	}

	/**
	 * @brief Add to mode response
	 *
	 * Add param to MODE response
	 *
	 * @param res resulting response vector
	 * @param sign Flag sign
	 * @param flag Mode flag
	 * @param param Optionnal mode param
	 * @return The updated res vector
	 */
	static std::vector<std::string>	addMode(std::vector<std::string>& result
			, const char sign, const char flag, const std::string& param) {
		std::vector<std::string>::iterator	it = result.begin();

		//! Try to add to existing result
		for (int i = 0; i < 2 && it != result.end(); i++) {
			if (sign == (*it)[0]) {
				it->append(1, flag);
				if (param.length() > 0)
					result.push_back(param);
				if (param.length() == 0 && (flag == 'b' || flag == 'e' || flag == 'I'))
					result.push_back("");
				return (result);
			}
		}
		//! Add new
		if (sign == PLUS || sign == MINUS) { //! First element to contain flag
			result.push_back(std::string(1, sign) + flag);
			if (param.length() > 0)
				result.push_back(param);
		}
		return (result);
	}

	/**
	 * @brief Check if channel mode require a parameter
	 *
	 * Used to check if ERR_NEEDMOREPARAMS error should be returned
	 *
	 * @param mode The channel mode (o, v, a, i...)
	 * @param sign The sign of the mode (+ or -)
	 * @return True if require param (ERR_NEEDMOREPARAMS), else False 
	 */
	static bool	requireParam(const char mode, const char sign) {
		if (mode =='o')
			return (true);
		if (sign == PLUS)
			return (mode == 'k' || mode == 'l');
		return (false);
	}

	static void	mode_channel(Server& serv, Channel* chan, Client& client, Message& msg) {
		std::vector<std::string>::const_iterator	prm_it;
		std::vector<std::string>::const_iterator	mode_prm_it;
		std::vector<std::string>					result;
		std::string									res_prm;
		std::string									response;
		Channel										new_chan = *chan;
		int											sign;

		if (msg.getParam().size() == 1) //!< Channel mode information
			return (serv.respond(NULL, client.getFd(), RPL_CHANNELMODEIS, client.getNickname().c_str(), \
				chan->getName().c_str(), chan->modeToString().c_str()));
		if (!chan->isOperator(client.getNickname())) //!< Not a channel operator
			return (serv.respond(NULL, client.getFd(), ERR_CHANOPRIVSNEEDED, client.getNickname().c_str(), \
				chan->getName().c_str()));
		
		for (prm_it = msg.getParam().begin() + 1; prm_it != msg.getParam().end() && prm_it->length() != 0; prm_it++) {
			if (prm_it->length() == 0) //!< INIFINITE LOOP PROBLEM
				break ;
			sign = (*prm_it)[0];
			mode_prm_it = prm_it + 1;
			if (sign != '+' && sign != '-') //!< Invalid mode cmd
				return (war_log("[MODE] invalid parameter : %s ...Ignore", prm_it->c_str()));
			
			for (std::string::const_iterator mode_it = prm_it->begin() + 1; mode_it != prm_it->end(); mode_it++) {
				res_prm = (mode_prm_it == msg.getParam().end() ? "" : *mode_prm_it);

				if (mode_prm_it == msg.getParam().end() && requireParam(*mode_it, sign))
						return (serv.respond(NULL, client.getFd(), ERR_NEEDMOREPARAMS, client.getNickname().c_str(), "MODE"));
				switch (*mode_it) {
					case 'o' :
						if (!chan->getClientbyNick(*mode_prm_it))
							return (serv.respond(NULL, client.getFd(), ERR_NOSUCHNICK, client.getNickname().c_str(), mode_prm_it->c_str()));
						new_chan.changeUserMode(*mode_prm_it, sign, CHNUSR_O);
						break ;
					case 'k' :
						//! Check ERR_KEYSET ???
						new_chan.setPassword(( sign == PLUS ? *mode_prm_it : ""));
						if (sign == PLUS)
							mode_prm_it = ( mode_prm_it == msg.getParam().end() ? mode_prm_it : mode_prm_it + 1 );
						break ;
					case 'l' :
						if (sign == PLUS) {
							int lim = std::atoi(mode_prm_it->c_str());
							if (lim > 0)
								new_chan.setLimit(lim);
							mode_prm_it = ( mode_prm_it == msg.getParam().end() ? mode_prm_it : mode_prm_it + 1 );
						}
						break ;
					case 'i' :
					case 't' : //!< Special
						break ;
					default :
						return (serv.respond(NULL, client.getFd(), ERR_UNKNOWNMODE, client.getNickname().c_str(), static_cast<int>(*mode_it)));
				}
				if (*mode_it != 'o')
					new_chan.changeMode(sign, *mode_it);
				result = addMode(result, sign, *mode_it, res_prm);
			}
			if (mode_prm_it == msg.getParam().end())
				break ;
			prm_it = mode_prm_it;
		}

		//! Server respond
		*chan = new_chan;
		for (prm_it = result.begin(); prm_it != result.end(); prm_it++) {
			if ((*prm_it)[0] == PLUS || (*prm_it)[0] == MINUS)
				response += *prm_it;
			else
				response += " " + *prm_it;
		}
		serv.respond(NULL, client.getFd(), RPL_CHANNELMODEIS, client.getNickname().c_str(), chan->getName().c_str(), response.c_str());
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
		std::string	channel = "#&!+";

		if (msg.getParam().size() == 0) { //!< No parameters
			serv.respond(NULL, client.getFd(), ERR_NEEDMOREPARAMS, client.getNickname().c_str(), msg.getCommand().c_str());
			return ;
		}
		if (channel.find(msg.getParam()[0][0]) != std::string::npos) {
			Channel*	chan = serv.findChannel(msg.getParam()[0]);
			if (!chan)
				return (serv.respond(NULL, client.getFd(), ERR_NOSUCHNICK, msg.getParam()[0].c_str()));
			return (mode_channel(serv, chan, client, msg));
		}
		return (mode_user(serv, client, msg));
	}


	/* ************************************************************************** */
	/* *                         IRC Channel's commands                        * */
	/* ************************************************************************** */
	

	/* we need to broadcast info about next commands:
		- JOIN + 
		- PART + 
		- KICK +
		- MODE +
		- QUIT -
		- PRIVMSG/NOTICE -
	*/

	/**
	 * @brief IRC JOIN command
	 *
	 * Perform IRC JOIN command and respond
	 *
	 * @param serv Actual server
	 * @param client Client who send comand
	 * @param msg Msg sent to server from client
	 */
	void	join( Server& serv, Client& client, Message& msg ) {

		std::vector<std::string>					channel_name;
		std::vector<std::string>::const_iterator	channel_it;
		std::vector<std::string>					key;
		std::vector<std::string>::const_iterator	key_it;
		Channel										*ch;
		int											channel_mode;
		
		if (msg.getParam().size() == 0) {
			serv.respond(NULL, client.getFd(), ERR_NEEDMOREPARAMS, client.getNickname().c_str(), msg.getCommand().c_str());
			return ;
		}

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
				ch = serv.findChannel(*channel_it);
				// continue; 
			}
			else {
				if (ch->getClient(client.getFd())) //!< Client already in channel
					continue ;

				//!< TOOMANYTARGET ERROR
				if (ch->getName()[0] == '!' && channel_it->c_str()[0] == '!'
						&& std::strcmp(ch->getName().c_str() + 6, channel_it->c_str() + 1) == 0) {
					serv.respond(NULL, client.getFd(), ERR_TOOMANYTARGETS, channel_it->c_str(), 471, "Safe channel already exist");
					continue ;
				}

				channel_mode = ch->getMode();
				if (channel_mode & CHN_I && !ch->isInvited(client.getNickname())) { //!< Invite only channel
					serv.respond(NULL, client.getFd(), ERR_INVITEONLYCHAN, client.getNickname().c_str(), channel_it->c_str());
					continue ;
				}
				if (ch->isFull()) { //!< User limit set and reached
					serv.respond(NULL, client.getFd(), ERR_CHANNELISFULL, client.getNickname().c_str(), channel_it->c_str());
					continue ;
				}
				if (key_it != key.end()) { //!< Invalid key
					if (!ch->validPassword(*key_it)) {
						serv.respond(NULL, client.getFd(), ERR_BADCHANNELKEY, client.getNickname().c_str(), channel_it->c_str());
						continue ;
					}
					key_it++;
				}
				if (client.getJoinedChannel() >= MAX_CHANNEL_PER_CLIENT) { //!< Maximum channel joined
					serv.respond(NULL, client.getFd(), ERR_TOOMANYCHANNELS, client.getNickname().c_str(), channel_it->c_str());
					continue ;
				}

				//! SUCCESS
				//! add client to channel
				ch->addClient(&client);
				client.setJoinedChannel();
			}
			
			//! response to client
			std::string response = std::string(":") + client.getNickname() + std::string("!~") + client.getUsername() + 
				std::string("@") + serv.getPrefix() + std::string(" JOIN ") + ch->getName();
			serv.respond(&client, client.getFd(), response.c_str());	
			
			//! RPL_TOPIC
			if (!ch->getTopic().empty()) { //!< send topic if it's set
				serv.respond(NULL, client.getFd(), RPL_TOPIC, client.getNickname().c_str(), \
					ch->getName().c_str(), ch->getTopic().c_str());
			}

			//!RPL_NAMERPLY
			std::string names = ":";
			const std::vector<std::pair<Client *, int> >& members = ch->getClients();
			for (std::vector<std::pair<Client *, int> >::const_iterator it = members.begin(); it != members.end(); ++ it){
				if (it != members.begin()) {
					names += " ";
				} 
				if (it->second & CHNUSR_BIGO || it->second & CHNUSR_O) {
					names += "@";
				}
				names += it->first->getNickname();
			}
			
			serv.respond(NULL, client.getFd(), RPL_NAMERPLY, client.getNickname().c_str(), ch->getName().c_str(), names.c_str());
			serv.respond(NULL, client.getFd(), RPL_ENDOFNAMES, client.getNickname().c_str(), ch->getName().c_str());
			
			//! broadcast to channel
			serv.broadcastToChannel(&client, response, ch, client.getFd());
			
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
	void	part( Server& serv, Client& client, Message& msg ) {
		Channel										*channel;
		std::vector<std::string>					channel_name;
		std::vector<std::string>::const_iterator	channel_it;
		std::string									part_msg;

		if (msg.getParam().size() == 0) { //!< No parameter
			serv.respond(NULL, client.getFd(), ERR_NEEDMOREPARAMS, client.getNickname().c_str(), msg.getCommand().c_str());
			return ;
		}

		part_msg = (msg.getParam().size() > 1 ? msg.getParam()[1] : client.getNickname());
		channel_name = AParser::getChannelList(msg.getParam()[0]);
		for (channel_it = channel_name.begin(); channel_it != channel_name.end(); channel_it++) {
			channel = serv.findChannel(*channel_it);
			if (!channel) { //!< Channel not found
				serv.respond(NULL, client.getFd(), ERR_NOSUCHNICK, channel_it->c_str());
				continue ;
			}
			if (!channel->getClient(client.getFd())) { //!< Client not in channel
				serv.respond(NULL, client.getFd(), ERR_NOTONCHANNEL, client.getNickname().c_str(), channel->getName().c_str());
				continue ;
			}
			//! Remove from channel
			channel->removeClient(client.getFd());
			
			//! response to client
			std::string response = ":" + client.getNickname() + "!~" + client.getUsername() + "@" + \
				std::string(inet_ntoa((client.getNetId().sin_addr))) + std::string(" PART ") + channel->getName() + " :" + part_msg;
			serv.respond(NULL, client.getFd(), response.c_str());
			
			if (channel->isEmpty()) { //!< Remove channel
				serv.delChannel(*channel);
				return ;
			}
		
			//! broadcast to channel
			serv.broadcastToChannel(NULL, response, channel, client.getFd());	
		}
	}

	/** 
	 * @brief IRC TOPIC command
	 * 
	 * <channel> [<topic>]
	 * 
	 * Message is used to change or view the topic of a channel.
	 * 
	 * BEWARE: to test this function MODE function has to be finished
	 * 
	 */
	void	topic( Server& serv, Client& client, Message& msg ) {
		Channel	*channel;
		
		if (msg.getParam().size() == 0) { //!< No parameter
			serv.respond(NULL, client.getFd(), ERR_NEEDMOREPARAMS, client.getNickname().c_str(), msg.getCommand().c_str());
			return ;
		}
		
		channel = serv.findChannel(msg.getParam()[0]);
		if (!channel){ //!< Channel not found
			serv.respond(NULL, client.getFd(), ERR_NOSUCHCHANNEL, client.getNickname().c_str(), msg.getParam()[0].c_str());
			return ;
		}
		
		if (!channel->getClient(client.getFd())) { //!< Client not on channel									 
			serv.respond(NULL, client.getFd(), ERR_NOTONCHANNEL, client.getNickname().c_str(), channel->getName().c_str());
			return;
		}
		
		if (msg.getParam().size() == 1){ //!< view the topic of the channel if exists
			if (!channel->getTopic().empty()) {
				serv.respond(NULL, client.getFd(), RPL_TOPIC, client.getNickname().c_str(), channel->getName().c_str(), channel->getTopic().c_str());
			}
			else { //!< no topic for this channel
				serv.respond(NULL, client.getFd(), RPL_NOTOPIC, client.getNickname().c_str(), channel->getName().c_str());
			}
		}
		else if (msg.getParam().size() >= 2) { //!< change the topic, if you have the rights
			if ((channel->getMode() & CHN_T) && (!channel->isOperator(client.getNickname()))) { //!< topic settable by channel operator only
				serv.respond(NULL, client.getFd(), ERR_CHANOPRIVSNEEDED, client.getNickname().c_str(), channel->getName().c_str());
				return;
			}
			else { //!< you can change the topic
				channel->setTopic(msg.getParam()[1]);
				
				std::string msg = ":" + client.getNickname() + "!~" + client.getUsername() + "@" + std::string(inet_ntoa((client.getNetId().sin_addr))) + \
					" " + std::string("TOPIC") + " " + channel->getName() + " :" + channel->getTopic();	
			
				//!< response to client
				serv.respond(NULL, client.getFd(), msg.c_str());
			
				//!< broadcast to channel
				serv.broadcastToChannel(NULL, msg, channel, client.getFd());
			}
		}
	}

	/**
	 * @brief IRC INVITE command
	 * 
	 * <nickname> <channel>
	 * 
	 * The parameter <nickname> is the nickname of the person to be invited to
	 * the target channel <channel>. 
	 * 
	 * NOTE: There is no requirement that the channel the target 
	 * user is being invited to must exist or be a valid channel. 
	 * 
	 * To invite a user to a channel which is invite only (MODE +i),
	 * the client sending the invite must be recognised as being a
	 * channel operator on the given channel.
	 * 
	 */
	void	invite( Server& serv, Client& client, Message& msg ) {
		Channel	*channel;
		std::string	response;
		
		if (msg.getParam().size() <= 1) { //!< No parameter
			serv.respond(NULL, client.getFd(), ERR_NEEDMOREPARAMS, client.getNickname().c_str(), msg.getCommand().c_str());
			return ;
		}
		
		std::string nick_target = msg.getParam()[0];
		if (serv.findClient(nick_target) == NULL){ //!< No nick found
			serv.respond(NULL, client.getFd(), ERR_NOSUCHNICK, nick_target.c_str());
			return ;
		}
		
		std::string channel_target = msg.getParam()[1];
		channel = serv.findChannel(channel_target); //!< selon la norme IRC on doit pas verifie l'existence du channel
		
		if (channel) {
			if (!channel->getClient(client.getFd())) { //!< Client trying to sent an invitation not on a channel									 
				serv.respond(NULL, client.getFd(), ERR_NOTONCHANNEL, client.getNickname().c_str(), channel->getName().c_str());
				return ;
			}
			if (channel->getClientbyNick(nick_target)) { //!< Client tries to invite a user to a channel they are already on
				serv.respond(NULL, client.getFd(), ERR_USERONCHANNEL, client.getNickname().c_str(), channel->getClientbyNick(nick_target)->getUsername().c_str(), \
					channel_target.c_str());
				return ;
			}
			if (channel->getMode() & CHN_I) { //! Invite-only channel -> client sending a msg has to be a channel operator
				if (client.getMode() & ~CHNUSR_BIGO && client.getMode() & ~CHNUSR_O){
					serv.respond(NULL, client.getFd() , ERR_CHANOPRIVSNEEDED, client.getNickname().c_str(), channel->getName().c_str());
					return ;
				}
			}
			channel->setInvited(nick_target); // segfault
		}
		
		//!< send invitation
		response = ":" + client.getNickname() + " INVITE " + channel_target + " " + nick_target;
		serv.respond(NULL, serv.findClient(nick_target)->getFd(), response.c_str());
		
		//!< reply by server
		serv.respond(NULL, client.getFd(), RPL_INVITING, channel_target.c_str(), nick_target.c_str());
	}


	/** 
	 * @brief IRC KICK command
	 * 
	 * <channel> <user> [<comment>]
	 * 
	 * ~ forced PART a user from channel
	 * 
	 * Only a channel operator may kick another user out of a channel.
	 * 
	*/
	void	kick( Server& serv, Client& client, Message& msg ) {
		Channel *channel;
		std::string comment = "";
		std::string response;
		
		if (msg.getParam().size() <= 1) { //!< No parameter
			serv.respond(NULL, client.getFd(), ERR_NEEDMOREPARAMS, client.getNickname().c_str(), msg.getCommand().c_str());
			return ;
		}

		channel = serv.findChannel(msg.getParam()[0]);
		if (!channel){ //!< Channel not found
			serv.respond(NULL, client.getFd(), ERR_NOSUCHCHANNEL, client.getNickname().c_str(), msg.getParam()[0].c_str());
			return ;
		}
		
		if (!channel->getClient(client.getFd())){ //!< Client not on channel									 
			serv.respond(NULL, client.getFd(), ERR_NOTONCHANNEL, client.getNickname().c_str(), channel->getName().c_str());
			return;
		}
		if (!channel->isOperator(client.getNickname())){ //!< Client has no privilages
			serv.respond(NULL, client.getFd(), ERR_CHANOPRIVSNEEDED, client.getNickname().c_str(), channel->getName().c_str());
			return ;
		}
		else{

			//!< kick client from channel
			channel->removeClient(channel->getClientbyNick(msg.getParam()[1])->getFd());
			
			if (msg.getParam().size() > 2) {
				comment = " :" + msg.getParam()[2];
			}
			
			response = ":" + client.getNickname() + "!~" + client.getUsername() + "@" + std::string(inet_ntoa((client.getNetId().sin_addr))) + \
				" KICK " + channel->getName() + " " + msg.getParam()[1] + comment;
				
			//!< response to client
			serv.respond(NULL, client.getFd(), response.c_str());
			
			//!< broadcast to channel
			serv.broadcastToChannel(NULL, response, channel, client.getFd());
		}
	}

		/* ************************************************************************** */
		/* *                         IRC Server's commands                          * */
		/* ************************************************************************** */

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

		size_t pos = timeStr.find('\n');
		if (pos != std::string::npos) {
			timeStr.erase(pos, 1); //<! Supprime \n a la find de timeStr
		}
		serv.respond(NULL, client.getFd(), RPL_TIME, client.getNickname().c_str(), serv.getPrefix().c_str(), timeStr.c_str());
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
			serv.respond(NULL, client.getFd(), RPL_INFO, client.getNickname().c_str(), it->c_str());
		}
		serv.respond(NULL, client.getFd(), RPL_ENDOFINFO, client.getNickname().c_str());
	}


	/**
	 * @brief IRC PRIVMSG command
	 * 
	 * <receiver>{,<receiver>} <text to be sent>
	 *
	 */
	void	privmsg( Server& serv, Client& client, Message& msg ) {
		std::vector<std::string>					receiver;
		std::vector<std::string>::const_iterator	receiver_it;
		std::string									text;
		Client										*clientReceiver;
		Channel										*channelReceiver;

		
		if (msg.getParam().size() == 0){ //!< no receiver given
			serv.respond(NULL, client.getFd(), ERR_NORECIPIENT, client.getNickname().c_str(), msg.getCommand().c_str());
			return ;
		}
		if (msg.getParam().size() <= 1){ //!< no text to be sent given
			serv.respond(NULL, client.getFd(), ERR_NOTEXTTOSEND, client.getNickname().c_str());
			return ;
		}
		
		receiver = AParser::getReceiverList(msg.getParam()[0]);
		
		for (receiver_it = receiver.begin(); receiver_it != receiver.end(); receiver_it++){
			if ((*receiver_it)[0] == '#' || (*receiver_it)[0] == '&') { //!< it's a channel
				channelReceiver = serv.findChannel((*receiver_it));
				if (!channelReceiver){ //!< no such channel
					serv.respond(NULL, client.getFd(), ERR_NOSUCHCHANNEL, client.getNickname().c_str(), receiver_it->c_str());
					continue ;
				}
				if (!channelReceiver->getClient(client.getFd())){ // !< client sending a msg not on  channel
					serv.respond(NULL, client.getFd(), ERR_NOTONCHANNEL, channelReceiver->getName().c_str());
					continue ;
				}
				text = ":" + client.getNickname() +" PRIVMSG " + channelReceiver->getName() + " :" + msg.getParam()[1];
				serv.broadcastToChannel(NULL, text, channelReceiver, client.getFd());
			}
			else { //!< it's a client
				clientReceiver = serv.findClient((*receiver_it)); 
				std::cout << "RECEIVER" << *receiver_it << std::endl;
				if (!clientReceiver){ //!< no such client on server
					serv.respond(NULL, client.getFd(), ERR_NOSUCHNICK, (*receiver_it).c_str());
					continue ;
				}
				text = ":" + client.getNickname() + " PRIVMSG " + clientReceiver->getNickname() + " :" + msg.getParam()[1];
				serv.respond(NULL, clientReceiver->getFd(), text.c_str());
			}
		}	
	}
	
	/**
	 * @brief IRC NOTICE Command 
	 * 
	 * <nickname> <text>
	 * 
	 * NOTE:
	 * this command is similiar to PRIVMSG but automatic replies
	 * must never be sent in response to a NOTICE message.
	 * 
	 */
	void	notice( Server& serv, Client& client, Message& msg ) {
		std::string nick_target;
		std::string text_target;

		if (msg.getParam().size() <= 1){
			return ;
		}
		nick_target = msg.getParam()[0];
		if (serv.findClient(nick_target) == NULL){//!< No nick found
			return ;
		}
		//!< send notice to client
		text_target = ":" + client.getNickname() + " NOTICE " + nick_target + " :" + msg.getParam()[1];
		serv.respond(NULL, serv.findClient(nick_target)->getFd(), text_target.c_str());	
	}
	
	/**
	 * @brief Reply to WHOIS command
	 * 
	 */
	void whoisReply( Server& serv, Client& client, Message& msg ){
		(void) msg;
		serv.respond(NULL, client.getFd(), RPL_WHOISUSER, client.getNickname().c_str(), \
			client.getUsername().c_str(), client.getHostname().c_str(), client.getRealname().c_str());
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
		serv.respond(NULL, client.getFd(), "PONG :localhost");
	}

	/** 
	 * @brief IRC QUIT command
	 * 
	 * [<Quit message>] is optional
	 * 
	 */
	void quit( Server& serv, Client& client, Message& msg ) {
		if (!msg.getParam().empty()){
			log("Client %s quit the server with next message: %s", client.getNickname().c_str(), msg.getParam()[0].c_str());
		}
		int index = serv.findClientIndex(client.getNickname());
		index++; // -> disconnectClient() fait -1 de l'index
		success_log("Client %s will be disconnected from the server", client.getNickname().c_str());
		serv.respond(NULL, client.getFd(), CMD_ERROR, "");
		serv.disconnectClient(index);
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
			serv.respond(NULL, client.getFd(), ERR_NEEDMOREPARAMS, client.getNickname().c_str(), msg.getCommand().c_str());
			return (war_log("ERR_NEEDMOREPARAMS sent to Client %d", client.getFd()));
		}
		
		std::vector<std::string> tmp = msg.getParam();	
		
		if (tmp[0] != serv.getOpUser()){
			serv.respond(NULL, client.getFd(), ERR_NOOPERHOST, client.getNickname().c_str());
			return(war_log("ERR_NOOPERHOST sent to Client %d", client.getFd()));
		}
		
		if (tmp[1] != serv.getOpPasswd()){
			serv.respond(NULL, client.getFd(), ERR_PASSWDMISMATCH, client.getNickname().c_str());
			return (war_log("ERR_PASSWDMISMATCH sent to Client %d", client.getFd()));
		}
		
		client.setOperator(); // !< this client is now Server operator
		serv.respond(NULL, client.getFd(), RPL_YOUREOPER, client.getNickname().c_str());
		
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
		if (!client.isServOperator()){
			serv.respond(NULL, client.getFd(), ERR_NOPRIVILEGES, client.getNickname().c_str());
			return (war_log("ERR_NOPRIVILEGES sent to Client %d", client.getFd()));
		}
		if (msg.getParam().size() < 2){
			serv.respond(NULL, client.getFd(), ERR_NEEDMOREPARAMS, client.getNickname().c_str(), msg.getCommand().c_str());
			return (war_log("ERR_NEEDMOREPARAMS sent to Client %d", client.getFd()));
		}
		std::vector<std::string> tmp = msg.getParam();
		if (serv.findClient(tmp[0]) == NULL){
			serv.respond(NULL, client.getFd(), ERR_NOSUCHNICK, tmp[0].c_str());
			return (war_log("ERR_NOSUCHNICK sent to Client %d", client.getFd()));
		}
		
		success_log("KILL %s. Comment: %s", tmp[0].c_str(), tmp[1].c_str());
		
		int index = serv.findClientIndex(tmp[0]);
		index++; // -> disconnectClient() fait -1 de l'index
		serv.disconnectClient(index); //!< this client was deconnected from the server
	}

	/** @brief IRC RESTART command
	 * 
	 * No parameters.
	 * 
	 * The restart message can only be used by an operator
	 * to force a server restart itself.
	 *  
	*/
	void restart( Server & serv, Client & client, Message & msg ){
		(void) msg;
		if (!client.isServOperator()){
			serv.respond(NULL, client.getFd(), ERR_NOPRIVILEGES, client.getNickname().c_str());
			return (war_log("ERR_NOPRIVILEGES sent to Client %d", client.getFd()));
		}
		
		//<! will notify all clients and IRC operators about restarting a server
		war_log("Server will be restarted by %s", client.getNickname().c_str());
		serv.broadcast(":localhost :Server will be restarted\r\n", 39, -1);

		serv.stopServer(); //<! will stop the server and deconnect every client before restart the server
		
		std::stringstream ss;
		ss << serv.getPort();
		
		const char *program = "./ircserv";
		const char *const args[] = { "./ircserv", ss.str().c_str(), serv.getPasswd().c_str(), NULL};

		if (execv(program, const_cast<char *const*>(args)) == -1) { //<! will restart the server with the same port/passwor
			fatal_log("Failed to restart server");
			exit(1);
		}
	}
	
	
	/* ************************************************************************** */
	/* *                         Command Executor Init                          * */
	/* ************************************************************************** */
	
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
		commandMap.push_back(std::make_pair("WHOIS", whoisReply));
		commandMap.push_back(std::make_pair("PART", part));
		commandMap.push_back(std::make_pair("MODE", mode));
		commandMap.push_back(std::make_pair("TOPIC", topic));
		commandMap.push_back(std::make_pair("INVITE", invite));
		commandMap.push_back(std::make_pair("KICK", kick));
		
		commandMap.push_back(std::make_pair("OPER", oper));
		commandMap.push_back(std::make_pair("kill", kill));
		commandMap.push_back(std::make_pair("restart", restart));
		
		commandMap.push_back(std::make_pair("PRIVMSG", privmsg));
		commandMap.push_back(std::make_pair("NOTICE", notice)); 
		commandMap.push_back(std::make_pair("time", time));
		commandMap.push_back(std::make_pair("info", info));
		commandMap.push_back(std::make_pair("PING", pong)); //!< PONG reagit a la cmd PING envoye par le client
		commandMap.push_back(std::make_pair("QUIT", quit));
		commandMap.push_back(std::make_pair("MODE", mode));
		commandMap.push_back(std::make_pair("INFO", info));


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
