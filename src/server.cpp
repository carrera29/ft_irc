/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pollo <pollo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 14:57:27 by pollo             #+#    #+#             */
/*   Updated: 2025/08/05 15:57:12 by pollo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"
#include "../include/client.hpp"
#include "../include/channel.hpp"

Server::Server(std::string password, int port) : _password(password), _port(port) {
	_serverSocket = -1;
	memset(&_hints, 0, sizeof(_hints));
	_hints.ai_family = AF_INET;
	_hints.ai_socktype = SOCK_STREAM;
	_hints.ai_flags = AI_PASSIVE;

	std::cout << "Server created with password: " << _password << " on port: " << _port << std::endl;
}

Server::~Server() {
	std::cout << "Server disconnected" << std::endl;
}

std::string	Server::getPassword() {
	return _password;
}

std::string	Server::getPort() {
	return _port;
}

const int	Server::getSocket() {
	return _serverSocket;
}

struct addrinfo*	Server::getHints() {
	return &_hints;
}

void	Server::setSocket(int socket) {
	_serverSocket = socket;
}

inline size_t	Server::sizeofFds() {
	return fds.size();
}

class client*	Server::createClient(std::string nickname, std::string username, int socket) {
	client *newClient = new client(nickname, username, socket);
	clients[socket] = newClient;
	return newClient;
}

void	Server::removeClient(int socket) {
	if (clients.find(socket) != clients.end()) {
		delete clients[socket];
		clients.erase(socket);
	}
}

class client*	Server::getClient(int socket) {
	if (clients.find(socket) != clients.end()) {
		return clients[socket];
	}
	return NULL;
}

void	Server::handleJoin(const std::string& name, const std::string& topic, class client* creator) {
	if (name.empty() || topic.empty() || creator == NULL) {
		std::cerr << "Invalid channel parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == "#" + name) {
			channels[i]->addUser(creator);
			std::cout << "Channel " << name << " already exists, user added" << std::endl;
		}
		else if ((i + 1) == channels.size()) {
			channel *newChannel = new channel(name, topic, creator);
			channels.push_back(newChannel);
			channels[i]->addUser(creator);
			channels[i]->addOperator(creator);
		}
	}
}

void	Server::handleKick(const std::string& channel, class client* user) {
	if (channel.empty() || user == NULL) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == channel) {
			channels[i]->removeUser(user);
			std::cout << "User " << user->getNickname() << " kicked from channel " << channel << std::endl;
			return;
		}
	}
	std::cerr << "Channel " << channel << " not found" << std::endl;
}

void	Server::handleInvite(const std::string& channel, class client* user) {
	if (channel.empty() || user == NULL) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == channel) {
			channels[i]->addUser(user);
			std::cout << "User " << user->getNickname() << " was invited to join the channel " << channel << std::endl;
			return;
		}
	}
	std::cerr << "Channel " << channel << " not found" << std::endl;
}

void	Server::handleTopic(const std::string& channel, const std::string& topic) {
	if (channel.empty() || topic.empty()) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == channel) {
			channels[i]->setChannelTopic(topic);
			std::cout << "Topic for channel " << channel << " set to: " << topic << std::endl;
			return;
		}
	}
	std::cerr << "Channel " << channel << " not found" << std::endl;
}

void	Server::handleMode(const std::string& channel, const std::string& mode) {
	if (channel.empty() || mode.empty()) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == channel) {
			if (mode == "i") {
				channels[i]->setInviteOnlyChannel(!channels[i]->getInviteOnlyChannel());
				std::cout << "Invite-only mode for channel " << channel << " set to: " << channels[i]->getInviteOnlyChannel() << std::endl;
			}
			else if (mode == "t") {
				channels[i]->setRestrictedTopic(!channels[i]->getRestrictedTopic());
				std::cout << "Restricted topic mode for channel " << channel << " set to: " << channels[i]->getRestrictedTopic() << std::endl;
			}
			else if (mode == "k") {
				channels[i]->setChannelKeyRequired(!channels[i]->getChannelKeyRequired());
				std::cout << "Channel key required mode for channel " << channel << " set to: " << channels[i]->getChannelKeyRequired() << std::endl;
			}
			else if (mode == "o") {
				channels[i]->setChannelOperatorPrivilege(!channels[i]->getChannelOperatorPrivilege());
				std::cout << "Channel operator privilege mode for channel " << channel << " set to: " << channels[i]->getChannelOperatorPrivilege() << std::endl;
			}
			else if (mode == "l") {
				int maxUsers = channels[i]->getMaxUsers();
				if (maxUsers > 0) {
					channels[i]->setMaxUsers(0);
					std::cout << "Max users for channel " << channel << " set to unlimited" << std::endl;
				} else {
					channels[i]->setMaxUsers(100);
					std::cout << "Max users for channel " << channel << " set to 100" << std::endl;
				}
			}
			else {
				std::cerr << "Unknown mode: " << mode << std::endl;
			}
			return;
		}
	}
	std::cerr << "Channel " << channel << " not found" << std::endl;
}

