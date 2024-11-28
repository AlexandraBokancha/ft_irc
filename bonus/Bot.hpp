#ifndef BOT_HPP
# define BOT_HPP
# include <iostream>
# include <vector>

class Bot
{
    public:
    
        Bot(void);
        Bot(const Bot& other);
        Bot &operator=(const Bot &other);
        ~Bot();
    
    private:

        int                         _port;
        const std::string           _hostname;
        const std::string           _username;
        const std::string           _nickname;
        std::vector<std::string>    _channel;
        
};


#endif

