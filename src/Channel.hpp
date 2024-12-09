/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 19:35:30 by dbaladro          #+#    #+#             */
/*   Updated: 2024/12/09 15:09:47 by alexandra        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>
# include <cstdio>
# include <algorithm>

//! Chanel user flag
# define CHNUSR_BIGO	0x01 //!< Channel creator
# define CHNUSR_O		0x02 //!< Channel operator
# define CHNUSR_V		0x04 //!< Voice privilege

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
		Client*		getClient( const int client_socket ) const;
		Client*		getClientbyNick( const std::string& nick ) const;
		const std::vector< std::pair<Client *, int> >&	getClients( void ) const;
		Client*		getClientByUsername( const std::string & user ) const;
		int			getUserMode( const Client* client ) const;
		std::string	getTopic( void ) const;
		
		void		setPassword( const std::string& password );
		void		setLimit( const int limit );
		void		setTopic(const std::string & topic);
		void		setInvited( const std::string & nick );
		
		

		bool		isOperator( const std::string nick );
		bool		isInvited( const std::string & nick ) const;
		bool		isFull( void ) const;
		bool		isEmpty( void ) const;
		
		std::string	modeToString( void ) const;

		void		changeMode( const char sign, const int mode );
		void		changeUserMode( const std::string& nick, const char sign, const int mode );
		bool		validPassword( const std::string& password ) const;

		void		addClient( Client* const& client );
		void		removeClient( int client_sock );

		//! FOR TESTING PURPOSE
		void		printChannel( void ) const;

	private:
		std::vector< std::pair< Client*, int > >	_client; //!< Reference to connected clients to the channel
		std::vector<Client*>						_operator; //!< Channel operator
		std::vector<std::string>					_invited; //! List of invited clients to invite-only channel
		
		std::string									_name; //!< Channel name
		std::string									_topic; //!< Channel topic

		int											_mode; //!< Channel mode flag
		int											_userLimit; //!< 0: no User limit
		std::string									_password;
};

std::ostream&	operator<<( std::ostream& os, const Channel& rhs );

#endif // !CHANNEL_HPP
