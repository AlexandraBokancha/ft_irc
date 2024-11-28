# include "Bot.hpp"
# include <netdb.h> 
# include <cstring>
# include <exception>
# include <unistd.h>

//#include <sys/types.h> 
//#include <netinet/in.h> 
//#include <sys/socket.h> 


int sendMsg(int socket, const char *buf, int len) {
    int total = 0; // bytes sent
    int left = len; // bytes left to sent
    int b;

    while (total < len){
        b = send(socket, buf + total, left, MSG_DONTWAIT);
        if (b == -1) {
			//err_log("send(): %s.", std::strerror(errno));
			break;
		}
        total += b;
        left -= b;
    }
    // *len = total; //!< Why
    return (b == -1 ? -1 : 0); // return -1 on error, 0 on success
}

int main(){
    struct hostent      *hostInfo;
    int                 sockfd, nbytes;
    struct sockaddr_in  botInfo;
   char                buf[512];

    if ((hostInfo = gethostbyname("localhost")) == NULL){
        throw (std::runtime_error("gethostbyname() error"));
    }
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        throw (std::runtime_error("socket() error"));
    }
    
    botInfo.sin_family = AF_INET;
    botInfo.sin_port = htons(6667);
    botInfo.sin_addr = *((struct in_addr *)hostInfo->h_addr_list[0]);
    std::memset(&(botInfo.sin_zero), 0, sizeof(botInfo.sin_zero));

    if (connect(sockfd, (struct sockaddr *)&botInfo, sizeof(struct sockaddr)) == -1){
        throw (std::runtime_error("connect() error"));
    }
    
    sendMsg(sockfd, "PASS pass\r\n", 11);
    sendMsg(sockfd, "NICK bot\r\n", 10);
    sendMsg(sockfd, "USER ircBot localhost bot :Bot\r\n", 32);
    
    // if "PING" -> envoie "PONG" (faut coder PING)
    // if "001" -> "JOIN"
    for (;;){
        if ((nbytes = recv(sockfd, buf, 512, 0)) == -1){
            throw (std::runtime_error("recv() error"));
        }
        if (nbytes == 0){
            std::cout << "Server was closed" << std::endl;
            break;
        }
        buf[nbytes] = '\0';
        std::cout << "Recu: " << buf << std::endl;
    }

    close(sockfd);
    return (0);
}