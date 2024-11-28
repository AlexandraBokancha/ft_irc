/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AParser.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:05:37 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/28 15:06:13 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef APARSER_HPP
# define APARSER_HPP

# include <exception>
# include <string>
# include <cctype>
# include <vector>
# include <utility>

//! Character aliases
# define LEFT_BRACKET '['
# define RIGHT_BRACKET ']'
# define LEFT_CURLY_BRACE '{'
# define RIGHT_CURLY_BRACE '}'
# define CARET '^'
# define BACKTICK '`'
# define BACKSLASH '\\'
# define UNDERSCORE '_'
# define PIPE '|'
# define SPACE ' '
# define POINT '.'
# define MINUS '-'
# define PLUS '+'
# define COLON ':'
# define CR 0x0D
# define LF 0x0A

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
	class InvalidNicknameException : public std::exception {
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

	//! Str analysis
	static bool										isNickname( const std::string& s );

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


	//! Message parameter analysis
	static std::string					getChannelName( const std::string& arg, std::string::const_iterator& pos );
	static std::vector<std::string>		getChannelList( const std::string& arg );

	static std::string					getKey( const std::string& arg, std::string::const_iterator& pos );
	static std::vector<std::string>		getKeyList( const std::string& arg );

	//! Mode parser
	static std::string					getMode( const std::string& arg, std::string::const_iterator& pos  );
	static std::vector<std::string>		getModeList( const std::string& arg );

private:

};

#endif // !APARSER_HPP
