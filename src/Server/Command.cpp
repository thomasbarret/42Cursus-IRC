#include "../../interface/IRC.hpp"

void Nick(Server &server, Client &client, std::vector<std::string> tokens) {

	if (tokens.size() < 2)
		return client.sendMessage("431 :No nickname given\r\n"); // 431 ERR_NONICKNAMEGIVEN
	Client *findUser = server.findClientByNickname(tokens[1]);
	if (findUser && findUser->getFd() != client.getFd())
		return client.sendMessage("433 " + tokens[1] + " :Nickname is already in use\r\n"); // 433 ERR_NICKNAMEINUSE
	if (tokens[1].find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_") != std::string::npos)
		return client.sendMessage("432 " + tokens[1] + " :Erroneous nickname\r\n");
	client.setNickname(tokens[1]);
};

void User(Client &client, std::vector<std::string> tokens, std::string content) {
	if (tokens.size() < 5)
		return client.sendMessage("461 USER :Not enough parameters\r\n"); // 461 ERR_NEEDMOREPARAMS
	client.setUsername(tokens[1]);
	client.setRealname(tokens[4]);
	client.setHost(tokens[2]);
	client.setServer(tokens[3]);
	std::vector<std::string> realname = split(content, ':');
	if (realname.size() < 2)
		return client.sendMessage("461 USER :Not enough parameters\r\n"); // 461 ERR_NEEDMOREPARAMS
	client.setRealname(realname[1]);
};

void Pass(Client &client, std::vector<std::string> tokens) {
	if (tokens.size() < 2)
		return client.sendMessage("461 PASS :Not enough parameters\r\n"); // 461 ERR_NEEDMOREPARAMS
	if (client.getLogged())
		return client.sendMessage(":server 464 * :Password incorrect\r\n"); // 462 ERR_ALREADYREGISTERED
	client.login(tokens[1]);
}

void Join(Server &server, Client &client, std::vector<std::string> tokens) {
	if (tokens.size() < 2)
		return client.sendMessage("461 JOIN :Not enough parameters\r\n"); // 461 ERR_NEEDMOREPARAMS
	if (!client.getRegistered())
		return client.sendMessage("451 JOIN :You have not registered\r\n"); // 451 ERR_NOTREGISTERED
	if (tokens[1][0] != '#' && tokens[1][0] != '&')
		return client.sendMessage("476 :Invalid channel name\r\n"); // 476 ERR_BADCHANMASK

	std::string channelName = tokens[1].erase(0, 1);
	if (channelName.empty())
		return client.sendMessage("476 :Invalid channel name\r\n"); // 476 ERR_BADCHANMASK

	Channel *channel = server.findChannelByName(channelName);

	if (!channel) {
		channel = server.createChannel(channelName);
		channel->addOperator(&client);
	}

	if (channel->isMember(&client) && !channel->isPendingInvite(&client))
		return client.sendMessage("443 " + client.getNickname() + " #" + channelName + " :You're already in this channel\r\n"); // 443 ERR_USERONCHANNEL

	if (channel->isPasswordSet() && !channel->isPendingInvite(&client)) {
		if (tokens.size() < 3)
			return client.sendMessage("475 " + client.getNickname() + " #" + channelName + " :Password required\r\n"); // 475 ERR_BADCHANNELKEY
		if (tokens[2] != channel->getPassword())
			return client.sendMessage("475 " + client.getNickname() + " #" + channelName + " :Password required\r\n"); // 475 ERR_BADCHANNELKEY
	}
	
	if (channel->isInviteOnly() && !channel->isPendingInvite(&client))
		return client.sendMessage("473 " + client.getNickname() + " #" + channelName + " :You're not invited to this channel\r\n"); // 473 ERR_INVITEONLYCHAN

	if (channel->getMemberCount() >= channel->getLimit() && channel->getLimit() != 0)
		return client.sendMessage("471 " + client.getNickname() + " #" + channelName + " :Cannot join channel (+l)\r\n"); // 471 ERR_CHANNELISFULL

	if (channel->isPendingInvite(&client))
		channel->removePendingInvite(&client);

	channel->addMember(&client);

	client.sendMessage(":" + client.getNickname() + " JOIN #" + channelName + "\r\n");
	channel->broadcastMessage(":" + client.getNickname() + "@" + inet_ntoa(client.getAuth().sin_addr) + " JOIN #" + channelName + "\r\n", &client);
}

void Msg(Server &server, Client &client, std::vector<std::string> tokens, std::string content) {
	if (tokens.size() < 3)
		return client.sendMessage("461 PRIVMSG :Not enough parameters\r\n"); // 461 ERR_NEEDMOREPARAMS
	if (!client.getRegistered())
		return client.sendMessage("451 PRIVMSG :You have not registered\r\n"); // 451 ERR_NOTREGISTERED
	if (tokens[1][0] != '#' && tokens[1][0] != '&') {
		Client *receiver = server.findClientByNickname(tokens[1]);
		if (!receiver)
			return client.sendMessage("401 " + tokens[1] + " :No such nick\r\n"); // 401 ERR_NOSUCHNICK
		if (tokens[2][0] != ':')
			return client.sendMessage("412 :No text to send\r\n"); // 412 ERR_NOTEXTTOSEND

		std::vector<std::string> message = split(content, ':');
		if (message.size() < 2)
			return client.sendMessage("412 :No text to send\r\n"); // 412 ERR_NOTEXTTOSEND

		receiver->sendMessage(":" + client.getNickname() + "@" + inet_ntoa(client.getAuth().sin_addr) + " PRIVMSG " + tokens[1] + " :" + message[1] + "\r\n");
	} else {
		std::string channelName = tokens[1].substr(1); // Remove # or &
		Channel *channel = server.findChannelByName(channelName);

		if (!channel)
			return client.sendMessage("403 " + client.getNickname() + " #" + channelName + " :No such channel\r\n"); // 403 ERR_NOSUCHCHANNEL
		if (!channel->isMember(&client))
			return client.sendMessage("442 " + client.getNickname() + " #" + channelName + " :You're not on that channel\r\n"); // 442 ERR_NOTONCHANNEL
		if (tokens[2][0] != ':')
			return client.sendMessage("412 :No text to send\r\n"); // 412 ERR_NOTEXTTOSEND

		std::vector<std::string> message = split(content, ':');
		if (message.size() < 2)
			return client.sendMessage("412 :No text to send\r\n"); // 412 ERR_NOTEXTTOSEND

		channel->broadcastMessage(":" + client.getNickname() + "@" + inet_ntoa(client.getAuth().sin_addr) + " PRIVMSG #" + channelName + " :" + message[1] + "\r\n", &client);
	}
}

void Part(Server &server, Client &client, std::vector<std::string> tokens) {
	if (tokens.size() < 2)
		return client.sendMessage("461 PART :Not enough parameters\r\n"); // 461 ERR_NEEDMOREPARAMS
	if (!client.getRegistered())
		return client.sendMessage("451 PART :You have not registered\r\n"); // 451 ERR_NOTREGISTERED
	if (tokens[1][0] != '#' && tokens[1][0] != '&')
		return client.sendMessage("476 :Invalid channel name\r\n"); // 476 ERR_BADCHANMASK

	std::string channelName = tokens[1].erase(0, 1);
	Channel *channel = server.findChannelByName(channelName);

	if (!channel)
		return client.sendMessage("403 " + client.getNickname() + " #" + channelName + " :No such channel\r\n"); // 403 ERR_NOSUCHCHANNEL
	if (!channel->isMember(&client))
		return client.sendMessage("442 " + client.getNickname() + " #" + channelName + " :You're not on that channel\r\n"); // 442 ERR_NOTONCHANNEL

	channel->removeMember(&client);

	client.sendMessage(":" + client.getNickname() + " PART #" + channelName + "\r\n");
	channel->broadcastMessage(":" + client.getNickname() + "@" + inet_ntoa(client.getAuth().sin_addr) + " PART #" + channelName + "\r\n", &client);
	if (channel->getMemberCount() <= 0) {
		server.deleteChannel(*channel);
	}
}

void Mode(Server &server, Client &client, std::vector<std::string> tokens) {
	if (tokens.size() < 3)
		return client.sendMessage("461 MODE :Not enough parameters\r\n"); // 461 ERR_NEEDMOREPARAMS
	if (!client.getRegistered())
		return client.sendMessage("451 MODE :You have not registered\r\n"); // 451 ERR_NOTREGISTERED
	if (tokens[1][0] != '#' && tokens[1][0] != '&')
		return;

	std::string channelName = tokens[1].erase(0, 1);
	Channel *channel = server.findChannelByName(channelName);

	if (!channel)
		return client.sendMessage("403 " + client.getNickname() + " #" + channelName + " :No such channel\r\n"); // 403 ERR_NOSUCHCHANNEL
	if (!channel->isMember(&client))
		return client.sendMessage("442 " + client.getNickname() + " #" + channelName + " :You're not on that channel\r\n"); // 442 ERR_NOTONCHANNEL

	if (tokens[2] == "+o" || tokens[2] == "-o") {
		ModeO(server, client, tokens, *channel);
	} else if (tokens[2] == "+t" || tokens[2] == "-t") {
		ModeT(client, tokens, *channel);
	} else if (tokens[2] == "+i" || tokens[2] == "-i") {
		ModeI(client, tokens, *channel);
	} else if (tokens[2] == "+k" || tokens[2] == "-k") {
		ModeK(client, tokens, *channel);
	} else if (tokens[2] == "+l" || tokens[2] == "-l") {
		ModeL(client, tokens, *channel);
	} else {
		client.sendMessage("472 " + client.getNickname() + " #" + channelName + " :Unknown MODE flag\r\n"); // 472 ERR_UNKNOWNMODE
	}
}

void Topic(Server &server, Client &client, std::vector<std::string> tokens, std::string content) {
	if (tokens.size() < 2)
		return client.sendMessage("461 TOPIC :Not enough parameters\r\n"); // 461 ERR_NEEDMOREPARAMS
	if (!client.getRegistered())
		return client.sendMessage("451 TOPIC :You have not registered\r\n"); // 451 ERR_NOTREGISTERED
	if (tokens[1][0] != '#' && tokens[1][0] != '&')
		return client.sendMessage("476 :Invalid channel name\r\n"); // 476 ERR_BADCHANMASK

	std::string channelName = tokens[1].substr(1); 
	Channel *channel = server.findChannelByName(channelName);

	if (!channel)
		return client.sendMessage("403 " + client.getNickname() + " #" + channelName + " :No such channel\r\n"); // 403 ERR_NOSUCHCHANNEL
	if (!channel->isMember(&client))
		return client.sendMessage("442 " + client.getNickname() + " #" + channelName + " :You're not on that channel\r\n"); // 442 ERR_NOTONCHANNEL
	
	if (tokens.size() == 2) {
		client.sendMessage("332 " + client.getNickname() + " #" + channelName + " :" + channel->getTopic() + "\r\n");
	} else {
		std::vector<std::string> topic = split(content, ':');
		if (topic.size() < 2)
			return client.sendMessage("332 " + client.getNickname() + " #" + channelName + " :" + channel->getTopic() + "\r\n");
		if (!channel->isTopicSet() && !channel->isOperator(&client))
			return client.sendMessage("482 " + client.getNickname() + " #" + channelName + " :You're not channel operator\r\n"); // 482 ERR_CHANOPRIVSNEEDED
		
		channel->setTopic(topic[1]);
		client.sendMessage(":" + client.getNickname() + " TOPIC #" + channelName + " :" + topic[1] + "\r\n");
		channel->broadcastMessage(":" + client.getNickname() + " TOPIC #" + channelName + " :" + topic[1] + "\r\n", &client);
	}
}


void Invite(Server &server, Client &client, std::vector<std::string> tokens) {
    std::cout << "Invite command" << std::endl;

    if (tokens.size() < 3)
        return client.sendMessage("461 INVITE :Not enough parameters\r\n"); // 461 ERR_NEEDMOREPARAMS


    if (!client.getRegistered())
        return client.sendMessage("451 INVITE :You have not registered\r\n"); // 451 ERR_NOTREGISTERED

    if (tokens[2][0] != '#' && tokens[2][0] != '&')
        return client.sendMessage("476 " + tokens[2] + " :Invalid channel name\r\n"); // 476 ERR_BADCHANMASK


    std::string channelName = tokens[2].substr(1);
    Channel *channel = server.findChannelByName(channelName);

    if (!channel)
        return client.sendMessage("403 " + client.getNickname() + " #" + channelName + " :No such channel\r\n"); // 403 ERR_NOSUCHCHANNEL


    if (!channel->isMember(&client)){
        return client.sendMessage("442 " + client.getNickname() + " #" + channelName + " :You're not on that channel\r\n"); // 442 ERR_NOTONCHANNEL
	}

	if (!channel->isOperator(&client)){
		return client.sendMessage("482 " + client.getNickname() + " #" + channelName + " :You're not channel operator\r\n"); // 482 ERR_CHANOPRIVSNEEDED
	}

	std::cout << "401 " + client.getNickname() + " #" + tokens[1] + " :No such nick/channel\r\n" << tokens[1] << std::endl;
	
    Client *receiver = server.findClientByNickname(tokens[1]);
    if (!receiver) {
        return client.sendMessage("401 " + client.getNickname() + " #" + channelName + " :No such nick/channel\r\n"); // 401 ERR_NOSUCHNICK
	}

	if (channel->isMember(receiver)) {
    	return client.sendMessage("443 " + client.getNickname() + " #" + channelName + " :is already on channel\r\n"); // 443 ERR_USERONCHANNEL
	}

	if (!channel->isPendingInvite(receiver)) {
    	channel->addPendingInvite(receiver);
	}
    
    receiver->sendMessage(":" + client.getNickname() + " INVITE " + receiver->getNickname() + " #" + channelName + "\r\n");

    client.sendMessage("341 " + client.getNickname() + " " + receiver->getNickname() + " #" + channelName + "\r\n"); // 341 RPL_INVITING
}

void Kick(Server &server, Client &client, std::vector<std::string> tokens) {
	if (tokens.size() < 3)
		return client.sendMessage("461 KICK :Not enough parameters\r\n"); // 461 ERR_NEEDMOREPARAMS

	if (!client.getRegistered())
		return client.sendMessage("451 KICK :You have not registered\r\n"); // 451 ERR_NOTREGISTERED

	if (tokens[1][0] != '#' && tokens[1][0] != '&')
		return client.sendMessage("476 " + tokens[1] + " :Invalid channel name\r\n"); // 476 ERR_BADCHANMASK

	std::string channelName = tokens[1].substr(1);
	Channel *channel = server.findChannelByName(channelName);

	if (!channel)
		return client.sendMessage("403 " + client.getNickname() + " #" + channelName + " :No such channel\r\n"); // 403 ERR_NOSUCHCHANNEL

	if (!channel->isMember(&client))
		return client.sendMessage("442 " + client.getNickname() + " #" + channelName + " :You're not on that channel\r\n"); // 442 ERR_NOTONCHANNEL

	if (!channel->isOperator(&client))
		return client.sendMessage("482 " + client.getNickname() + " #" + channelName + " :You're not channel operator\r\n"); // 482 ERR_CHANOPRIVSNEEDED

	Client *receiver = server.findClientByNickname(tokens[2]);
	if (!receiver)
		return client.sendMessage("401 " + tokens[2] + " :No such nick\r\n"); // 401 ERR_NOSUCHNICK
	
	if (!channel->isMember(receiver))
		return client.sendMessage("441 " + tokens[2] + " #" + channelName + " :User is not on that channel\r\n"); // 441 ERR_NOTONCHANNEL
	
	channel->removeMember(receiver);
	client.sendMessage(":" + client.getNickname() + " KICK #" + channelName + " " + receiver->getNickname() + "\r\n");
	receiver->sendMessage(":" + client.getNickname() + " KICK #" + channelName + " " + receiver->getNickname() + "\r\n");
}