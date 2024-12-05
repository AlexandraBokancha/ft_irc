/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 19:35:30 by dbaladro          #+#    #+#             */
/*   Updated: 2024/12/05 19:49:32 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>
# include <cstdio>

// # define O 0x01 //!< Give/take channel operator privileges
// # define I 0x02 //!< Invite-only channel
// # define INVITE_ONLY 0x02 //!< Invite-only channel
// # define T 0x04 //!< Topic settable by channel operator only
// # define TOPIC 0x04 //!< Topic settable by channel operator only
// # define L 0x08 //!< User limit for the channel
// # define USER_LIMIT 0x08 //!< User limit for the channel
// # define K 0x10//!< Set/remove the channel key (password)

//! Chanel user flag
# define CHNUSR_BIGO	0x01
# define CHNUSR_O		0x02
# define CHNUSR_V		0x04

//! Chanel flag
# define CHN_I	0x01	//!< Invite-mode
# define CHN_T	0x02	//!< Topic

# define CHN_K	0x04	//!< Channel password setter
# define CHN_L	0x08	//!< User limit setter

class Client;

class Channel {
public:
	Channel( void );
	Channel( Channel const & rhs );
	Channel( Client* creator, const std::string& name );
	~Channel();

	Channel& operator=( Channel const & rhs );

	std::string	getName( void ) const;
	int			getMode( void ) const;
	Client*		getClient( const std::string& nick ) const;
	const std::vector< std::pair<Client *, int> >&	getClients( void ) const;
	Client*		getClient( const int client_socket ) const;
	Client*		getClientByUsername( const std::string & user ) const;
	std::string	getTopic( void ) const;
	int			getUserMode( const Client* client ) const;
	void		setPassword( const std::string& password );
	void		setLimit( const int limit );
	std::string	modeToString( void ) const;

	void		changeMode( const char sign, const int mode );
	void		changeUserMode( const std::string& nick, const char sign, const int mode );

	void		setTopic(const std::string & topic);

	void		isOperator( const std::string nick );

	bool		isFull( void ) const;
	bool		isEmpty( void ) const;
	bool		validPassword( const std::string& password ) const;

	void		addClient( Client* const& client );
	void		removeClient( int client_sock );

	//! FOR TESTING PURPOSE
	void		printChannel( void ) const;

private:
	std::string					_name; //!< Channel name
	std::string					_topic;

	std::vector< std::pair< Client*, int > >	_client; //!< Reference to connected clients to the channel
	// std::vector<Client*>	_client; //!< Reference to connected clients to the channel
	std::vector<Client*>		_operator; //!< Channel operator

	int						_mode; //!< Mode flag
	int						_channelMode;
	int						_userLimit; //!< 0: no User limit
	std::string				_password;
};

std::ostream&	operator<<( std::ostream& os, const Channel& rhs );

#endif // !CHANNEL_HPP
