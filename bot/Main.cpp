#include "interface/IRCbot.hpp"

void cleanExit(int signal) {
	(void)signal;
	Bot::stop = true;
}

int main(int argc, char **argv) {
	try {

		if (argc != 5)
			throw std::invalid_argument("Usage: ./bot [localhost] [port] [password] [channel]");
		signal(SIGINT, cleanExit);

		Bot bot("ToToRobot", argv[4], argv[3], argv[1], atoi(argv[2]));
		
		bot.start();
	} catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}