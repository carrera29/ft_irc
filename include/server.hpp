/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pollo <pollo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 13:12:49 by pollo             #+#    #+#             */
/*   Updated: 2025/08/05 16:08:21 by pollo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include    <iostream>
#include    <sys/socket.h>
#include    <netdb.h>
#include    <string.h>
#include    <unistd.h>
#include    <cstring>
#include    <poll.h>
#include    <vector>
#include    <map>

class client;
class channel;

class Server
{
	private:

		std::string			_password;
		std::string			_port;
		int					_serverSocket;
		struct addrinfo		_hints;

	public:

		std::vector<struct pollfd>				fds;
		std::map<int, class client *>			clients;
		std::vector<class channel *>			channels;

		Server(std::string password, int port) : _password(password), _port(port) {};
		~Server() {};

		std::string	getPassword() {};
		std::string	getPort() {};
		const int	getSocket() {};
		struct addrinfo	*getHints() {};
		
		void	setSocket(int socket) {};

		inline size_t	sizeofFds() {};

		class client	*createClient(std::string nickname, std::string username, int socket) {};
		void			removeClient(int socket) {};
		class client	*getClient(int socket) {};

		void	handleJoin(const std::string& name, const std::string& topic, class client* creator) {};
		void	handleKick(const std::string& name, class client* user) {};
		void	handleInvite(const std::string& name, class client* user) {};
		void	handleTopic(const std::string& name, const std::string& topic) {};
		void	handleMode(const std::string& name, const std::string& mode) {};

};