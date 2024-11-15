#include "../../interface/IRC.hpp"
 
Channel::Channel(const std::string& name) : _name(name), _invite(false), _topicSet(true), _passwordSet(false), _limit(0) {};

const std::string& Channel::getName() const {
	return this->_name;
};

const std::string& Channel::getTopic() const {
	return this->_topic;
};

void Channel::setPassword(const std::string& password) {
	this->_password = password;
	this->_passwordSet = true;
};

bool Channel::isPasswordSet() const {
	return this->_passwordSet;
};

void Channel::deletePassword() {
	this->_password = "";
	this->_passwordSet = false;
};

const std::string& Channel::getPassword() const {
	return this->_password;
};

int Channel::getLimit() const {
	return this->_limit;
};

void Channel::setLimit(int limit) {
	this->_limit = limit;
};

void Channel::setTopic(const std::string& newTopic) {
	this->_topic = newTopic;
};

bool Channel::isTopicSet() const {
	return this->_topicSet;
};

void Channel::setTopicSet(bool topicSet) {
	this->_topicSet = topicSet;
};

void Channel::addMember(Client* client) {
	this->_clients.push_back(client);
};

void Channel::removeMember(Client* client) {
    for (std::vector<Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); ) {
        if ((*it)->getFd() == client->getFd()) {
			Server::logger("DELETE", "Client " + (*it)->getNickname() + " removed from channel " + this->_name);
			if (this->isOperator(client)) 
				this->removeOperator(client);
			if (this->isPendingInvite(client))
				this->removePendingInvite(client);
            it = this->_clients.erase(it); 
            break; 
        } else {
            ++it; 
        }
    }
}


int Channel::getMemberCount() const {
	return this->_clients.size();
};

bool Channel::isMember(Client* client) const {
	for (size_t i = 0; i < this->_clients.size(); i++) {
		if (this->_clients[i]->getFd() == client->getFd()) {
			return true;
		}
	}
	return false;
};

bool Channel::isPendingInvite(Client* client) const {
	return std::find(this->_pendingInvites.begin(), this->_pendingInvites.end(), client) != this->_pendingInvites.end();
};

void Channel::addPendingInvite(Client* client) {
	if (this->isPendingInvite(client)) return ;
	this->_pendingInvites.push_back(client);
};

void Channel::removePendingInvite(Client* client) {
	for (std::vector<Client *>::iterator it = this->_pendingInvites.begin(); it != this->_pendingInvites.end(); ) {
		if ((*it)->getFd() == client->getFd()) {
			it = this->_pendingInvites.erase(it);
			break;
		} else {
			++it;
		}
	}
};

void Channel::addOperator(Client* client) {
	this->_operators.push_back(client);
};

void Channel::removeOperator(Client* client) {
	this->_operators.erase(std::remove(this->_operators.begin(), this->_operators.end(), client), this->_operators.end());
};

bool Channel::isOperator(Client* client) const {
	for (size_t i = 0; i < this->_operators.size(); i++) {
		if (this->_operators[i]->getFd() == client->getFd()) {
			return true;
		}
	}
	return false;
};

bool Channel::isInviteOnly() const {
	return this->_invite;
};

void Channel::setInviteOnly(bool invite) {
	this->_invite = invite;
};

void Channel::broadcastMessage(const std::string& message, Client* sender) {
	std::vector<Client*>::iterator it;

	if (!sender)
		return ;

	for (it = this->_clients.begin(); it != this->_clients.end(); ++it) {
		if ((*it)->getFd() == sender->getFd()) continue;
		Server::logger("CREATE", "Message from " + sender->getNickname() + "sent to " + (*it)->getNickname() + " on channel " + this->_name);
		(*it)->sendMessage(message);
	}
};

Channel::~Channel() {};