#include "../IRC.hpp"

class Client {
	private: 
		int _fd;
		bool _logged;
		bool _registered;
		std::string _nickname;
		std::string _username;
		std::string _realname;
		std::string _host;
		std::string _server;
		std::string _password;
		std::string _data;
		struct sockaddr_in _auth;
	public:
		Client(int fd);

		int getFd() const;
		bool getLogged() const;
		bool getRegistered() const;
		void checkRegistration();
		std::string getNickname() const;
		std::string getUsername() const;
		std::string getHostname() const;
		void setNickname(std::string nickname);
		void setHost(std::string host);
		void setUsername(std::string username);
		void setRealname(std::string realname);
		void setPassword(std::string password);
		void setServer(std::string server);
		void sendMessage(std::string message);
		void login(std::string password);

		void setAuth(struct sockaddr_in ip);
		struct sockaddr_in getAuth() const;

		void setData(const std::string& data);
		const std::string& getData() const;
		void appendData(const std::string& newData);
};