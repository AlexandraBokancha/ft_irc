/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AParser.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:05:37 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/23 17:35:16 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef APARSER_HPP
# define APARSER_HPP

# include <exception>
# include <string>
# include <cctype>
# include <vector>
# include <utility>

class AParser {
public:
	virtual ~AParser() = 0;

	//! Exceptions
	class InvalidPrefixException : public std::exception { //!< Invalid message prefix
		public :
			const char *what() const throw();
	};
	class InvalidCommandException : public std::exception { //!< Invalid message command
		public :
			const char *what() const throw();
	};
	class InvalidParameterException : public std::exception { //!< Invalid param command
		public :
			const char *what() const throw();
	};
	class IllegalChannelNameException : public std::exception {
		public:
			const char*	what() const throw();
	};

	virtual void	parse(std::vector<std::string> arg) = 0;

	//! Character analysis
	static bool										isSpecial(const char& c);
	static bool										isShortHost( const char& c );
	static bool										isNickname( const char& c );
	static bool										isUsername(const char& c);
	static bool										isParam( const char& c );

	//! Message Analysis
	static std::string								getShortHost( const char* buf, int& i, const int len);
	static std::string								getHostname( const char* buf, int& i, const int len);
	static std::string								getServername( const char* buf, int& i, const int len);
	static std::string								getNickname( const char* buf, int& i, const int len);
	static std::string								getUsername( const char* buf, int& i, const int len);

	static std::string								getPrefixClient( const char* buf, int& i, const int len);
	static std::string								getPrefixServer( const char* buf, int& i, const int len);
	static std::string								getPrefix( const char* buf, int& i, const int len);

	static std::string								getCommand( const char* buf, int& i, const int len );

	static std::string								getTrailingParam( const char* buf, int& i, const int len );
	static std::string								getMiddleParam( const char* buf, int& i, const int len );
	static std::vector<std::string>					getParam( const char* buf, int& i, const int len );


	//! 
	static std::string					getChannelName( const std::string& arg, std::string::const_iterator& pos );
	static std::vector<std::string>		getChannelList( const std::string& arg );

	static std::string					getKey( const std::string& arg, std::string::const_iterator& pos );
	static std::vector<std::string>		getKeyList( const std::string& arg );


private:

};

#endif // !APARSER_HPP
