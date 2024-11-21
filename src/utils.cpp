#include "Server.hpp"

void replyToClient(std::string msg, int *fd){
	write(*fd, msg.c_str(), msg.length());
}