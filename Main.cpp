#include "interface/IRC.hpp"

int main(int argc, char **argv) {
	try {
		if (argc < 3)
			throw std::invalid_argument("Usage: ./ircserv [port] <password>");
		signal(SIGINT, cleanExit);
		
		Server server(argv[1], argv[2] ? argv[2] : "");

		server.start();
	} catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}