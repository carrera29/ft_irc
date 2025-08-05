/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pollo <pollo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 13:13:13 by pollo             #+#    #+#             */
/*   Updated: 2025/08/05 16:04:45 by pollo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>
#include <vector>
#include <set>
#include "client.hpp"

class server;
class client;

class channel {

    private:
        
        std::string				_name;
		std::string				_topic;
        std::vector<client*>	_users;
		std::set<client*>		_operators;
		std::set<client*>		_bannedUsers;

		bool					_inviteOnlyChannel;
		bool					_restrictedTopic;
		bool					_channelKeyRequired;
		bool					_channelOperatorPrivilege;
		int						_maxUsers;


    public:	

		channel(const std::string& name, const std::string& topic, client* creator) {
			_name = "#"+name;
			_topic = topic;
			_users.push_back(creator);
			_operators.insert(creator);
			_inviteOnlyChannel = false;
			_restrictedTopic = false;
			_channelKeyRequired = false;
			_channelOperatorPrivilege = false;
			_maxUsers = 0; 

			std::cout << "Channel " << _name << ": created" << std::endl;
		}

		~channel() {
			_users.clear();
			_operators.clear();
			_bannedUsers.clear();
			_name.clear();
			_topic.clear();

			std::cout << "Channel " << _name << ": deleted" << std::endl;
		}

		const std::string	getChannelName() {
			return _name;
		}

		const std::string	getChannelTopic() {
			return _topic;
		}

		void	setChannelTopic(const std::string& topic) {
			_topic = topic;
		}

		void	addUser(client* user) {
			if (std::find(_users.begin(), _users.end(), user) != _users.end()) {
				return;
			}
			if (_maxUsers != 0 && _users.size() >= _maxUsers) {
				std::cout << "Channel " << _name << " is full. Cannot add user " << user->getNickname() << std::endl;
				return;
			}
			_users.push_back(user);
			std::cout << "User " << user->getNickname() << " added to channel " << _name << std::endl;
		}

		void	addOperator(client* user) {
			if (_operators.find(user) != _operators.end()) {
				return;
			}
			_operators.insert(user);

			std::cout << "User " << user->getNickname() << " is now an operator in channel " << _name << std::endl;
		}

		void	removeOperator(client* user) {
			if (_operators.find(user) == _operators.end()) {
				return;
			}
			_operators.erase(user);

			std::cout << "User " << user->getNickname() << " is no longer an operator in channel " << _name << std::endl;
		}

		void	removeUser(client* user) {
			std::vector<client*>::iterator it = std::find(_users.begin(), _users.end(), user);
			if (it == _users.end()) {
				return;
			}
			_users.erase(it);
			if (_operators.find(user) != _operators.end()) {
				_operators.erase(user);
			}

			std::cout << "User " << user->getNickname() << " removed from channel " << _name << std::endl;
		}

		void	banUser(client* user) {
			if (_bannedUsers.find(user) != _bannedUsers.end()) {
				return;
			}
			_bannedUsers.insert(user);
			removeUser(user);
			removeOperator(user);

			std::cout << "User " << user->getNickname() << " is banned from channel " << _name << std::endl;
		}

		bool	getInviteOnlyChannel() const {
			return _inviteOnlyChannel;
		}

		bool	getRestrictedTopic() const {
			return _restrictedTopic;
		}

		bool	getChannelKeyRequired() const {
			return _channelKeyRequired;
		}

		bool	getChannelOperatorPrivilege() const {
			return _channelOperatorPrivilege;
		}

		int		getMaxUsers() const {
			return _maxUsers;
		}

		void	setInviteOnlyChannel(bool inviteOnly) {
			_inviteOnlyChannel = inviteOnly;
		}

		void	setRestrictedTopic(bool restricted) {
			_restrictedTopic = restricted;
		}

		void	setChannelKeyRequired(bool keyRequired) {
			_channelKeyRequired = keyRequired;
		}

		void	setChannelOperatorPrivilege(bool operatorPrivilege) {
			_channelOperatorPrivilege = operatorPrivilege;
		}

		void	setMaxUsers(int maxUsers) {
			_maxUsers = maxUsers;
		}

};
