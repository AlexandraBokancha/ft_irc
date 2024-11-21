/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 08:52:50 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/21 19:34:23 by alexandra        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <netinet/in.h>
# include <cstring>
# include <string>
# include <ostream>
# include <iostream>
# include "Message.hpp"
# include "log.hpp"
# include "numericReplies.hpp"
# include <unistd.h>
# include <vector>

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

	int					*getFd( void ) const;
	bool				getRegistred( void ) const;
	void				setRegistred( bool status );

	void	setNetId( struct sockaddr_in addr );
	void	setFd( int *fd );
	int		setNickname( const  Message & obj, std::vector<Client> clients );
	int		setUsername( const Message & obj );
	int		setPassword( const Message & obj, std::string serverPasswd );
	int		authentification(long unsigned int & i, const Message & msg);

private:
	struct sockaddr_in	_netId;		//!< Client addr
	std::string			_password;
	std::string			_hostname;
	std::string			_username;
	std::string			_nickname;
	std::string			_servername;
	std::string			_realname;
	bool				_isRegistred;
	int					*_fd; //!< Pointer to server_poll fd
};

std::ostream&	operator<<( std::ostream& os, const struct sockaddr_in& rhs );
std::ostream&	operator<<( std::ostream& os, const Client& rhs );

#endif // !CLIENT_HPP
