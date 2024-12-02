/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 19:35:30 by dbaladro          #+#    #+#             */
/*   Updated: 2024/12/02 21:10:44 by alexandra        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>

# define O 0x01 //!< Give/take channel operator privileges
# define I 0x02 //!< Invite-only channel
# define INVITE_ONLY 0x02 //!< Invite-only channel
# define T 0x04 //!< Topic settable by channel operator only
# define TOPIC 0x04 //!< Topic settable by channel operator only
# define L 0x08 //!< User limit for the channel
# define USER_LIMIT 0x08 //!< User limit for the channel
# define K 0x10//!< Set/remove the channel key (password)

class Client;

class Channel {
public:
	Channel( void );
	Channel( Channel const & rhs );
	Channel( Client* creator, const std::string& name );
	~Channel();

	Channel& operator=( Channel const & rhs );

	const std::vector<Client *>&	getClients( void ) const;
	std::string						getName( void ) const;
	std::string						getTopic( void ) const;
	int								getMode( void ) const;
	int								getChannelMode( void ) const;
	Client*							getClient( const int client_socket ) const;
	Client*							getClient( const std::string & nick ) const;
	Client*							getClientByUsername( const std::string & user ) const;

	void							setTopic(const std::string & topic);

	bool							isFull( void ) const;
	bool							isEmpty( void ) const;
	bool							validPassword( const std::string& password ) const;

	void							addClient( Client* const& client );
	void							removeClient( int client_sock );

	//! FOR TESTING PURPOSE
	void	printChannel( void ) const;

private:
	std::string				_name; //!< Channel name
	std::string				_topic;

	std::vector<Client*>	_client; //!< Reference to connected clients to the channel
	std::vector<Client*>	_operator; //!< Channel operator

	int						_mode; //!< Mode flag
	//int						_userMode;
	int						_channelMode;
	int						_userLimit; //!< 0: no User limit
	std::string				_password;
};

std::ostream&	operator<<( std::ostream& os, const Channel& rhs );

#endif // !CHANNEL_HPP
