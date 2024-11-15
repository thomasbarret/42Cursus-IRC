#include "../../interface/IRC.hpp"

void cleanExit(int signal) {
	(void)signal;
	Server::stop = true;
	return ;
}