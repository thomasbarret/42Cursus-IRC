#include "../../interface/IRC.hpp"

void ModeO(Server &server, Client &client, std::vector<std::string> tokens, Channel &channel) {
		std::string channelName = channel.getName();

		if (tokens.size() < 4)
			return client.sendMessage("461 MODE :Not enough parameters\r\n"); // 461 ERR_NEEDMOREPARAMS
		Client *receiver = server.findClientByNickname(tokens[3]);
		if (!receiver)
			return client.sendMessage("401 " + tokens[3] + " :No such nick\r\n"); // 401 ERR_NOSUCHNICK
		if (!channel.isMember(receiver))
			return client.sendMessage("442 " + tokens[3] + " #" + channelName + " :User is not on that channel\r\n"); // 442 ERR_NOTONCHANNEL
		if (channel.isOperator(receiver))
			return client.sendMessage("443 " + tokens[3] + " #" + channelName + " :User is already operator\r\n"); // 443 ERR_USERONCHANNEL
		if (tokens[2] == "+o")
			channel.addOperator(receiver);
		else if (tokens[2] == "-o")
			channel.removeOperator(receiver);
		client.sendMessage(":" + client.getNickname() + " MODE #" + channelName + " " + tokens[2] + " " + receiver->getNickname() + "\r\n");
}

void ModeT(Client &client, std::vector<std::string> tokens, Channel &channel) {
		std::string channelName = channel.getName();

		if (!channel.isOperator(&client))
			return client.sendMessage("482 " + client.getNickname() + " #" + channelName + " :You're not channel operator\r\n"); // 482 ERR_CHANOPRIVSNEEDED
		channel.setTopicSet(tokens[2] == "-t");
		client.sendMessage(":" + client.getNickname() + " MODE #" + channelName + " " + tokens[2] + "\r\n");
}

void ModeI(Client &client, std::vector<std::string> tokens, Channel &channel) {
		std::string channelName = channel.getName();

		if (!channel.isOperator(&client))
			return client.sendMessage("482 " + client.getNickname() + " #" + channelName + " :You're not channel operator\r\n"); // 482 ERR_CHANOPRIVSNEEDED
		channel.setInviteOnly(tokens[2] == "+i");
		client.sendMessage(":" + client.getNickname() + " MODE #" + channelName + " " + tokens[2] + "\r\n");
}

void ModeK(Client &client, std::vector<std::string> tokens, Channel &channel) {
		std::string channelName = channel.getName();

		if (!channel.isOperator(&client))
			return client.sendMessage("482 " + client.getNickname() + " #" + channelName + " :You're not channel operator\r\n"); // 482 ERR_CHANOPRIVSNEEDED
		if (tokens.size() < 4 && tokens[2] == "+k")
			return client.sendMessage("461 MODE :Not enough parameters\r\n"); // 461 ERR_NEEDMOREPARAMS

		if (tokens[2] == "+k")
			channel.setPassword(tokens[3]);
		else 
			channel.deletePassword();

		client.sendMessage(":" + client.getNickname() + " MODE #" + channelName + " " + tokens[2] + " " + tokens[3] + "\r\n");
}

void ModeL(Client &client, std::vector<std::string> tokens, Channel &channel) {
		std::string channelName = channel.getName();

		if (!channel.isOperator(&client))
			return client.sendMessage("482 " + client.getNickname() + " #" + channelName + " :You're not channel operator\r\n"); // 482 ERR_CHANOPRIVSNEEDED
		if (tokens[2] == "+l" && tokens.size() < 4)
			return client.sendMessage("461 MODE :Not enough parameters\r\n"); // 461 ERR_NEEDMOREPARAMS
		if (tokens[2] == "+l" && atoi(tokens[3].c_str()) <= 0)
			return client.sendMessage("461 MODE :Not enough parameters\r\n"); // 461 ERR_NEEDMOREPARAMS
		
		if (tokens[2] == "+l")
			channel.setLimit(atoi(tokens[3].c_str()));
		else 
			channel.setLimit(0);

		client.sendMessage(":" + client.getNickname() + " MODE #" + channelName + " " + tokens[2] + " " + (tokens[2] == "+l" ? tokens[3] : "") + "\r\n");
}