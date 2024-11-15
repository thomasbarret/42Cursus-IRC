#include "../IRC.hpp"

class Client;
class Channel;

class Server {
	private:
		int 		_socket;
		int 		_port;
		std::string _password;
		std::vector<pollfd> _fds;
		std::vector<Client *> _clients;
		std::map<std::string, Channel *> _channels;
		
		int createSocket();
		void setupSocketFd();
		void acceptClient();
		void deleteClient(Client &client);
		void handleClient(int fd);
		void handleCommand(Client &client, std::string content);
		Client* findClientByFd(int fd);
	public:
		static bool stop;
		static void logger(std::string status, std::string message);


		void start();
		Channel* findChannelByName(std::string name);
		std::vector<std::string> findChannelsByClient(Client &client);
		Channel* createChannel(std::string name);
		void deleteChannel(Channel &channel);
		void quitClientChannels(Client &client);
		Client* findClientByNickname(std::string nickname);
		Server(std::string port, std::string password);
		~Server();
};