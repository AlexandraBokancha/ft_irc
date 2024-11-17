/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 21:50:40 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/13 14:37:19 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_HPP
# define LOG_HPP

#include <cstdarg>
#include <ctime>
#include <iostream>
#include "Colors.hpp"

void	log(const char *format, ...);
void	war_log(const char *format, ...);
void	err_log(const char *format, ...);
void	success_log(const char *format, ...);
void	fatal_log(const char *format, ...);

#endif // !LOG_HPP
