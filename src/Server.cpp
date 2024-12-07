/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexandra <alexandra@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 11:10:53 by dbaladro          #+#    #+#             */
/*   Updated: 2024/12/07 09:16:14 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Message.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "CommandExecutor.hpp"


/* FOR TESTING PURPOSE */
#include "NumericResponse.hpp"
# include "log.hpp"
#include <cstring>
#include <netdb.h>
#include <string>

void	Server::printChannel( void ) const {
	std::cout << "Channel :" << std::endl;
	for (std::vector<Channel>::const_iterator it = _channel.begin(); it != _channel.end(); it++) {
		it->printChannel();
	}
}

void	print_client(std::vector<Client> v) {
	for (std::vector<Client>::const_iterator it = v.begin(); it < v.end(); it++) {
		std::cout << *it << std::endl;
	}
}

void	printServerInfo(struct addrinfo *a) {
	std::cout << "ServerInfo :" << a << std::endl;
        for (struct addrinfo* p = a; p != NULL; p = p->ai_next) {
            void* addr;
            char ipstr[INET6_ADDRSTRLEN];
            // Get the pointer to the address itself
            if (p->ai_family == AF_INET) { // IPv4
                struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
                addr = &(ipv4->sin_addr);
            } else { // IPv6
                struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)p->ai_addr;
                addr = &(ipv6->sin6_addr);
            }
		
            // Convert the IP to a string
            inet_ntop(a->ai_family, addr, ipstr, sizeof(ipstr));
			std::cout << "addr: " << ipstr << std::endl;
		}
}

/* ************************************************************************** */
/* *                             Signal Handling                            * */
/* ************************************************************************** */
int g_signal = 0;

void	signal_handler(int signum) {
	if (signum == SIGINT || signum == SIGQUIT) {
		std::cout << std::endl;
		log("%s======= SHUTDOWN SIGNAL RECEIVED =======%s", BLU, RESET);
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
Server::Server( void )
	: _port(8080), _passwd("0000"), _opPasswd("admin"), _opUser("admin")
{
	this->_serverInfo = NULL;
	this->_socket = -1;
	this->_channel = std::vector<Channel>();
	this->_client = std::vector<Client*>();
	this->_pollFd = std::vector<struct pollfd>();
	return ;
}

Server::Server(const int port, const std::string password )
	: _port(port), _passwd(password), _opPasswd("admin"), _opUser("admin")
{
	std::cout << "Created Server object using port " << port << std::endl;
	this->_serverInfo = NULL;
	this->_socket = -1;
	this->_channel = std::vector<Channel>();
	this->_client = std::vector<Client*>();
	this->_pollFd = std::vector<struct pollfd>();
	return ;
}

Server::Server( Server const &rhs )
	: _port(rhs._port), _passwd(rhs._passwd)
{
	this->_serverInfo = NULL;
	this->_socket = -1;
	this->_channel = std::vector<Channel>();
	this->_client = std::vector<Client*>();
	this->_pollFd = std::vector<struct pollfd>();
	return ;
}

Server::~Server() {
	return ;
}

/* ************************************************************************** */
/* *                             Operator Overload                          * */
/* ************************************************************************** */
Server& Server::operator=( Server const & rhs ) {
	if (this != &rhs) {
		*this = rhs;
	}
	return (*this);
}

/* ************************************************************************** */
/* *                            Getters and Setters                         * */
/* ************************************************************************** */

/**
 * @brief Created server prefix for server response
 *
 * @return Server prefix
 */
std::string	Server::getPrefix( void ) const {
	return (this->_ip);
}

std::string Server::getOpPasswd( void ) const {
	return (this->_opPasswd);
}

std::string Server::getOpUser( void ) const {
	return (this->_opUser);
}

std::string Server::getPasswd( void ) const{
	return (this->_passwd);
}

int Server::getPort( void ) const{
	return (this->_port);
}

/* ************************************************************************** */
/* *                              Member function                           * */
/* ************************************************************************** */
/**
 * @brief Add new <struct pollfd> to server poll
 *
 * Add new element to poll
 *
 * @param fd The socket file descriptor to add
 * @param events The poll event mask
 */
void	Server::pollPushBack(int fd, short events) {
	struct pollfd	new_socket;
	
	new_socket.fd = fd;
	new_socket.events = events;
	new_socket.revents = 0;
	this->_pollFd.push_back(new_socket);
}

/**
 * @brief Compare param to server password
 *
 * This function make the password access more secure by never directly returning it
 *
 * @param str The client pass argument to be compared with server password
 * @return 0 when match password, else 1
 */
int Server::comparePassword(const std::string& str) const {
	return ((str.compare(this->_passwd) == 0 ? 0 : 1));
}

/**
 * @brief return client based on client socket
 *
 * Get client base on client socket
 *
 * @param client_socket Client socket
 * @preturn The client which the sockt is refering to, NULL if not found
 */
Client*	Server::findClient( int client_sock ) {
	std::vector<Client*>::iterator it;
	for (it = this->_client.begin(); it != this->_client.end(); it++) {
		if ((*it)->getFd() == client_sock)
			return (*it);
	}
	return (NULL);
}

/**
 * @brief Return lient based on client nickname
 *
 * Find client based on nickname (Check client existence)
 *
 * @param nick Nickname to search
 * @return Client pointer if found, else NULL
 */
Client*	Server::findClient( const std::string& nick ) {
	std::vector<Client*>::iterator it;
	for (it = this->_client.begin(); it != this->_client.end(); it++) {
		if ((*it)->getNickname() == nick)
			return (*it);
	}
	return (NULL);
}

int Server::findClientIndex( const std::string & nick ){
	for (std::size_t i = 0; i < _client.size(); ++i){
		if (_client[i]->getNickname() == nick){
			return (static_cast<int>(i));
		}
	}
	return (-1);
}


/**
 * @brief Server response to client command
 *
 * This is a way to snend formated response to client command in a formatted way
 *
 * @param The source of the message (NULL if it come from the server)
 * @param client_sock Client socket to send message to
 * @param fmt The formated message to send (defined in Numericresponse.hpp)
 * @param ... The fmt argument
 */
void	Server::respond(const Client* src, const int& client_sock, const char* fmt, ...) const {
	va_list		args;
	std::string	buffer;
	Message		response;

	if (!src)
		response.setPrefix(this->getPrefix());
	else
		response.setPrefix(src->getPrefix());

	buffer.reserve(512);
	va_start(args, fmt);
	while (*fmt) {
		if (*fmt == '%') {
			fmt++;
			switch (*fmt) {
				case 's' : { //!< string
					char* s = va_arg(args, char* );
					buffer.append(s);
					break ;
				}
				case 'd' : { //!< int 
					int d = va_arg(args, int);
					std::stringstream ss;
					ss << d;
					buffer.append(ss.str());
					break ;
				}
				case 'l' : { //!< int 
					long int l = va_arg(args, long int);
					std::stringstream ss;
					ss << l;
					buffer.append(ss.str());
					break ;
				}
				case 'c' : {
					char c = static_cast<char>(va_arg(args, int));
					buffer.append(1, c);
					break ;
				}
				default :
				   break ;
			}
			fmt++;
			continue ;
		}
		buffer.append(1, *fmt);
		fmt++;
	}
	va_end(args);

	log("Sending :%s to client: %d", buffer.c_str(), client_sock);

	response.setContent(buffer);
	sendMsg(client_sock, response);
}

/**
 * @brief Add new channel to server
 *
 * Used when joining a channel that doesn't exst
 *
 * @param channel The channel to add to server
 */
void	Server::addChannel( Channel& channel ) {
	this->_channel.push_back(channel);
	return ;
}

/**
 * @brief Delete channel
 *
 * To be used when a client disconnect
 *
 * @param Channel to be deleted
 */
void	Server::delChannel( Channel& channel ) {
	std::vector<Channel>::iterator	it;

	for (it = this->_channel.begin(); it != this->_channel.end(); it++) {
		if (it->getName() == channel.getName()) {
			this->_channel.erase(it);
			log("Deleting channel: %s", it->getName().c_str());
			return ;
		}
	}
}

/**
 * @brief find channel by name
 *
 * Find channel identified by name passed in paramters
 *
 * @param name Channel name to find
 * @return Channel pointer if found, else NULL
 */
Channel*	Server::findChannel( const std::string& name ) {
	std::vector<Channel>::iterator	it;
	int								safe_channel = (name[0] == '!' ? 1 : 0);
	std::string						channel_name;

	for (it = this->_channel.begin(); it != this->_channel.end(); it++) {
		channel_name = it->getName();
		//! Used to check if a safe channel with the same shortname exist
		if (channel_name[0] == '!' && safe_channel
			&& (strcmp(channel_name.c_str() + 6, name.c_str() + 1) == 0))
			return (&(*it)); // on peut remplacer par -> channel_name.substr(6).compare(name.substr(1)) == 0
		if (name.compare(it->getName()) == 0)
			return (&(*it));
	}
	return (NULL);
}


// std::vector<Channel*>	Server::clientOnChannel(Client* client ){
// 	std::vector<Channel*>	clientChannels;
// 	std::vector<Channel>::iterator it;
//
// 	for (it = this->_channel.begin(); it != this->_channel.end(); it++){
// 		if (std::find((*it).getClients().begin(), (*it).getClients().end(), client) != (*it).getClients().end()) {
//             clientChannels.push_back(&(*it));
//         }
// 	}
// 	return (clientChannels);
// }	


/**
 * @brief Disconnect client and remove it from poll
 *
 * This function remove the client-index from poll.
 * It does it by first closing the client socket,
 * then it remove all of it's data from poll
 *
 * BEWARE:
 *     The index is reduced by one and this behavioue shouldn't be changed
 *     For the moment !
 *
 * @param index	The client index in the pollFd
 */
void	Server::disconnectClient( int& index ) { if (index == 0)
		return ;
	
	int	client_sock = this->_pollFd[index].fd;
	Client	*client = this->findClient(client_sock);
	//! Find every channel were client is connected
	if (client_sock > 0) {
		for (std::vector<Channel>::iterator it = _channel.begin(); it != _channel.end(); it++) {
			if (it->getClient(client_sock) != NULL) {
				it->removeClient(client_sock);
				this->broadcastToChannel(client, "QUIT :Quit: ", &(*it), client_sock);
			}
			if (it->isEmpty()) {
				this->_channel.erase(it);
				log("Removing channel %s", it->getName().c_str());
				it--;
			}
			// peut-etre on doit rajouter un broadcast ici pour notifier les clients qu'un user est parti ?
		}
	}

	log("Closing socket %d.", this->_pollFd[index].fd);
	close(this->_pollFd[index].fd);
	this->_pollFd.erase(this->_pollFd.begin() + index);

	//! Remove client from client vector
	delete (this->_client[index - 1]);
	this->_client.erase(this->_client.begin() + index - 1);

	index--;

}

                           /* SERVER HANDLING */
// TESTING

// END TESTING

/**
 * @brief Start the server
 *
 * Init server variable and signals, create socket, bind it and start listening
 *
 * @param Port in char* format just for an easier access to get_addr
 */
void	Server::startServer( const char *port_str ) {
	struct addrinfo	hints, *p;
	struct addrinfo	*servInfo;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;		 //!< Use AF_UNSPEC for both ipv4 ipv6 interfaces
	hints.ai_socktype = SOCK_STREAM; //!< TCP stream socket
	hints.ai_flags = AI_PASSIVE;	 //!< <=> INADDR_ANY <=> "0.0.0.0"
									 //!< Used by application that indent to accept
									 //!< connexion on any of the host net adresses
	if (getaddrinfo(NULL, port_str, &hints, &servInfo) != 0)
		throw (std::runtime_error(std::string("getaddrinfo: ") + std::strerror(errno)));

	this->_serverInfo = servInfo;

    char ipstr[INET_ADDRSTRLEN];
    for (p = servInfo; p != NULL; p = p->ai_next) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
        void *addr = &(ipv4->sin_addr);

        // Convert the IP to a string and return it
        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        break; // We only need the first IP address
    }
	this->_ip = ipstr;

	// this->_socket = socket(this->_serverInfo->ai_family, this->_serverInfo->ai_socktype, this->_serverInfo->ai_protocol);
	this->_socket = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol);
	if (this->_socket == -1)
		throw (std::runtime_error(std::string("socket: ") + std::strerror(errno)));
	this->pollPushBack(this->_socket, POLLIN); //!< Add server to poll
	int	reusable_addr = 1;
	if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &reusable_addr, sizeof(reusable_addr)) < 0)
		throw (std::runtime_error(std::string("setsockopt: ") + std::strerror(errno)));

	success_log("Socket %d created", this->_socket);

	if (bind(this->_socket, this->_serverInfo->ai_addr, this->_serverInfo->ai_addrlen) < 0)
		throw (std::runtime_error(std::string("bind: ") + std::strerror(errno)));

	success_log("Binded socket %d.", this->_socket);

	if (listen(this->_socket, 256) < 0)
		throw (std::runtime_error(std::string("listen: ") + std::strerror(errno)));

	log("Socket %d is listening on port %d.", this->_socket, this->_port);

	set_signal();
}

/**
 * @brief Stop the server
 *
 * Stop the server by disconnecting every client and closing associated sockets
 * Should be called just before server desctructor / End of program
 */
void		Server::stopServer( void ) {
	if (this->_pollFd.size() > 1) {
		log("Disconecting every client...");
		int i = this->_pollFd.size() - 1;
		while (i > 0)
			disconnectClient(i);
	}
	log("%sStopping server%s", WHT, RESET);
	if (this->_socket != -1) {
		log("Closing socket %d.", this->_socket);
		close(this->_socket);
	}
	if (this->_serverInfo != NULL) {
		log("Freeing server informations");
		freeaddrinfo(this->_serverInfo);
	}
	log("Server stopped!");
}

/**
 * @brief Accept a new client connecting to the server
 *
 * Accept new client after a POLLIN event have been caught on the Server socket
 */
void	Server::acceptNewClient( void ) {
	Client				*new_client = new Client();

	int					client_socket;
	struct sockaddr_in	client_addr;
	int					addr_len = sizeof(client_addr);

		client_socket = accept(this->_socket, (struct sockaddr*)&client_addr,
			reinterpret_cast<socklen_t *>(&addr_len));
		if (client_socket < 0) //!< accept error
			return (err_log("%sInternal error%s: %saccept%s: %s",
					RED, RESET, MAG, RESET, std::strerror(errno)));
		success_log("accepted client on: %s", inet_ntoa(client_addr.sin_addr));

		this->pollPushBack(client_socket, POLLIN); //!< add client fd to poll

		//! Add client
		new_client->setFd(client_socket);
		// new_client.setFd(&client_socket);
		new_client->setNetId(client_addr);
		this->_client.push_back(new_client);

}

/**
 * @brief Send the complete message to Client
 *
 * Protected send ensuring that the full message real the client at one time
 *
 * @param socket The client socket
 * @param buf The buffer containing the message to send
 * @param len The message len
 *
 * @return 0 on SUCCESS; -1 on ERROR
 */
int Server::sendMsg(int socket, const char *buf, int len) const {
    int total = 0; // bytes sent
    int left = len; // bytes left to sent
    int b;

    while (total < len){
        b = send(socket, buf + total, left, MSG_DONTWAIT);
        if (b == -1) {
			err_log("send(): %s.", std::strerror(errno));
			break;
		}
        total += b;
        left -= b;
    }
    return (b == -1 ? -1 : 0); // return -1 on error, 0 on success
}


/**
 * @brief Send a message to client socket
 *
 * Just on easier way to send a msg object directly instead of buffer
 *
 * @param socket Client socket to send message to
 * @param msg Message to send
 *
 * @return 0 on SUCCESS; -1 on ERROR
 */
int Server::sendMsg(int socket, const Message& msg ) const {
	std::string	buffer = msg.toString();

    return (this->sendMsg(socket, buffer.c_str(), buffer.size()));
}

/**
 * @brief Send msg to all client except the one specified by fd
 *
 * This function is needed to broadcast the irc message when needed
 *
 * @param buffer The buffer containing the message to send
 * @param len The message len
 * @param fd The client exluded from receiving the message (the sender)
 */
void	Server::broadcast(const char *buffer, int len, int fd) const {
	for (unsigned long int i = 1; i < this->_pollFd.size(); i++) {
		if (this->_pollFd[i].fd == fd)
			continue ;

		log("Sending back msg received on socket %d to %d", fd, this->_pollFd[i].fd);

		if (sendMsg(this->_pollFd[i].fd, buffer, len) == -1)
			err_log("Msg could not be sent on socket %d", this->_pollFd[i].fd);
	}
}

void Server::broadcastToChannel( const Client* src, const std::string& msg, const Channel * ch, int fd ) const {
	const std::vector< std::pair<Client *, int> >& clients = ch->getClients(); //!< clients connected to this channel

	for (std::vector<std::pair<Client *,int> >::const_iterator it = clients.begin(); it != clients.end(); ++it){
			if (it->first->getFd() != fd)
				this->respond(src, it->first->getFd(), msg.c_str());
	}
}

/**
 * @brief Receive client message after POLLIN event
 *
 * This function read the Msg send by the client after POLLIN event
 * IT SHOULD BE UPDATED ACCORDINGLY
 *
 * @param i The pollFd index
 */
void	Server::receiveMsg( int& i ) {
	
	char	buffer[512]; //<! 512 = max irc message size
	int buffer_size = recv(this->_pollFd[i].fd, buffer, 512 - 1, MSG_DONTWAIT);

	/* TESTING */
	if (buffer_size > 0) { //!< Received msg
		try{
			buffer[buffer_size] = '\0';
			int msg_i = 0;
			Client* sender = findClient(this->_pollFd[i].fd);

			log("Recevied from client on socket %d: %s", this->_pollFd[i].fd, buffer);
			while (msg_i < buffer_size - 2){
				Message msg(buffer, msg_i, buffer_size);
				
				CommandExecutor::execute(*this, *sender, msg);

				// broadcast(buffer + start, msg_i - start - 2, this->_pollFd[i].fd);
			}
		}
		catch (std::exception & e){
			err_log("MESSAGE: %s : %s", buffer, e.what());
		}
		return;
	}
	if (buffer_size == 0)
		log("Client disconnected on socket %d", this->_pollFd[i].fd);
	if (buffer_size < 0)
		err_log("Could not received data on socket %d: %srecv%s: %s.", this->_pollFd[i].fd, MAG, RESET, std::strerror(errno));
	disconnectClient(i);
}

/**
 * @brief Check if an event have been received and treat it if received
 *
 * This function check if an event have been received on the revents field
 *
 * It either catch a POLLIN event and will read the client Msg.
 * Or it received on ERROR and will display a log according to the error
 * Or it do nothing @param i The pollFd index
 *
 * @param server poll index
 */
void	Server::checkEvent( int& i ) {
	//! Receive nothing
	if (this->_pollFd[i].revents == 0)
		return ;

	//! Receive data
	if (this->_pollFd[i].revents & POLLIN)
		return (this->receiveMsg(i));	
				
	//! received error
	if (this->_pollFd[i].revents & POLLHUP)
		err_log("Client hang up on socket %d.", this->_pollFd[i].fd);
	if (this->_pollFd[i].revents & POLLERR)
		err_log("Error on socket %d.", this->_pollFd[i].fd);
	if (this->_pollFd[i].revents & POLLNVAL)
		err_log("Invalid request on socket %d.", this->_pollFd[i].fd);
	disconnectClient(i);
}

/**
 * @brief Run the server
 */
void	Server::runServer( void ) {
	int	status;

	log("Server waiting for connection...");

	//! Perform poll check until error or signal catch
	while ((status = poll(this->_pollFd.data(), this->_pollFd.size(), -1)) > 0) {

		if (this->_pollFd[0].revents == POLLIN) //!< New client connecting
			this->acceptNewClient();

		//! Check if data has been received on client socket
		for (int i = 1; i < static_cast<int>(this->_pollFd.size()); i++) {
			if (this->_pollFd[i].fd > 0)
				this->checkEvent(i);
			else if (this->_pollFd[i].fd == 0) { //! Case unused fd
				war_log("Removed unused fd from pollfd [%sShouldn't happened%s]", YEL, RESET);
				disconnectClient(i);
			} else
				war_log("Why tha F*** socket = %d?", this->_pollFd[i].fd);
		}
        if (status == 0)
            fatal_log("poll(): Timeout");
        if (status < 0)
			fatal_log("poll(): %s.", std::strerror(errno));
	}
}

/* ************************************************************************** */
/* *                             Class  Exception                           * */
/* ************************************************************************** */
const char*	Server::InvalidArgException::what() const throw() {
	return ("Use the program as folow: ./ircserv <port> <password>");
}

const char*	Server::InvalidPortException::what() const throw() {
	return ("ircserv: Invalid port");
}
