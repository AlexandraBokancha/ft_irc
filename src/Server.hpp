/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 11:02:35 by dbaladro          #+#    #+#             */
/*   Updated: 2024/12/09 15:57:00 by alexandra        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <unistd.h>
# include <sstream>
# include <cstring>
# include <ctime>
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
# include <algorithm>
# include "NumericResponse.hpp"

# define MAX_CHANNEL_PER_CLIENT 10

extern int g_signal;

class Channel;
class Client;
class Message;

class Server {
	public:
		Server( void );
		Server( int port, std::string password );
		Server( Server const & rhs );
		~Server();

		Server& operator=( Server const & rhs );

		//! Getters and Setters
		std::string					getPrefix( void ) const;
		std::string					getOpPasswd( void ) const;
		std::string					getOpUser( void ) const;
		std::string					getPasswd( void ) const;
		int							getPort( void ) const;



		void						pollPushBack( int fd, short events );
		int							comparePassword( const std::string& str ) const;
			
		Client*						findClient( int client_sock );
		Client*						findClient( const std::string& nick );
		int							findClientIndex( const std::string & nick );

		void						respond( const Client* src, const int& client_sock, const char* fmr, ... ) const;

		void						addChannel( Channel& channel );
		void						delChannel( Channel& channel );
		Channel*					findChannel( const std::string& name );
		
		void						disconnectClient( int& index );
		
		void						startServer( const char *port_str ); //!< Start the server
		void						stopServer( void );	
		
		int							sendMsg( int socket, const char *buf, int len ) const;
		void						broadcast( const char *buffer, int len, int fd ) const;
		void 						broadcastToChannel( const Client* src, const std::string& msg, const Channel * ch, int fd ) const;
		
		void						runServer( void );


		//! Server exceptions
		class InvalidArgException : public std::exception {
			public :
				const char *what() const throw();
		};
		class InvalidPortException : public InvalidArgException {
			public :
				const char *what() const throw();
		};

		//! FOR TESTING PURPOSE
		void						printChannel( void ) const;

	private:
		//! Private member
		const int					_port;
		const std::string			_passwd;
		const std::string			_opPasswd; 
		const std::string			_opUser;
		int							_socket;

		std::vector<Channel>		_channel;

		std::vector<Client>		_client;

		std::vector<struct pollfd>	_pollFd;
		
		unsigned int				_clientNbr;

		struct addrinfo				*_serverInfo;
		std::string					_ip;

		//! Private member function
		void						acceptNewClient( void );
		int							sendMsg( int socket, const Message& msg ) const;
		void						receiveMsg( int& i );
		void						checkEvent( int& i );

};

#endif // !SERVER_HPP
