/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 08:52:50 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/25 15:51:08 by alexandra        ###   ########.fr       */
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
# include "numericReplies.hpp"
# include "Message.hpp"

class Client {
public:
	Client( void );
	Client( Client const & rhs );
	~Client();

	Client& operator=( Client const & rhs );

	//* Getters and Setters *//
	struct sockaddr_in	getNetId( void ) const;
	std::string			getHostname( void ) const;
	std::string			getUsername( void ) const;
	std::string			getNickname( void ) const;
	std::string			getPassword( void ) const;
	std::string			getRealname( void ) const;
	std::string			getServername( void ) const;
	bool				getRegistred( void ) const;
	bool				getValidPass( void ) const;
	bool				getValidUser( void ) const;
	bool				getValidNick( void ) const;
	int					*getFd( void ) const;
	void				setRegistred( bool status );
	void				setNetId( struct sockaddr_in addr );
	void				setFd( int *fd );
	int					setPassword( const Message & msg, const std::string & serverPasswd );
	int					setNickname( const  Message & msg, const std::vector<Client> & clients );
	int					setUsername( const Message & msg );

private:
	struct sockaddr_in	_netId;		//!< Client addr
	std::string			_password;
	std::string			_hostname;
	std::string			_username;
	std::string			_nickname;
	std::string			_servername;
	std::string			_realname;
	bool				_isRegistred;
	bool				_validPass;
	bool				_validNick;
	bool				_validUser;
	int					*_fd; //!< Pointer to server_poll fd
};

std::ostream&	operator<<( std::ostream& os, const struct sockaddr_in& rhs );
std::ostream&	operator<<( std::ostream& os, const Client& rhs );

#endif // !CLIENT_HPP
