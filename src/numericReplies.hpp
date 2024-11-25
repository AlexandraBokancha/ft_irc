#ifndef NUMERIC_REPLIES_HPP
# define NUMERIC_REPLIES_HPP

// PASS

# define ERR_NEEDMOREPARAMS(command) (std::string(":localhost 461 *") + command + std::string(" :Not enough parameters.\r\n"))
# define ERR_ALREADYREGISTRED(nick) (":localhost 462 " + nick + " :You may not reregister.\r\n")
# define ERR_PASSWDMISMATCH() (":localhost 464 * :Password incorrect.\r\n") // avec "*" a la place de nick car pass est 
                                                                            // une premiere cmd a executer (logiquement 
                                                                            // on peut pas avoir le nick)

// NICK

# define ERR_NONICKNAMEGIVEN() (":localhost 431 :No nickname given\r\n")
# define ERR_NICKNAMEINUSE(nick) (std::string(":localhost 433 * ") + nick + std::string(" :Nickname is already in use\r\n")) 
# define ERR_ERRONEUSNICKNAME(nick) (std::string(":localhost 432 * ") + nick + std::string(" :Erroneus nickname\r\n"))

// succesful authentification 
# define RPL_WELCOME(nick, user, host) (":" + host + " 001 " + nick + " :Welcome to the Inernet Relay Network " \
    + nick + "!" + user + "@localhost" + "\r\n")

// TIME
# define RPL_TIME(timeStr) (":localhost 391 " + timeStr + "\r\n")

// INFO
# define RPL_INFO(str) (":localhost 371 "  + str + "\r\n")
# define RPL_ENDOFINFO() (":localhost 374 : End of /INFO\r\n")

#endif                                                                       
