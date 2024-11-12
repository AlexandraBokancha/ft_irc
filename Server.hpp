/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 11:02:35 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/12 21:56:38 by dbaladro         ###   ########.fr       */
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
# include <exception>
# include <vector>
# include <poll.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>

class Server {
public:
	Server( void );
	Server( int port, std::string password);
	Server( Server const & rhs );
	~Server();

	Server& operator=( Server const & rhs );

	// Server exception
	class InvalidArgException : public std::exception {
		public :
			const char *what() const throw();
	};
	class InvalidPortException : public InvalidArgException {
		public :
			virtual const char *what() const throw();
	};

	void	startServer( void );
	void	waitClient( void );

	void	pollPushBack(int fd, short events);

private:
	// static const std::string	_ip;
	const int						_port;
	const std::string				_passwd;
	int								_socket;
	struct sockaddr_in				_socketAddress;
	int								_socketAdress_len;

	std::vector<struct pollfd>		_pollFd;
	unsigned int					_clientNbr;
};

#endif // !SERVER_HPP
