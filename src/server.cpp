/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pollo <pollo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 14:57:27 by pollo             #+#    #+#             */
/*   Updated: 2025/08/21 15:06:20 by pollo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"
#include "../include/client.hpp"
#include "../include/channel.hpp"

server::server(std::string password, std::string port) : _password(password), _port(port) {

	memset(&_hints, 0, sizeof(_hints));
	_hints.ai_family = AF_INET;
	_hints.ai_socktype = SOCK_STREAM;
	_hints.ai_flags = AI_PASSIVE;

	std::cout << "Server created with password: " << _password << " on port: " << _port << std::endl;
}

server::~server() {
	std::cout << "Server disconnected" << std::endl;
}

std::string	server::getPassword() {
	return _password;
}

std::string	server::getPort() {
	return _port;
}

int	server::getSocket() {
	return _serverSocket;
}

struct addrinfo*	server::getHints() {
	return &_hints;
}

void	server::setSocket(int socket) {
	_serverSocket = socket;
}

inline size_t	server::sizeofFds() {
	return fds.size();
}

class client*	server::createClient(std::string nickname, std::string username, int socket) {
	client *newClient = new client(nickname, username, socket);
	clients[socket] = newClient;
	return newClient;
}

void	server::removeClient(int socket) {
	if (clients.find(socket) != clients.end()) {
		delete clients[socket];
		clients.erase(socket);
	}
}

class client*	server::getClient(int socket) {
	if (clients.find(socket) != clients.end()) {
		return clients[socket];
	}
	return NULL;
}

void	server::handleJoin(const std::string& name, const std::string& topic, class client* creator) {
	if (name.empty() || topic.empty() || creator == NULL) {
		std::cerr << "Invalid channel parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == "#" + name) {
			std::cout << "Channel " << channels[i]->getChannelName() << " already exists" << std::endl;
			channels[i]->addUser(creator);
			return;
		}
	}
	channel *newChannel = new channel(name, topic, creator);
	channels.push_back(newChannel);
	channels[channels.size() - 1]->addUser(creator);
	channels[channels.size() - 1]->addOperator(creator);
}

void	server::handleKick(const std::string& channel, class client* user) {
	if (channel.empty() || user == NULL) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == channel) {
			channels[i]->removeUser(user);
			return;
		}
	}
	std::cerr << "Channel " << channel << " not found" << std::endl;
}

void	server::handleInvite(const std::string& channel, class client* user) {
	if (channel.empty() || user == NULL) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == channel) {
			channels[i]->addUser(user);
			return;
		}
	}
	std::cerr << "Channel " << channel << " not found" << std::endl;
}

void	server::handleTopic(const std::string& channel, const std::string& topic) {
	if (channel.empty() || topic.empty()) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == "#"+channel) {
			channels[i]->setChannelTopic(topic);
			std::cout << "Topic for channel " << channel << " set to: " << topic << std::endl;
			return;
		}
	}
	std::cerr << "Channel " << channel << " not found" << std::endl;
}

void	server::handleMode(const std::string& channel, const std::string& mode, bool enable) {
	if (channel.empty() || mode.empty()) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == channel) {
			if (mode == "i") {
				if (channels[i]->getInviteOnlyChannel() != enable) {
					channels[i]->setInviteOnlyChannel(enable);
				}
				std::cout << "Invite-only mode for channel " << channel << " set to: " << (enable ? "true" : "false") << std::endl;
			}
			else if (mode == "t") {
				if (channels[i]->getRestrictedTopic() != enable) {
					channels[i]->setRestrictedTopic(enable);
				}
				std::cout << "Restricted topic mode for channel " << channel << " set to: " << (enable ? "true" : "false") << std::endl;
			}
			else if (mode == "k") {
				if (channels[i]->getChannelKeyRequired() != enable) {
					channels[i]->setChannelKeyRequired(enable);
				}
				std::cout << "Channel key required mode for channel " << channel << " set to: " << (enable ? "true" : "false") << std::endl;
			}
			else if (mode == "o") {
				if (channels[i]->getChannelOperatorPrivilege() != enable) {
					channels[i]->setChannelOperatorPrivilege(enable);
				}
				std::cout << "Channel operator privilege mode for channel " << channel << " set to: " << (enable ? "true" : "false") << std::endl;
			}
			else
				std::cerr << "Unknown mode: " << mode << std::endl;
			return;
		}
	}
	std::cout << "Channel " << channel << " not found" << std::endl;
}

void	server::handleMode(const std::string& channel, const std::string& mode, int number) {
	if (channel.empty() || mode.empty() || mode != "l" || number < 0) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == channel) {
			if (number != channels[i]->getMaxUsers()) {
				channels[i]->setMaxUsers(0);
				std::cout << "Max users for channel " << channel << " set to: " << number << std::endl;
			}
			else {
				std::cout << "Max users for channel " << channel << " already set to: " << number << std::endl;
			}
			return;
		}
		std::cout << "Channel " << channel << " not found" << std::endl;
	}
}

