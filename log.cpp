/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 11:03:57 by dbaladro          #+#    #+#             */
/*   Updated: 2024/11/13 14:41:26 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file log.cpp
 * @brief Server logger
 *
 * Server logs, created similarly as minimalist printf formatting :
 *     %d : Int number
 *     %f : double number
 *     %s : char* characters
 * 
 * The %s flag is often use for coloring has follow :
 *     ("%s some red text %s %s some blue test %s ", RED, RESET, BLU, RESET)
 * Note how the colored text is preceeded by %s flag containing color,
 * and an %s flag reseting color to default
 */


#include "log.hpp"

/**
 * @brief Create timestamp in the buffer
 *
 * The buffer must be previously allocated and big enough
 */
static void	get_timestamp(char *buffer) {
	time_t		now = time(NULL);
	struct tm	*datetime = localtime(&now);
	
	strftime(buffer, 50, "[%H:%M:%S %e/%m/%y]", datetime);
}

/**
 * @brief Information log
 *
 * Print the INFO log message using formatted paramters.
 *
 * To get complete description of flags refer to global file comment
 *
 * @param format The string containing format flags (like printf)
 * @param ... The associated flag value (like printf)
 */
void	log(const char *format, ...) {
	char		timestamp[50];
	va_list		args;

	get_timestamp(timestamp);
	std::cout << CYN << timestamp << RESET << WHT << ": "
		<< "[INFO]   " << RESET << ": ";
	va_start(args, format);
	while (*format) {
		if (*format == '%') {
			format++;
			switch (*format) {
				case 'd': {
					int i = va_arg(args, int);
					std::cout << CYN << i << RESET;
					break ;
				}
				case 'f': {
					float f = va_arg(args, double);
					std::cout << f;
					break ;
				}
				case 's': {
					char *s = va_arg(args, char *);
					std::cout << s;
					break;
				}
				default :
					std::cout << "%" << *format;
			}
		}
		else
			std::cout << *format;
		format++;
	}
	std::cout << std::endl;
}

/**
 * @brief Information log
 *
 * Print the SUCCESS log message using formatted paramters.
 *
 * To get complete description of flags refer to global file comment
 *
 * @param format The string containing format flags (like printf)
 * @param ... The associated flag value (like printf)
 */
void	success_log(const char *format, ...) {
	char		timestamp[50];
	va_list		args;

	get_timestamp(timestamp);
	std::cout << CYN << timestamp << RESET << WHT << ": "
		<< FLUOGRN << "[SUCCESS]" << RESET << ": ";
	va_start(args, format);
	while (*format) {
		if (*format == '%') {
			format++;
			switch (*format) {
				case 'd': {
					int i = va_arg(args, int);
					std::cout << CYN << i << RESET;
					break ;
				}
				case 'f': {
					float f = va_arg(args, double);
					std::cout << CYN << f << RESET;
					break ;
				}
				case 's': {
					char *s = va_arg(args, char *);
					std::cout << s;
					break;
				}
				default :
					std::cout << "%" << *format;
			}
		}
		else
			std::cout << *format;
		format++;
	}
	std::cout << std::endl;
}

/**
 * @brief Information log
 *
 * Print the WARNING log message using formatted paramters.
 *
 * To get complete description of flags refer to global file comment
 *
 * @param format The string containing format flags (like printf)
 * @param ... The associated flag value (like printf)
 */
void	war_log(const char *format, ...) {
	char		timestamp[50];
	va_list		args;

	get_timestamp(timestamp);
	va_start(args, format);
	std::cout << CYN << timestamp << RESET << WHT << ": "
		<< YEL << "[WARNING]" << RESET << ": ";
	while (*format) {
		if (*format == '%') {
			format++;
			switch (*format) {
				case 'd': {
					int i = va_arg(args, int);
					std::cout << CYN << i << RESET;
					break ;
				}
				case 'f': {
					float f = va_arg(args, double);
					std::cout << CYN << f << RESET;
					break ;
				}
				case 's': {
					char *s = va_arg(args, char *);
					std::cout << s;
					break;
				}
				default :
					std::cout << "%" << *format;
			}
		}
		else
			std::cout << *format;
		format++;
	}
	std::cout << std::endl;
}

/**
 * @brief Information log
 *
 * Print the ERROR log message using formatted paramters.
 *
 * To get complete description of flags refer to global file comment
 *
 * @param format The string containing format flags (like printf)
 * @param ... The associated flag value (like printf)
 */
void	err_log(const char *format, ...) {
	char		timestamp[50];
	va_list		args;

	get_timestamp(timestamp);
	std::cout << CYN << timestamp << RESET << WHT << ": "
		<< RED << "[ERROR]  " << RESET << ": ";
	va_start(args, format);
	while (*format) {
		if (*format == '%') {
			format++;
			switch (*format) {
				case 'd': {
					int i = va_arg(args, int);
					std::cout << CYN << i << RESET;
					break ;
				}
				case 'f': {
					float f = va_arg(args, double);
					std::cout << CYN << f << RESET;
					break ;
				}
				case 's': {
					char *s = va_arg(args, char *);
					std::cout << s;
					break;
				}
				default :
					std::cout << "%" << *format;
			}
		} else
			std::cout << *format;
		format++;
	}
	std::cout << std::endl;
}

/**
 * @brief Information log
 *
 * Print the FATAL log message using formatted paramters.
 *
 * To get complete description of flags refer to global file comment
 *
 * @param format The string containing format flags (like printf)
 * @param ... The associated flag value (like printf)
 */
void	fatal_log(const char *format, ...) {
	char		timestamp[50];
	va_list		args;

	get_timestamp(timestamp);
	std::cout << CYN << timestamp << RESET << WHT << ": "
		<< DRKRED << "[FATAL]  " << RESET << ": ";
	va_start(args, format);
	while (*format) {
		if (*format == '%') {
			format++;
			switch (*format) {
				case 'd': {
					int i = va_arg(args, int);
					std::cout << CYN << i << RESET;
					break ;
				}
				case 'f': {
					float f = va_arg(args, double);
					std::cout << CYN << f << RESET;
					break ;
				}
				case 's': {
					char *s = va_arg(args, char *);
					std::cout << s;
					break;
				}
				default :
					std::cout << "%" << *format;
			}
		}
		else
			std::cout << *format;
		format++;
	}
	std::cout << std::endl;
}

