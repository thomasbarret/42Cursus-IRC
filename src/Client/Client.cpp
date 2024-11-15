#include "../../interface/IRC.hpp"


Client::Client(int fd) : _fd(fd), _logged(false), _registered(false), _nickname(""), _username(""), _realname(""), _host(""), _server(""), _password(""), _data("") {};

int Client::getFd() const {
	return this->_fd;
};

bool Client::getLogged() const {
	if (this->_password.empty())
		return true;
	return this->_logged;
};

bool Client::getRegistered() const {
	if (!this->_nickname.empty() && !this->_username.empty() && !this->_realname.empty() && (this->_logged || this->_password.empty()))
		return true;
	return false;
};

void Client::checkRegistration() {
	if (this->getRegistered() && !this->_registered) {
		Server::logger("IP", "[" +  intToString(this->_fd) + "]" + " Client " + this->_nickname + " registered with IP " + inet_ntoa(this->_auth.sin_addr));
		Server::logger("CREATE", "[" +  intToString(this->_fd) + "]" + " Client " + this->_nickname + " registered");
		this->_registered = true;
		this->_logged = true;
		this->sendMessage(":localhost 001 " + this->_nickname + " :Welcome to the Internet Relay Network " + this->_nickname + "\r\n");
	}
};

std::string Client::getNickname() const {
	return this->_nickname;
};

std::string Client::getUsername() const {
	return this->_username;
};

std::string Client::getHostname() const {
	return this->_host;
};

void Client::setNickname(std::string nickname) {
	this->_nickname = nickname;
};

void Client::setHost(std::string host) {
	this->_host = host;
};

void Client::setUsername(std::string username) {
	this->_username = username;
};

void Client::setRealname(std::string realname) {
	this->_realname = realname;
};

void Client::setServer(std::string server) {
	this->_server = server;
};

void Client::setPassword(std::string password) {
	this->_password = password;
};

void Client::setAuth(struct sockaddr_in auth) {
	this->_auth = auth;
};

struct sockaddr_in Client::getAuth() const {
	return this->_auth;
};

void Client::sendMessage(std::string message) {
	send(this->_fd, message.c_str(), message.size(), 0);
};

void Client::login(std::string password) {
    if (this->_password == password) {
        this->_logged = true;
        Server::logger("INFO", "[" + intToString(this->_fd) + "] Client authenticated");
    } else {
        this->sendMessage("464 :Password incorrect\r\n");
        Server::logger("WARNING", "[" + intToString(this->_fd) + "] Client failed to authenticate");
    }
}

void Client::setData(const std::string& data) {
	this->_data = data; 
}

const std::string& Client::getData() const {
    return this->_data;
}

void Client::appendData(const std::string& newData) {
    this->_data += newData;
}
