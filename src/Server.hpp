/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 11:02:35 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/20 23:16:25 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Colors.hpp"
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

# include "Message.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include "CommandExecutor.hpp"

extern int g_signal;

class Server {
public:
	Server( void );
	Server( int port, std::string password);
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

	int		comparePassword(const std::string& str) const;

	Channel*	findChannel( const std::string& name ) const;

	void	startServer( const char *port_str ); //!< Start the server
	void	runServer( void );
	void	stopServer( void );

private:
	//! Private member
	// static const std::string	_ip;
	const int					_port;
	const std::string			_passwd;
	int							_socket;

	std::vector<Channel>		_channel;

	std::vector<Client>			_client;

	std::vector<struct pollfd>	_pollFd;
	unsigned int				_clientNbr;

	struct addrinfo				*_serverInfo;

	//! Private member function
	void						acceptNewClient( void );
	void						disconnectClient( long unsigned int& index );

	Client&						findClient( int client_sock );

	void						checkEvent( long unsigned int& i );
	void						receiveMsg( long unsigned int& i );
	int							sendMsg(int socket, const char *buf, int len) const;
	void						broadcast(const char *buffer, int len, int fd) const;

};

#endif // !SERVER_HPP
