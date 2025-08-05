/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pollo <pollo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 13:13:30 by pollo             #+#    #+#             */
/*   Updated: 2025/08/05 13:13:33 by pollo            ###   ########.fr       */
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

class server;
class channel;

class client {

    private:
        
        std::string				_nickname;
		std::string				_username;
        int						_socket;
        std::vector<channel*>	_channels;


    public:


		client(const std::string nickname, const std::string& username, const int socket) {
			_socket = socket;
			_nickname = nickname;
			_username = username;
			_channels.clear();

			std::cout << "Client created with nickname: " << _nickname << " and username: " << _username << std::endl;
		}

		~client() {
			_channels.clear();
			if (_socket >= 0)
				close(_socket);

			std::cout << "Client disconnected" << std::endl;
		}

		const int	getClientSocket() {
			return _socket;
		}

		const std::string	getNickname() const {
			return _nickname;
		}

		const std::string	getUsername() const {
			return _username;
		}
};
