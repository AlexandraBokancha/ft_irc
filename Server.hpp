/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 11:02:35 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/13 14:37:29 by dbaladro         ###   ########.fr       */
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
# include <netinet/in.h>
# include <arpa/inet.h>

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
	void	disconnectClient( long unsigned int& index );

	void	startServer( void ); //!< Start the server
	void	runServer( void );
	void	stopServer( void );

private:
	//! Private member
	// static const std::string	_ip;
	const int						_port;
	const std::string				_passwd;
	int								_socket;
	struct sockaddr_in				_socketAddress;
	int								_socketAdress_len;

	std::vector<struct pollfd>		_pollFd;
	unsigned int					_clientNbr;

	//! Private member function
	void							acceptNewClient( void );
	void							receiveMsg( long unsigned int& i );
	void							checkEvent( long unsigned int& i );

};

#endif // !SERVER_HPP
