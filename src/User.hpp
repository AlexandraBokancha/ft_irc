/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 18:41:35 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/13 18:41:48 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

class User {
public:
	User( void );
	User( User const & rhs );
	~User();

	User& operator=( User const & rhs );

private:
	
};

#endif // !USER_HPP
