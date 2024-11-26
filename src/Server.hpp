/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 11:02:35 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/25 19:50:43 by alexandra        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <unistd.h>
# include <sstream>
# include <cstring>
# include <string>
# include <errno.h>
# include <signal.h>
# include <exception>
# include <vector>
# include <poll.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>

# include "Colors.hpp"
# include "Client.hpp"
# include "Message.hpp"
# include "irc.hpp"
# include "Channel.hpp"

extern int g_signal;

class Server {
public:
	Server( void );
	Server( int port, std::string password );
	Server( Server const & rhs );
	~Server();

	Server& operator=( Server const & rhs );

	//! Server exceptions
	class InvalidArgException : public std::exception {
		public :
			const char *what() const throw();
	};
	class InvalidPortException : public InvalidArgException {
		public :
			const char *what() const throw();
	};

	void	pollPushBack( int fd, short events );
	
	void	startServer( const char *port_str ); //!< Start the server
	void	runServer( void );
	void	stopServer( void );	

private:
	//! Private member
	// static const std::string	_ip;
	const int					_port;
	const std::string			_passwd;
	int							_socket;

	std::vector<Client>			_client;
	std::vector<struct pollfd>	_pollFd;
	
	unsigned int				_clientNbr;

	struct addrinfo				*_serverInfo;

	//! Private member function
	void						disconnectClient( long unsigned int & i );
	void						acceptNewClient( void );
	
	void						checkEvent( long unsigned int & i );
	void						receiveMsg( long unsigned int & i );
	int							sendMsg( int socket, const char *buf, int len ) const;
	void						broadcast( const char *buffer, int len, int fd ) const;
	int							authentification( long unsigned int & i, const Message & msg );
	
	void						timeCmd( int fd ) const;
	void						infoCmd( int fd ) const;
	void						pongCmd( int fd ) const;
};

#endif // !SERVER_HPP
