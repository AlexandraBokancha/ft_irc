/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AParser.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 18:05:37 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/22 00:40:30 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef APARSER_HPP
# define APARSER_HPP

# include <string>
# include <vector>
# include <utility>

class AParser {
public:
	virtual ~AParser() = 0;

	virtual void	parse(std::vector<std::string> arg) = 0;

	static std::string					getChannelName( const std::string& arg, std::string::const_iterator& pos );
	static std::vector<std::string>		getChannelList( const std::string& arg );

	static std::string					getKey( const std::string& arg, std::string::const_iterator& pos );
	static std::vector<std::string>		getKeyList( const std::string& arg );

	class IllegalChannelNameException : public std::exception {
		public:
			const char*	what() const throw();
	};

private:

};

#endif // !APARSER_HPP
