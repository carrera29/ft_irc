/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pollo <pollo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 13:13:13 by pollo             #+#    #+#             */
/*   Updated: 2025/08/22 14:52:54 by pollo            ###   ########.fr       */
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
		int						_currentUsers;
		char					_channelKey[256];

    public:	

		channel(const std::string& name, const std::string& topic, client* creator);
		~channel();

		const std::string	getChannelName();
		const std::string	getChannelTopic();

		void	setChannelTopic(const std::string& topic);

		void	addUser(client* user);
		void	addOperator(client* user);
		void	removeOperator(client* user);
		void	removeUser(client* user);
		void	banUser(client* user);

		bool	getInviteOnlyChannel() const;
		bool	getRestrictedTopic() const;
		bool	getChannelKeyRequired() const;
		bool	getChannelOperatorPrivilege() const;
		int		getMaxUsers() const;
		int		getCurrentUsers() const;

		void	setInviteOnlyChannel(bool inviteOnly);
		void	setRestrictedTopic(bool restricted);
		void	setChannelKeyRequired(bool keyRequired);
		void	setChannelOperatorPrivilege(bool operatorPrivilege);
		void	setMaxUsers(int maxUsers);
		
};
