#include "Bot.hpp"

int sendMsg(int socket, const char *buf, int len) {
    int total = 0; // bytes sent
    int left = len; // bytes left to sent
    int b;

    while (total < len){
        b = send(socket, buf + total, left, MSG_DONTWAIT);
        if (b == -1) {
			break;
		}
        total += b;
        left -= b;
    }
    return (b == -1 ? -1 : 0); // return -1 on error, 0 on success
}

/* ************************************************************************** */
/* *                             Signal Handling                            * */
/* ************************************************************************** */
int g_signal = 0;

void	signal_handler(int signum) {
	if (signum == SIGINT || signum == SIGQUIT) {
		g_signal = signum;
	}
}

void	set_signal( void ) {
	signal(SIGQUIT, signal_handler);
	signal(SIGINT, signal_handler);
}


/* ************************************************************************** */
/* *                       Constructors && Destructors                      * */
/* ************************************************************************** */
Bot::Bot(void){
    this->_sockfd = -1;
    return;
}

Bot::Bot(const Bot &other) : _sockfd(other._sockfd) {
    return ;
}

Bot::~Bot(void){
    return ;
}

/* ************************************************************************** */
/* *                             Operator Overload                          * */
/* ************************************************************************** */
Bot &Bot::operator=(const Bot &other){
    if (this != &other){
        *this = other;
    }
    return (*this);
}

/* ************************************************************************** */
/* *                              Member function                           * */
/* ************************************************************************** */
bool    Bot::hasFword( const std::string &msg, const std::string &word ) {
    size_t  pos = msg.find(word);
    if (pos != std::string::npos) {
        std::cout << "F word found at index " << pos << std::endl;
        return (true);
    }
    return (false);
}


void    Bot::runBot( void ) {
    int     nbytes;
    char    buf[512];
    
    set_signal();

    sendMsg(this->_sockfd, "PASS pass\r\n", 11);
    sendMsg(this->_sockfd, "NICK bot\r\n", 10);
    sendMsg(this->_sockfd, "USER ircBot localhost bot :Bot\r\n", 32);
    
    for (;;) {
        
        if (g_signal == SIGINT || g_signal == SIGQUIT) {
            continue; //! bot has a blocking socket, it will ignore signals
        }
        
        if ((nbytes = recv(this->_sockfd, buf, 512, 0)) == -1) {
            throw (std::runtime_error("recv() error"));
        }
        
        if (nbytes == 0) { //! bot is closed as soon as a server has been closed
            std::cout << "Server was closed" << std::endl;
            break;
        }
        
        buf[nbytes] = '\0';
        std::string str(buf);
       
        //!< join #main channel
        size_t pos = str.find("001");
        if (pos != std::string::npos) {
            sendMsg(this->_sockfd, "JOIN #main\r\n", 12);
        }
            
        //!< send welcoming message
        size_t join_pos = str.find("JOIN");
        if (join_pos != std::string::npos) { 
            size_t nick_pos = str.find('!');
            if (nick_pos != std::string::npos) {
                if (str.substr(1, nick_pos - 1) != "bot") {
                    std::string welcoming_msg = "PRIVMSG #main :Welcome to channel, " + str.substr(1, nick_pos - 1) + "\r\n";
                    sendMsg(this->_sockfd, welcoming_msg.c_str(), welcoming_msg.length());
                }
            }
        }

        //!< kick user from the channel #main if he's sending "F*" word
        std::cout << str << std::endl;
        size_t privmsg_pos = str.find("PRIVMSG  #main");
        if (privmsg_pos != std::string::npos) {
            size_t msg_pos = str.find(" :");
            if (msg_pos != std::string::npos) {
                std::string message = str.substr(msg_pos + 2);
                message.erase(message.find_last_not_of(" \t\n\r\f\v") + 1);  // !< Trim the message

                if (hasFword(message, "fuck") || hasFword(message, "FUCK")) {
                    size_t nick_start = str.find(':') + 1;
                    size_t nick_end = str.find(' ');
                    if (nick_start != std::string::npos && nick_end != std::string::npos) {
                        std::string user = str.substr(nick_start, nick_end - nick_start);
                        
                        std::string kick_msg = "KICK #main " + user + " :No F* words allowed. You've been removed.\r\n";
                        sendMsg(this->_sockfd, kick_msg.c_str(), kick_msg.length());
                        
                        std::string notice_msg = "NOTICE " + user + " :No F* words allowed. You've been removed.\r\n";
                        sendMsg(this->_sockfd, notice_msg.c_str(), notice_msg.length());
                    }
                }
            }
        }
    }
}

void    Bot::startBot( void ) {
    struct hostent      *hostInfo;
    struct sockaddr_in  botInfo;

    if ((hostInfo = gethostbyname("localhost")) == NULL) {
        throw (std::runtime_error("gethostbyname() error"));
    }
    
    if ((this->_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        throw (std::runtime_error("socket() error"));
    }
    
    botInfo.sin_family = AF_INET;
    botInfo.sin_port = htons(6667);
    botInfo.sin_addr = *((struct in_addr *)hostInfo->h_addr_list[0]);
    std::memset(&(botInfo.sin_zero), 0, sizeof(botInfo.sin_zero));

    if (connect(this->_sockfd, (struct sockaddr *)&botInfo, sizeof(struct sockaddr)) == -1) { //! connect bot to server
        throw (std::runtime_error("connect() error"));
    }
}

void Bot::stopBot( void ) {
    if (this->_sockfd != -1){
        close(this->_sockfd);
    }
}


