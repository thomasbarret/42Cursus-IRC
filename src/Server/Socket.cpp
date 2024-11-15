#include "../../interface/IRC.hpp"

int Server::createSocket() {
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0)
		throw std::runtime_error("Socket creation failed");

	int opt = 1;

	//LINUX socket
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("Socket option failed");

	//MAC socket

	// if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
	// 	std::cerr << "setsockopt failed: " << strerror(errno) << std::endl;
	// 	throw std::runtime_error("Socket option failed");
	// }



	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(this->_port);

	if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
		throw std::runtime_error("Bind failed");

	if (listen(serverSocket, 3) < 0)
		throw std::runtime_error("Listen failed");

	return serverSocket;
}

void Server::setupSocketFd() {
	fcntl(this->_socket, F_SETFL, O_NONBLOCK);

	pollfd serverPollFd = {this->_socket, POLLIN, 0};
	this->_fds.push_back(serverPollFd);
}

void Server::acceptClient() {
	int clientSocket = accept(this->_socket, NULL, NULL);
	if (clientSocket < 0)
		throw std::runtime_error("Accept failed");

	int flags = fcntl(clientSocket, F_GETFL, 0);
	fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);

	pollfd clientPollFd = {clientSocket, POLLIN, 0};
	this->_fds.push_back(clientPollFd);

	Client *client = new Client(clientSocket);

	client->setPassword(this->_password);
	this->_clients.push_back(client);

	Server::logger("INFO", "[" + intToString(clientSocket) + "] New client connected");
}

void Server::handleClient(int client_fd) {
    char buffer[1024];
    int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_read < 0)
		return;
    if (bytes_read == 0) {
		Client* client = findClientByFd(client_fd);
		if (client)
			this->deleteClient(*client);
		else
			close(client_fd);
        return;
    }

    buffer[bytes_read] = '\0';
    std::string received_data(buffer);
        
    Client* client = findClientByFd(client_fd);
    if (!client) {
		Server::logger("ERROR","[" + intToString(client_fd) + "]" + "Client not found");        
		close(client_fd);
        return;
    }

	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	if (getpeername(client_fd, (struct sockaddr*)&addr, &addr_len) == -1) {
		if (client)
			this->deleteClient(*client);
		else 
			close(client_fd);
		return;
	}

	client->setAuth(addr);
    client->appendData(received_data);
    std::string data = client->getData();
    std::string delimiter1 = "\r\n";
    std::string delimiter2 = "\n";

    size_t pos = 0;
    while ((pos = data.find(delimiter1)) != std::string::npos ||
           (pos = data.find(delimiter2)) != std::string::npos) {
        std::string command = data.substr(0, pos);
        if (!command.empty()) {
            handleCommand(*client, command);
        }
        data.erase(0, pos + delimiter1.length());
    }

	if (findClientByFd(client_fd))
    	client->setData(data);
}

void Server::handleCommand(Client &client, std::string content) {
    std::vector<std::string> tokens = split(content, ' ');



	Server::logger("INFO", "[" + intToString(client.getFd()) + "] " + content);


    if (tokens.empty()) return;

    if (tokens[0] == "CAP") return;

    if (tokens[0] == "QUIT") {
        client.sendMessage("Goodbye!\r\n");
		this->deleteClient(client);
		return ;
    } else if (tokens[0] != "PASS" && !this->_password.empty() && !client.getLogged()) {
        client.sendMessage(":server 464 * :Password incorrect\r\n");
        return;
    }
	else if (tokens[0] == "NICK") {
        Nick(*this, client, tokens);
    } else if (tokens[0] == "USER") {
        User(client, tokens, content);
    } else if (tokens[0] == "PASS") {
        Pass(client, tokens);
    } else if (tokens[0] == "JOIN") {
        Join(*this, client, tokens);
    } else if (tokens[0] == "PART") {
        Part(*this, client, tokens);
    } else if (tokens[0] == "MSG" || tokens[0] == "PRIVMSG") {
        Msg(*this, client, tokens, content);
    } else if (tokens[0] == "PING") {
        client.sendMessage("PONG\r\n");
	} else if (tokens[0] == "MODE") {
		Mode(*this, client, tokens);
	} else if (tokens[0] == "TOPIC") {
		Topic(*this, client, tokens, content);
	} else if (tokens[0] == "INVITE") {
		Invite(*this, client, tokens);
	} else if (tokens[0] == "KICK") {
		Kick(*this, client, tokens);
    } else {
        client.sendMessage("421 :Unknown command\r\n");
    }
    client.checkRegistration();
}
