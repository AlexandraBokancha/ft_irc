/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */ /*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 19:35:30 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/29 04:15:09 by dbaladro         ###   ########.fr       */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>
# include <cstdio>

# define O 0x01 //!< Give/take channel operator privileges
# define I 0x02 //!< Invite-only channel
# define INVITE_ONLY 0x02 //!< Invite-only channel
# define T 0x04 //!< Topic settable by channel operator only
# define TOPIC 0x04 //!< Topic settable by channel operator only
# define L 0x08 //!< User limit for the channel
# define USER_LIMIT 0x08 //!< User limit for the channel
# define K 0x10//!< Set/remove the channel key (password)

//! Chanel user flag
# define CHNUSR_BIGO	0x01
# define CHNUSR_O		0x02
# define CHNUSR_V		0x04

//! Chanel flag
# define CHN_A	0x0001	//!< Anonymous channel
# define CHN_I	0x0002	//!< Invite-mode
# define CHN_M	0x0004	//!< Moderated
# define CHN_N	0x0008	//!< No messgae from client on the outside
# define CHN_Q	0x0010	//!< Quiet
# define CHN_P	0x0020	//!< Private
# define CHN_S	0x0040	//!< Secret
# define CHN_R	0x0080	//!< reop
# define CHN_T	0x0100	//!< Topic

# define CHN_K	0x0200	//!< Channel password setter
# define CHN_L	0x0400	//!< User limit setter

# define CHN_B		0x0800	//!< Ban mask setter
# define CHN_E		0x1000	//!< Exception mask setter
# define CHN_BIGI	0x2000	//!< Invitation mask setter

class Client;

class Channel {
public:
	Channel( void );
	Channel( Channel const & rhs );
	Channel( Client* creator, const std::string& name );
	~Channel();

	Channel& operator=( Channel const & rhs );

	std::string			getName( void ) const;
	int					getMode( void ) const;
	Client*				getClient( const std::string& nick ) const;
	Client*				getClient( const int client_socket ) const;
	int					getUserMode( const Client* client ) const;
	std::vector<std::string>	getBanMask( void ) const;
	std::vector<std::string>	getExceptionMask( void ) const;
	std::vector<std::string>	getInviteMask( void ) const;
	void				setPassword( const std::string& password );
	void				setLimit( const int limit );
	std::string			modeToString( const int mode ) const;

	void				changeMode( const char sign, const int mode );
	void				changeUserMode( const std::string& nick, const char sign, const int mode );

	bool				isFull( void ) const;
	bool				isEmpty( void ) const;
	bool				validPassword( const std::string& password ) const;

	void				addClient( Client* const& client );
	void				removeClient( int client_sock );
	void				isOperator( const std::string nick );

	void				addMask(const char mode, const std::string& mask);
	void				removeMask(const char mode, const std::string& mask);

	//! FOR TESTING PURPOSE
	void	printChannel( void ) const;

private:
	std::string					_name; //!< Channel name
	std::string					_topic;

	std::vector< std::pair< Client*, int > >	_client; //!< Reference to connected clients to the channel
	// std::vector<Client*>	_client; //!< Reference to connected clients to the channel
	std::vector<Client*>		_operator; //!< Channel operator

	int							_mode; //!< Mode flag
	int							_userLimit; //!< 0: no User limit
	std::string					_password;
	std::vector<std::string>	_banmask; //! Channel banmask
	std::vector<std::string>	_exception; //! Channel exception mask
	std::vector<std::string>	_invite; //! Channel invite mask
};

std::ostream&	operator<<( std::ostream& os, const Channel& rhs );

#endif // !CHANNEL_HPP
