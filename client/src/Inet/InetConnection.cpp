#include <sys/select.h>

#include "Inet/InetConnection.hpp"

#define PORT "3393"
#define UDP_BUFMAX "1500"

InetConnection::InetConnection() {
	//result = static_cast<addrinfo*>(malloc(sizeof(addrinfo)));
	/*hints = {
		AI_NUMERICHOST | AI_NUMERICSERV,  // addrinfo::ai_flags
		PF_UNSPEC,// addrinfo::ai_family
		SOCK_STREAM,// addrinfo::ai_socktype
		IPPROTO_UDP,// addrinfo::ai_protocol
		0, 0, nullptr, nullptr// unused
	};
	*/
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	iter = nullptr;
}

std::vector<Message*> messages;

void InetConnection::init(void) {
  /*
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, PORT, &hints, &result)) {
		std::cout << "Cannot resolve address. Exiting" << std::endl;
		exit (EXIT_FAILURE);
	} else {
		std::cout << "getaddrinfo success" << std::endl;
		// Go through every returned address and attempt to connect to each
		for (iter = result; iter != NULL; iter = iter->ai_next) {
			if ((listensocket = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol)) < 0) {
				std::cout << "Error socket(): " << strerror(errno) << std::endl;
				exit (EXIT_FAILURE);
				break;
			}
			if (bind(listensocket, iter->ai_addr, iter->ai_addrlen) < 0) {
				close(listensocket);
				std::cout << "Error bind(): " << strerror(errno) << std::endl;
				break;
			}
			break;
		}
	}
	std::cout << "Listening socket" << listensocket << std::endl;
	*/
}

void InetConnection::destroy(void) {
	// delete messages behind pointers
	for (auto& it : messages) {
		delete it;
	}
	// empty whole vector
	messages.clear();
}
bool InetConnection::send(std::string l_ip, std::string l_port, std::string message) {
	/* Try to send data to server:
	 * sendto(socket, data , data length, flags, destination, struct length)
	 * see 'man sendto'
	 */

	struct addrinfo * server_addrinfo = nullptr;
	if (getaddrinfo(l_ip.c_str(), l_port.c_str(), &hints, &server_addrinfo) < 0) {
		std::cout << "Cannot resolve address: " << strerror(errno) << std::endl;
		return false;
	}

	/* send to asdfasdf	 */
	if ((sendto(listensocket, message.c_str(), BUFFER_SIZE, 0, server_addrinfo->ai_addr, server_addrinfo->ai_addrlen)) < 0) {
		std::cout << "Error sentto(): " << strerror(errno) << std::endl;
		return false;
	} else {
		std::cout << "Client: Sent datagram" << std::endl;
	}
	return true;

}
// http://stackoverflow.com/questions/17769964/linux-sockets-non-blocking-connect
/**
 * connect
 * this function tries to connect to given server crediterials.
 * it makes ConnectionState CONNECTING and then tries to send message to connect
 * @params: string ip, string port. Port and IP of the server we are connecting.
 * @return: bool success. returns success if there was no socket error
 */
bool InetConnection::connect(const std::string& l_ip, const std::string& l_port) {
  if(m_state != ConnectionState::CONNECTING) {
    cout << "Trying to connect to host" << endl;
    // get server address information
    ip = l_ip;
    port = l_port;
    if(getaddrinfo(ip.c_str(), port.c_str(),&hints, &res) != 0) {
      cerr << "getaddrinfo error "<< strerror(errno) << endl;
      return false;
    }
    // create socket
  	if((sockettcp = socket(res->ai_family, res->ai_socktype | SOCK_NONBLOCK, res->ai_protocol))< 0) {
  	  cerr << "cannot create socket for tcp connection " << strerror(errno) << endl;
  	  return false;
  	}
  	if(::connect(sockettcp, res->ai_addr, res->ai_addrlen)<0) {
  	  cout << strerror(errno) << endl;
  	}

  }
	//try to connect
	m_state = ConnectionState::CONNECTING;
	//memset(static_cast<char*>(&me_addr), 0, sizeof(me_addr)):
	
	return false;
}
bool InetConnection::disconnect() {
	if (res == nullptr) freeaddrinfo(res);
	close(sockettcp);
	return true;
}

void InetConnection::update() {
  // if connecting
  
  if(m_state == ConnectionState::CONNECTING) {
    void* err;
    socklen_t len;
    getsockopt(sockettcp, SOL_SOCKET, SO_ERROR, &err, &len);
    cout << err << endl;
    /*
    if(success < 0) {
      cout << errno << endl;
      if(errno == 111)
        m_state = ConnectionState::REFUSED;
      if(errno == 114 || errno == 115)
        m_state = ConnectionState::CONNECTING;
    } else {
      cout << "should be connected" << endl;
    }
    */
    return;
  }
  /*
	memset(&timeout, 0, sizeof(timeout));
	biggestsocket = 0;
	timeout.tv_usec = 1000; // microseconds
	timeout.tv_sec = 2; // seconds
	FD_ZERO(&socket_fds); // Clear the set of file descriptors

// Add listening socket to the set and check if it is the biggest socket number
	FD_SET(listensocket, &socket_fds);
	if (listensocket > biggestsocket) {
		std::cout << "We have a descriptor: " << listensocket << std::endl;
		biggestsocket = listensocket;
	}
	Message * unpackedMessage;
	switch (select(biggestsocket + 1, &socket_fds, NULL, NULL, &timeout)) {
	case (-1):
		std::cout << "Error -1" << std::endl;
		break;
	case (0):
		std::cout << "Timeout" << std::endl;
		break;
	default:
		std::cout << "Inet update" << std::endl;
		struct MessageHeader *header = static_cast<struct MessageHeader*>(malloc(sizeof(struct MessageHeader)));

		uint8_t payloadBuffer[BUFFER_SIZE];

		for (int socket_fd = 0; socket_fd <= biggestsocket; socket_fd++) {
			if (FD_ISSET(socket_fd, &socket_fds)) {
				std::cout << "Unpacking" << std::endl;
				Message::UnpackHeader(socket_fd, header, payloadBuffer);
				unpackedMessage = MessageFactory::getInstance().getMessageByType(header, payloadBuffer);
			}
		}
		break;
	}

	switch (unpackedMessage->getMessageType()) {
	case MESSAGE_TYPE::GAME_MESSAGE:
//		switch (unpackedMessage->getMessageType()) {
//		case GAME_MESSAGE_TYPE::JOIN:
//			message = dynamic_cast<Join*>(unpackedMessage);
//		case GAME_MESSAGE_TYPE::NICK:
//			return dynamic_cast<Nick*>(unpackedMessage);
//		case GAME_MESSAGE_TYPE::EXIT:
//			return dynamic_cast<Exit*>(unpackedMessage);
//		case GAME_MESSAGE_TYPE::RESTART:
//			return dynamic_cast<Restart*>(unpackedMessage);
//		case GAME_MESSAGE_TYPE::GAME_END:
//			return dynamic_cast<GameEnd*>(unpackedMessage);
//		case GAME_MESSAGE_TYPE::GAME_UPDATE:
//			return dynamic_cast<GameUpdate*>(unpackedMessage);
//		case GAME_MESSAGE_TYPE::POINTS:
//			return dynamic_cast<Points*>(unpackedMessage);
//		case GAME_MESSAGE_TYPE::PLAYER_DEAD:
//			return dynamic_cast<PlayerDead*>(unpackedMessage);
//		case GAME_MESSAGE_TYPE::PLAYER_OUT:
//			return dynamic_cast<PlayerOut*>(unpackedMessage);
//		}
		break;
	case MESSAGE_TYPE::PLAYER_CHAT_MESSAGE:
		break;
	case MESSAGE_TYPE::PLAYER_MOVEMENT:
		break;
	case MESSAGE_TYPE::ACK:
		break;
	case MESSAGE_TYPE::STATISTICS_MESSAGE:
		break;
	default:
		break;
	}
  */
	/*
	 switch msg{
	 case JOIN_ACK:
	 messages.push_back(new NickACK());
	 case WAIT:
	 messages.push_back(new WAIT());
	 default:
	 std::cout << "Undefined message error"  << std::endl;
	 }
	 */
//if(m_state == ConnectionState::CONNECTING)
//
	for (auto& it : messages) {
		it->Update();
	}
}
std::vector<ChatMessage*> InetConnection::getChatMessages() {
	std::vector<ChatMessage*> messages;
	for (auto& message : m_messages) {
		if (message->getMessageType() == MESSAGE_TYPE::PLAYER_CHAT_MESSAGE) {
			//messages.push_back(dynamic_cast<ChatMessage*>(message));
			//message = m_messages.end();
			//m_messages.pop_back();
		}
	}
	return messages;
}

std::vector<PlayerDead*> InetConnection::getDeadPayers() {
	std::vector<PlayerDead*> messages;
	for (auto& message : m_messages) {
		if (message->getMessageType() == MESSAGE_TYPE::GAME_MESSAGE) {

			if (dynamic_cast<GameMessage*>(message)->getGameMessageType() == GAME_MESSAGE_TYPE::PLAYER_DEAD) {
				messages.push_back(dynamic_cast<PlayerDead*>(message));
				//delete m_messages[it];
				//m_messages[it] = m_messages.back();
				//m_messages.pop_back();
			}
		}
	}
	return messages;
}
