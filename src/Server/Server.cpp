#include "../../interface/IRC.hpp"

bool Server::stop = false; 


void Server::logger(std::string status, std::string message) {
	if (status == "INFO") {
		std::cout << "\033[1;34m[INFO]\033[0m " << message << std::endl;
	} else if (status == "CREATE") {
		std::cout << "\033[1;32m[CREATE]\033[0m " << message << std::endl;
	} else if (status == "DELETE") {
		std::cout << "\033[1;31m[DELETE]\033[0m " << message << std::endl;
	} else if (status == "ERROR") {
		std::cout << "\033[1;31m[ERROR]\033[0m " << message << std::endl;
	} else if (status == "IP") {
		std::cout << "\033[1;33m[IP]\033[0m " << message << std::endl;
	}
};

Server::Server(std::string port, std::string password) : _port(atoi(port.c_str())), _password(password) {
	if (this->_port < 0 || this->_port > 65535)
		throw std::invalid_argument("Invalid port (0-65535)");

	this->_socket = this->createSocket();
	this->setupSocketFd();
	std::cout << "\033[5;34m███████ ████████         ██ ██████   ██████ \033[0m" << std::endl;
	std::cout << "\033[5;34m██         ██            ██ ██   ██ ██      \033[0m" << std::endl;
	std::cout << "\033[5;34m█████      ██            ██ ██████  ██      \033[0m" << std::endl;
	std::cout << "\033[5;34m██         ██            ██ ██   ██ ██      \033[0m" << std::endl;
	std::cout << "\033[5;34m██         ██    ███████ ██ ██   ██  ██████ \033[0m" << std::endl;
	std::cout << "\033[5;34m                                            \033[0m" << std::endl;
	std::cout << "\033[5;34m                                            \033[0m" << std::endl;


	Server::logger("INFO", "Server started on port " + port + " with password " + password);
};


void Server::start() {
	while (1) {
		int fd = poll(this->_fds.data(), this->_fds.size(), -1);
		if (fd == -1)
			throw std::runtime_error("Poll Exited");
		for (size_t i = 0; i < this->_fds.size(); i++) {
			if (!(this->_fds[i].revents & POLLIN)) continue;
			if (this->_fds[i].fd == this->_socket) {
				this->acceptClient();
			} else {
				this->handleClient(this->_fds[i].fd);
			}
		}
		if (Server::stop) {
			std::cout << "Server stopped" << std::endl;
			break;
		};
	}
};

Client* Server::findClientByFd(int fd) {
	std::vector<Client *>::iterator it;

	for (it = this->_clients.begin(); it != this->_clients.end(); ++it) {
		if ((*it)->getFd() == fd) {
			return *it;
		}
	}
	return NULL;
}


Client *Server::findClientByNickname(std::string nickname) {
	std::vector<Client *>::iterator it;

	for (it = this->_clients.begin(); it != this->_clients.end(); ++it) {
		if (toLowerCase((*it)->getNickname()) == toLowerCase(nickname)) {
			return *it;
		}
	}
	return NULL;
};


Channel* Server::findChannelByName(std::string name) {
	std::map<std::string, Channel *>::iterator it;

	it = this->_channels.find(name);
	if (it != this->_channels.end()) {
		return it->second;
	}
	return NULL;
};

std::vector<std::string> Server::findChannelsByClient(Client &client) {
    std::vector<std::string> channelsList;
    std::map<std::string, Channel *>::iterator it;

    for (it = this->_channels.begin(); it != this->_channels.end(); ++it) {
        if (it->second->isMember(&client) || it->second->isPendingInvite(&client)) {
            channelsList.push_back(it->first);
        }
    }

    return channelsList;
}


Channel* Server::createChannel(std::string name) {
	Channel *channel = new Channel(name);
	this->_channels.insert(std::pair<std::string, Channel *>(name, channel));
	Server::logger("CREATE", "Channel " + name + " created");
	return channel;
};

void Server::deleteClient(Client &client) {
	std::vector<Client *>::iterator it;
	for (it = this->_clients.begin(); it != this->_clients.end(); ++it) {
		if ((*it)->getFd() == client.getFd()) {
			this->quitClientChannels(client);
			Server::logger("DELETE", "Client " + (client.getNickname().empty() ? intToString(client.getFd()) : client.getNickname()) + " disconnected");
			close(client.getFd());
			delete (*it);
			this->_clients.erase(it);
			break;
		}
	}
};

void Server::deleteChannel(Channel &channel) {
	std::map<std::string, Channel *>::iterator it;
	it = this->_channels.find(channel.getName());
	if (it != this->_channels.end()) {
		Server::logger("DELETE", "Channel " + channel.getName() + " deleted");
		delete it->second;
		this->_channels.erase(it);
	}
};

void Server::quitClientChannels(Client &client) {
	std::vector<std::string> channels = this->findChannelsByClient(client);
	if (channels.empty()) return;
	
	std::vector<std::string>::iterator it;
	for (it = channels.begin(); it != channels.end(); ++it) {
		Channel *channel = this->findChannelByName(*it);
		if (channel) {
			if (channel->isMember(&client))
				channel->removeMember(&client);
		
			if (channel->isPendingInvite(&client))
				channel->removePendingInvite(&client);
		
			if (channel->getMemberCount() <= 0) {
				std::map<std::string, Channel *>::iterator it = this->_channels.find(channel->getName());

				if (it != this->_channels.end()) {
					this->deleteChannel(*channel);
				}
			} 
		}
	}
};

Server::~Server() {
	for (size_t i = 0; i < this->_clients.size(); i++) {
		this->_clients[i]->sendMessage("ERROR : Server closed");
		close(this->_clients[i]->getFd());
		delete this->_clients[i];
	}

	for (std::map<std::string, Channel *>::iterator it = this->_channels.begin(); it != this->_channels.end(); ++it)
		delete it->second;
	close(this->_socket);
};