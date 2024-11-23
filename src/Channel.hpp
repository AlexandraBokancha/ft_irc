/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 19:35:30 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/23 13:41:53 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <list>

# define O 0x01 //!< Give/take channel operator privileges
# define I 0x04 //!< Invite-only channel
# define T 0x05 //!< Topic settable by channel operator only
# define L 0x08 //!< User limit for the channel
# define K 0x0B //!< Set/remove the channel key (password)

class Client;

class Channel {
public:
	Channel( void );
	Channel( Channel const & rhs );
	Channel( Client* creator, const std::string& name );
	~Channel();

	Channel& operator=( Channel const & rhs );

	std::string			getName( void ) const;

private:
	std::string			_name; //!< Channel name
	std::string			_topic;

	std::list<Client*>	_client; //!< Reference to connected clients to the channel
	Client*				_operator; //!< Channel operator

	int					_mode; //!< Mode flag
	int					_userLimit; //!< 0: no User limit
	std::string			_password;
};

#endif // !CHANNEL_HPP
