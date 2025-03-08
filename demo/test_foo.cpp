#include <string>
#include <iostream>

#define BUFF_SIZE 6

std::string convert(char *buf)
{
	std::string str;

	if (buf[BUFF_SIZE - 1] != '\0')
	{
		char c = buf[BUFF_SIZE - 1];
		buf[BUFF_SIZE - 1] = '\0';
		str = buf;
		str = str + c;
	}
	std::cout << "str = " << str << std::endl;
	std::cout << "str ptr = " << &str << " buf ptr = " << &buf << std::endl;
	return str;
}

int	main(void)
{
	char buf[BUFF_SIZE] = {'s', 't', 'r', 'i', 'n', 'g'};

	std::string str = convert(buf);


	return 0;
}

/*----------------------------old working code----------------------------
static int	getConnIdx(int fd, std::vector<Conn> &conns)
{
	int size = conns.size();

	for (int i = 0; i < size; i++)
	{
		if (conns[i].fd == fd)
			return i;
	}
	return size;
}

void	ServerControler::startServing()
{
	int timeout, size, res, temp;
	std::string str;
	std::string request;
	struct pollfd pfds[MAX_CONN_NUM]; //max number of connections
	int nfds = 0;
	int new_fd = -1;
	char buf[1500];
	//bool conn_active = false;
	std::vector<Conn> conns;

	// signal(SIGINT, ServerControler::sig_handler);

	try
	{
		this->createListeningSockets();
	}
	catch(const std::exception& e)
	{
		// close all sockets;
		std::cerr << e.what() << '\n';
		throw std::runtime_error("Error: socket setup failed");
	}

	memset(pfds, 0, sizeof(pfds));
	size = _socketFds.size();
	for (int i = 0; i < size; i++)
	{
		pfds[i].fd = _socketFds[i];
		pfds[i].events = POLLIN;
		nfds++;
	}

	// polling();

	timeout = 2 * 60000;

	std::cout << "Waiting on poll" << std::endl;
	while (!g_serv_end)
	{
		res = poll(pfds, nfds, timeout);
		if (res < 0)
		{
			throw std::runtime_error(strerror(errno));
			//close(_socketFds);
			// std::cerr << "Error on poll: " << strerror(errno) << std::endl;
			// return;
		}
		if (res == 0)
		{
			std::cout << "Timeout" << std::endl;
			g_serv_end = true;
			//close(_socketFds);
			break;
		}
		//std::cout << "nfds = " << nfds << std::endl;
		for (int i = 0; i < nfds; i++)
		{
			if (pfds[i].revents == 0)
			{
				res = getConnIdx(pfds[i].fd, conns);
				if (res != static_cast<int>(conns.size()))
				{
					time_t t = time(NULL);
					temp = difftime(t, conns[res].start);
					if (temp > 5)
					{
						std::cout << "Timeout on connection " << res << " : " << temp << " sec" << std::endl;
						conns.erase(conns.begin() + res);
						close(pfds[i].fd);
						pfds[i].fd = -1;
					}
				}
			}
			else if ((pfds[i].revents & POLLHUP) || (pfds[i].revents & POLLERR))
			{
				res = getConnIdx(pfds[i].fd, conns);
				if (res != static_cast<int>(conns.size()))
					conns.erase(conns.begin() + res);
				close(pfds[i].fd);
				pfds[i].fd = -1;
				std::cout << "Connection " << res << " closed because of error or client disconnect" << std::endl;
			}
			else if (pfds[i].revents & POLLIN)
			{
				//std::cout << "POLLIN on poll_fd " << i << std::endl;
				res = isInPollfds(pfds[i].fd, _socketFds);
				if (res)
				{
					do
					{
						new_fd = accept(res, NULL, NULL);
						if (new_fd < 0 && errno != EWOULDBLOCK)
							throw std::runtime_error("Error: accept() failed");
						if (new_fd > 0)
						{
							std::cout << "New connection on listening socket " << res << ", new_fd = " << new_fd << std::endl;
							pfds[nfds].fd = new_fd;
							pfds[nfds].events = POLLIN;
							nfds++;
							Conn c;
							c.fd = new_fd;
							c.active = true;
							c.start = time(NULL);
							conns.push_back(c);
						}
					} while (new_fd != -1);
				}
				else
				{
					//conn_active = true;
					temp = getConnIdx(pfds[i].fd, conns);

					request = "";
					str = "";
					do
					{
						memset(buf, 0, sizeof(buf));
						res = recv(pfds[i].fd, buf, sizeof(buf), 0);
						if (res < 0)
						{
							std::cout << "Error: recv() failed" << std::endl;
							break;
						}
						if (res > 0)
						{
							request.append(buf);
						}
						if (res == 0)
						{
							if (temp != static_cast<int>(conns.size()))
							{
								time_t t = time(NULL);
								if (difftime(t, conns[res].start) > 5)
								{
									conns.erase(conns.begin() + temp);
									close(pfds[i].fd);
									pfds[i].fd = -1;
									std::cout << "Connection " << temp << " closed because of timeout" << std::endl;
								}
							}
						}
					} while (res == 1500);
					if (!request.empty())
					{
						conns[temp].start = time(NULL);
						std::cout << "Processing request..." << std::endl;
						str = processRequest(request);

						if (!str.empty())
						{
							res = send(pfds[i].fd, str.c_str(), str.size(), 0);
							if (res < 0)
								throw std::runtime_error("Error: send() failed");
							std::cout << "[INFO] : "  << utils::getFormattedDateTime() << " Transmitted Data Size "<< res <<" Bytes."  << std::endl;
							std::cout << "[INFO] : "  << utils::getFormattedDateTime() << "  File Transfer Complete." << std::endl;
						}
					}
					//conn_active = false;
				}
			}
			// else
			// {
			// 	std::cout << "Poll_fd " << i << " revents = " << pfds[i].revents << std::endl;
			// 	//if (i == nfds - 1)
			// 	//	servEnd = true;
			// }
			for (int n = 0; n < nfds; n++)
			{
				if (pfds[n].fd == -1)
				{
					for(int j = n; j < nfds - 1; j++)
					{
						pfds[j].fd = pfds[j + 1].fd;
						pfds[j].events =pfds[j + 1].events;
						pfds[j].revents =pfds[j + 1].revents;
					}
					n--;
					nfds--;
				}
			}
		}
	}
	//close sockets;
	for (int i = 0; i < nfds; i++)
	{
		if(pfds[i].fd >= 0)
		close(pfds[i].fd);
	}
	return;
}
----------------------------------------------------------------------------------*/

// void	ServerControler::polling()
// {
// 	int	res, indx;
// 	int	new_fd;
// 	int timeout = 5 * 60000;
// 	Connection *connection = NULL;

// 	std::cout << "Waiting on poll" << std::endl;
// 	while (!g_serv_end)
// 	{
// 		res = poll(_pfds, _nfds, timeout);
// 		if (res < 0)
// 		{
// 			closeFds();
// 			throw std::runtime_error(strerror(errno));
// 		}
// 		if (res == 0)
// 		{
// 			std::cout << "Timeout" << std::endl;
// 			g_serv_end = true;
// 			break;
// 		}
// 		for (int i = 0; i < _nfds; i++)
// 		{
// 			if (_pfds[i].revents == 0)
// 			{
// 				connection = getConnection(_pfds[i].fd);
// 				if (connection && connection->isTimeout())
// 				{
// 					removeConnection(_pfds[i].fd);
// 					std::cout << "Connection on fd " << _pfds[i].fd << " was closed because of timeout" << std::endl;
// 				}
// 				continue;
// 			}
// 			else if ((_pfds[i].revents & POLLHUP) || (_pfds[i].revents & POLLERR))
// 			{
// 				connection = getConnection(_pfds[i].fd);
// 				if (connection)
// 				{
// 					removeConnection(_pfds[i].fd);
// 					std::cout << "Connection on fd " << _pfds[i].fd << " was closed because of error or client disconnect" << std::endl;
// 				}
// 				else
// 					removePfd(_pfds[i].fd);
// 				continue;
// 			}
// 			else if (_pfds[i].revents & POLLIN)
// 			{
// 				indx = isSocketFd(_pfds[i].fd);
// 				if (indx != -1)
// 				{
// 					new_fd = accept(_pfds[i].fd, NULL, NULL);
// 					if (new_fd < 0 && errno != EWOULDBLOCK)
// 						throw std::runtime_error("Error: accept() failed");
// 					if (new_fd > 0)
// 					{
// 						addConnection(new_fd);
// 						std::cout << "New connection on listening socket " << indx << ", new_fd = " << new_fd << std::endl;
// 						continue;
// 					}
// 				}
// 				else
// 					handleInEvent(_pfds[i].fd);
// 			}
// 			if (_pfds[i].revents & POLLOUT)
// 				handleOutEvent(_pfds[i].fd);
// 		}
// 	}
// 	closeFds();
// }

// static int	isInPollfds(int fd, const std::vector<int> & sds)
// {
// 	int	ss = sds.size();
// 	for (int i = 0; i < ss; i++)
// 	{
// 		if (fd == sds[i])
// 		return sds[i];
// 	}
// 	return 0;
// }
