/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 19:35:30 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/13 19:35:45 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

class Channel {
public:
	Channel( void );
	Channel( Channel const & rhs );
	~Channel();

	Channel& operator=( Channel const & rhs );

private:
	
};

#endif // !CHANNEL_HPP
