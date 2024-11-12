#include "log.hpp"

void	log(const char *format, ...) {
	time_t		now = time(NULL);
	struct tm	*datetime = localtime(&now);
	char		timestamp[50];
	va_list		args;

	va_start(args, format);
	strftime(timestamp, 50, "[%H:%M:%S %e/%m/%y]", datetime);
	std::cout << GRN << timestamp << RESET << WHT << " : ";
	while (*format) {
		if (*format == '%') {
			format++;
			switch (*format) {
				case 'd': {
					int i = va_arg(args, int);
					std::cout << i;
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

void	war_log(const char *format, ...) {
	time_t		now = time(NULL);
	struct tm	*datetime = localtime(&now);
	char		timestamp[50];
	va_list		args;

	va_start(args, format);
	strftime(timestamp, 50, "[%H:%M:%S %e/%m/%y]", datetime);
	std::cout << YEL << timestamp << RESET << WHT << " : ";
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

void	err_log(const char *format, ...) {
	time_t		now = time(NULL);
	struct tm	*datetime = localtime(&now);
	char		timestamp[50];
	va_list		args;

	va_start(args, format);
	strftime(timestamp, 50, "[%H:%M:%S %e/%m/%y]", datetime);
	std::cout << RED << timestamp << RESET << WHT << " : ";
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

// template<typename T>
// void	log(T str) {
// 	time_t		now = time(NULL);
// 	struct tm	*datetime = localtime(&now);
// 	char		timestamp[50];
//
// 	strftime(timestamp, 50, "[%H:%M:%S %e/%m/%y]", datetime);
// 	std::cout << GRN << timestamp << RESET << WHT << " : " << str << RESET << std::endl;
// }
//
// template<typename T>
// void	war_log(T str) {
// 	time_t		now = time(NULL);
// 	struct tm	*datetime = localtime(&now);
// 	char		timestamp[50];
//
// 	strftime(timestamp, 50, "[%H:%M:%S %e/%m/%y]", datetime);
// 	std::cout << YEL << timestamp << RESET << WHT << " : " << str << RESET << std::endl;
// }
//
// template<typename T>
// void	err_log(T str) {
// 	time_t		now = time(NULL);
// 	struct tm	*datetime = localtime(&now);
// 	char		timestamp[50];
//
// 	strftime(timestamp, 50, "[%H:%M:%S %e/%m/%y]", datetime);
// 	std::cout << RED << timestamp << RESET << WHT << " : " << str << RESET << std::endl;
// }
