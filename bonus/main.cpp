# include "Bot.hpp"

int main(){
    Bot bot; //!< Creating bot
    try{
        bot.startBot();
        bot.runBot();
    }
    catch (std::exception& e){
        std::cout << e.what() << std::endl;
        return (bot.stopBot(), 1);
    }
    return (bot.stopBot(), 0);
}