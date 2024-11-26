/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 23:00:44 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/26 09:57:47 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file Message.hpp
 *
 * Define message class as specified in RFC2812 :
 * Messages are splitted in tree part :
 *   1 Prefix     : Who sent the message [ optionnal ]
 *   2 Command    : What should be done. The important part of the message
 *   3 Parameters : Most of the time command arguments
 *
 */
#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include "AParser.hpp"
# include <exception>
# include <cctype>
# include <cstring>
# include <vector>
# include <string>
# include <ostream>

//! Types
# define NICKNAME_PREFIX 0
# define SERVER_PREFIX 1

class Message {
public:
	Message( void );
	Message( Message const & rhs );
	Message( const char *buf, int& i, const int len );
	~Message();

	Message& operator=( Message const & rhs );

	//! Getters and Setters
	std::string					getPrefix( void ) const;
	std::string					getCommand( void ) const;
	std::vector<std::string>	getParam( void ) const;

	void						setPrefix( std::string prefix );
	void						setContent( std::string msg );

	std::string					toString( void ) const;

	//! FOR TESTING PURPOSE
	std::ostream&			printParam( std::ostream& os ) const;



	//! Message Error
	class InvalidMessageException : public std::exception {
		public :
			const char *what() const throw();
	};

private:
	void					_init(const char *buf, int& i, const int len);

	std::string					_prefix;	//!< Optionnal message prefix
	std::string					_command;	//!< Message command
	std::vector<std::string>	_param;		//!< Message paramaters
	
};

std::ostream&	operator<<(std::ostream& os, const Message& rhs);

#endif // !MESSAGE_HPP
