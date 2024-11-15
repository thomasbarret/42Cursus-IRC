#include "../IRC.hpp"

void Nick(Server &server, Client &client, std::vector<std::string> tokens);
void User(Client &client, std::vector<std::string> tokens, std::string content);
void Pass(Client &client, std::vector<std::string> tokens);
void Join(Server &server, Client &client, std::vector<std::string> tokens);
void Msg(Server &server, Client &client, std::vector<std::string> tokens, std::string content);
void Part(Server &server, Client &client, std::vector<std::string> tokens);
void Mode(Server &server, Client &client, std::vector<std::string> tokens);
void Topic(Server &server, Client &client, std::vector<std::string> tokens, std::string content);
void Invite(Server &server, Client &client, std::vector<std::string> tokens);
void Kick(Server &server, Client &client, std::vector<std::string> tokens);


void ModeO(Server &server, Client &client, std::vector<std::string> tokens, Channel &channel);
void ModeT(Client &client, std::vector<std::string> tokens, Channel &channel);
void ModeI(Client &client, std::vector<std::string> tokens, Channel &channel);
void ModeK(Client &client, std::vector<std::string> tokens, Channel &channel);
void ModeL(Client &client, std::vector<std::string> tokens, Channel &channel);