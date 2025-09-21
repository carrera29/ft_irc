/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pollo <pollo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:51:45 by pollo             #+#    #+#             */
/*   Updated: 2025/08/18 14:21:09 by pollo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/channel.hpp"

channel::channel(const std::string& name, const std::string& topic, client* creator) {
    _name = "#" + name;
    _topic = topic;
    _users.push_back(creator);
    _operators.insert(creator);
    _inviteOnlyChannel = false;
    _restrictedTopic = false;
    _channelKeyRequired = false;
    _channelOperatorPrivilege = false;
    _maxUsers = 0; 

    std::cout << "Channel " << _name << " created" << std::endl;
}

channel::~channel() {
    _users.clear();
    _operators.clear();
    _bannedUsers.clear();
    _name.clear();
    _topic.clear();

    std::cout << "Channel " << _name << ": deleted" << std::endl;
}

const std::string	channel::getChannelName() {
    return _name;
}

const std::string	channel::getChannelTopic() {
    return _topic;
}

void	channel::setChannelTopic(const std::string& topic) {
    _topic = topic;
}

void	channel::addUser(client* user) {
    if (std::find(_users.begin(), _users.end(), user) != _users.end()) {
        std::cout << "User " << user->getNickname() << " is already in channel " << _name << std::endl;
        return;
    }
    if (_maxUsers != 0 && _currentUsers >= _maxUsers) {
        std::cout << "Channel " << _name << " is full. Cannot add user " << user->getNickname() << std::endl;
        return;
    }
    _users.push_back(user);
    std::cout << "User " << user->getNickname() << " added to channel " << _name << std::endl;
}

void	channel::addOperator(client* user) {
    if (_operators.find(user) != _operators.end()) {
        return;
    }
    _operators.insert(user);

    std::cout << "User " << user->getNickname() << " is now an operator in channel " << _name << std::endl;
}

void	channel::removeOperator(client* user) {
    if (_operators.find(user) == _operators.end()) {
        return;
    }
    _operators.erase(user);

    std::cout << "User " << user->getNickname() << " is no longer an operator in channel " << _name << std::endl;
}

void	channel::removeUser(client* user) {
    std::vector<client*>::iterator it = std::find(_users.begin(), _users.end(), user);
    if (it == _users.end()) {
        std::cout << "User " << user->getNickname() << " not found in channel " << _name << std::endl;
        return;
    }
    _users.erase(it);
    if (_operators.find(user) != _operators.end()) {
        _operators.erase(user);
    }

    std::cout << "User " << user->getNickname() << " removed from channel " << _name << std::endl;
}

void	channel::banUser(client* user) {
    if (_bannedUsers.find(user) != _bannedUsers.end()) {
        return;
    }
    _bannedUsers.insert(user);
    removeUser(user);
    removeOperator(user);

    std::cout << "User " << user->getNickname() << " is banned from channel " << _name << std::endl;
}

bool	channel::getInviteOnlyChannel() const {
    return _inviteOnlyChannel;
}

bool	channel::getRestrictedTopic() const {
    return _restrictedTopic;
}

bool	channel::getChannelKeyRequired() const {
    return _channelKeyRequired;
}

bool	channel::getChannelOperatorPrivilege() const {
    return _channelOperatorPrivilege;
}

int		channel::getMaxUsers() const {
    return _maxUsers;
}

int		channel::getCurrentUsers() const {
    return (int)_users.size();
}

void	channel::setInviteOnlyChannel(bool inviteOnly) {
    _inviteOnlyChannel = inviteOnly;
}

void	channel::setRestrictedTopic(bool restricted) {
    _restrictedTopic = restricted;
}

void	channel::setChannelKeyRequired(bool keyRequired) {
    _channelKeyRequired = keyRequired;
}

void	channel::setChannelOperatorPrivilege(bool operatorPrivilege) {
    _channelOperatorPrivilege = operatorPrivilege;
}

void	channel::setMaxUsers(int maxUsers) {
    _maxUsers = maxUsers;
}