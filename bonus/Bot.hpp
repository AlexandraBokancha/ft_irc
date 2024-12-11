#ifndef BOT_HPP
# define BOT_HPP
# include <iostream>
# include <vector>
# include <netdb.h> 
# include <cstring>
# include <exception>
# include <unistd.h>
# include <sstream>
# include <signal.h>
# include <fcntl.h>
# include <cerrno>

extern int g_signal;

class Bot
{
    public:
    
        Bot(void);
        Bot(const Bot& other);
        Bot &operator=(const Bot &other);
        ~Bot();
        
        bool hasFword(const std::string &msg, const std::string &word);
        void startBot( void );
        void runBot( void );
        void stopBot( void );
    
    private:

        int _sockfd;
};


#endif

