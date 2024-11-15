#include "../IRC.hpp"

class Client;

class Channel {
	private:
		std::string _name;
		std::string _topic;
		std::string _password;
		bool		_invite;
		bool 		_topicSet;
		bool 		_passwordSet;
		int			_limit;
		std::vector<Client *> _clients;
		std::vector<Client *> _operators;
		std::vector<Client *> _pendingInvites;
	public:
		Channel(const std::string& name);
		~Channel();

		const std::string& getName() const;
		const std::string& getTopic() const;
		void setTopic(const std::string& newTopic);
		bool isTopicSet() const;
		void setTopicSet(bool topicSet);

		void deletePassword();
		void setPassword(const std::string& password);
		const std::string& getPassword() const;
		bool isPasswordSet() const;

		int getLimit() const;
		void setLimit(int limit);


		void addMember(Client* client);
		void removeMember(Client* client);
		bool isMember(Client* client) const;
		int getMemberCount() const;

		void setInviteOnly(bool invite);
		bool isInviteOnly() const;


		bool isPendingInvite(Client* client) const;
		void addPendingInvite(Client* client);
		void removePendingInvite(Client* client);

		void addOperator(Client* client);
		void removeOperator(Client* client);
		bool isOperator(Client* client) const;

		void broadcastMessage(const std::string& message, Client* sender = NULL);
};