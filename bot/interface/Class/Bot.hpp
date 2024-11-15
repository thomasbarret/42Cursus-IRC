#include "../IRCbot.hpp"


class Bot {
	private:
		std::string _nickname;
		std::string _channel;
		std::string _password;
		std::string _server;
		int _port;

		int _socket;
		struct sockaddr_in _addr;
	public:
		static bool stop;

		Bot(std::string nickname, std::string channel, std::string password, std::string server, int port);
		~Bot();
		void start();
		void sendMessage(std::string message);
		void handleMessage(std::string message);
};