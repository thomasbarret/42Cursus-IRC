#include "../../interface/IRCbot.hpp"

bool Bot::stop = false;

Bot::Bot(std::string nickname, std::string channel, std::string password, std::string server, int port) : _nickname(nickname), _channel(channel), _password(password), _server(server), _port(port) {
	this->_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == -1) {
		std::runtime_error("Error: socket creation failed");
	}

	this->_addr.sin_family = AF_INET;
	this->_addr.sin_port = htons(this->_port);
	this->_addr.sin_addr.s_addr = inet_addr(server == "localhost" ? "127.0.0.1" : server.c_str());

	if (connect(this->_socket, (struct sockaddr*)&this->_addr, sizeof(this->_addr)) == -1) {
		std::runtime_error("Error: connection failed");
	}


	sendMessage("PASS " + password);
	sendMessage("NICK " + nickname);
	sendMessage("USER " + nickname + " 0 * :" + nickname);
	sendMessage("JOIN " + channel);
}

void Bot::start() {
	struct pollfd fds[1];
	fds[0].fd = this->_socket;
	fds[0].events = POLLIN;

	std::cout << "Bot started" << std::endl;

	while (1) {
		if (Bot::stop) {
			break;
		}
		int ret = poll(fds, 1, -1);
		if (ret > 0) {
			if (fds[0].revents & POLLIN) {
				char buffer[4096];

				int received = recv(this->_socket, buffer, sizeof(buffer) - 1, 0);

				buffer[received] = '\0';
				if (received > 0) {
					std::cout << "Received: " << buffer << std::endl;
					this->handleMessage(buffer);
				} else if (received == 0) {
					std::cout << "Disconnected from server" << std::endl;
					break;
				}
			}
		}
	}
}

void Bot::handleMessage(std::string message) {
	if (message.find("hello") != std::string::npos) {
		sendMessage("PRIVMSG " + this->_channel + " :Hey!");
	}
}


void Bot::sendMessage(std::string message) {
	std::string buffer = message + "\r\n";
	send(this->_socket, buffer.c_str(), buffer.size(), 0);
}

Bot::~Bot() {
	this->sendMessage("QUIT :Bye!");
	close(this->_socket);
	std::cout << "Bot destroyed" << std::endl;
}