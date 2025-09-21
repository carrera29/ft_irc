/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pollo <pollo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 10:03:15 by pollo             #+#    #+#             */
/*   Updated: 2025/08/18 11:40:06 by pollo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/client.hpp"

client::client(const std::string nickname, const std::string& username, const int socket) {
    _socket = socket;
    _nickname = nickname;
    _username = username;
    _channels.clear();

    std::cout << "Client created with nickname: " << _nickname << " and username: " << _username << std::endl;
}

client::~client() {
    _channels.clear();
    if (_socket >= 0)
        close(_socket);

    std::cout << "Client disconnected" << std::endl;
}

int	client::getClientSocket() {
    return _socket;
}

const std::string	client::getNickname() const {
    return _nickname;
}

const std::string	client::getUsername() const {
    return _username;
}