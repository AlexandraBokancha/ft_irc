/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 08:52:50 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/26 16:08:11 by alexandra        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <netinet/in.h>
# include <cstring>
# include <string>
# include <ostream>
# include <iostream>
# include <unistd.h>
# include <vector>

# include "log.hpp"
# include "Message.hpp"

#include <arpa/inet.h>		//!< Just for struct sockaddr_in operator<<
#include <sys/socket.h>		//!< Just for struct sockaddr_in operator<<

class Client {
public:
	Client( void );
	Client( Client const & rhs );
	~Client();

	Client& operator=( Client const & rhs );

	//* Getters and Setters *//
	struct sockaddr_in	getNetId( void ) const;
	// std::string			getPassword( void ) const;
	std::string			getHostname( void ) const;
	std::string			getUsername( void ) const;
	std::string			getNickname( void ) const;
	std::string			getServername( void ) const;
	std::string			getRealname( void ) const;
	bool				getConnected( void ) const;
	bool				getRegistred( void ) const;
	int					getFd( void ) const;
	int					getJoinedChannel( void ) const;

	void				setRegistred( void );
	void				setConnected( void );
	void				setNetId( struct sockaddr_in addr );
	void				setFd( int fd );
	void				setNickname( const std::string& s );
	void				setOperator( void );
	bool				isOperator( void ) const;

private:
	struct sockaddr_in	_netId;		//!< Client addr
	std::string			_hostname;
	std::string			_username;
	std::string			_nickname;
	std::string			_servername;
	std::string			_realname;

	bool				_connected; //! Entered right PASS
	bool				_registred; //! User fully registered
	
	int					_fd; //!< Pointer to server_poll fd
	int					_joinedChannel; //!< Number of joinend channel
	bool				_isOperator;

};

std::ostream&	operator<<( std::ostream& os, const struct sockaddr_in& rhs );
std::ostream&	operator<<( std::ostream& os, const Client& rhs );

#endif // !CLIENT_HPP
