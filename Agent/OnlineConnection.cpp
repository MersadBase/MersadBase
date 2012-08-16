/*
 *  Copyright 2002-2010, Mersad Team, Allameh Helli High School (NODET).
 *
 *  This program is free software, you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Library General Public License for more details.
 *
 *  This file is created by: Mohsen Izadi
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <OnlineConnection.h>
#include <csignal>
#include <cstring>
#include <cstdlib>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cassert>
#include <stdio.h>

using namespace std;

OnlineConnection::OnlineConnection(const string &host, int port):
	socket(new Socket), host(host), port(port)
{
	assert(init());

	localBuffer = "";
}

OnlineConnection::~OnlineConnection()
{
	if (socket) delete socket;
}

bool OnlineConnection::init()
{
	hostent *hostEnt;
	in_addr *addrPtr;
	sockaddr_in cliAddr;
	int sockfd, val;

	socket->fd = -1;

	if (!(hostEnt = (hostent *)gethostbyname(host.c_str())))
	{
		if ((int)inet_addr(host.c_str()) == -1)
			return false;
	}
	else
	{
		addrPtr = (in_addr *) *hostEnt->h_addr_list;
        host = inet_ntoa(*addrPtr);
	}


	// Open UDP socket.
	if ((sockfd = ::socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return false; // Can't open socket.

	if (fcntl(sockfd, F_SETOWN, getpid()) == -1)
		return false;

	val = fcntl(sockfd, F_GETFL, 0);

	val |= O_NDELAY;
	val |= FASYNC;
	fcntl(sockfd, F_SETFL, val);

	// Bind any local address.
	bzero((char *)&cliAddr, sizeof cliAddr);
	cliAddr.sin_family = AF_INET;
	cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	cliAddr.sin_port = htons(0);

	if (bind(sockfd, (sockaddr *)&cliAddr, sizeof cliAddr) < 0)
		return false; // Can't bind local address

	// Fill in the structure with the address of the server.
	socket->fd = sockfd;

	bzero((char *)&socket->servAddr, sizeof socket->servAddr);
	socket->servAddr.sin_family = AF_INET;
	socket->servAddr.sin_addr.s_addr = inet_addr(host.c_str());
	socket->servAddr.sin_port = htons(port);

	return true;
}

void OnlineConnection::send(const string &message)
{
	int n = message.length() + 1;

	if (sendto(socket->fd, message.c_str(), n, 0, (sockaddr *)&socket->servAddr,
		sizeof socket->servAddr) != n)
	{
		cout << "Send message fault." << endl;
		exit(1);
	}
}

int OnlineConnection::receive(string &message)
{
	if (localBuffer != "")
	{
		message = localBuffer;
		localBuffer = "";

		return 1;
	}

	message = "";
	char buf[MAX_MSG] = { 0 };
	sockaddr_in servAddr;
	unsigned servLen = sizeof servAddr;
	int n = recvfrom(socket->fd, buf, MAX_MSG, 0, (sockaddr *)&servAddr,
		&servLen);

    if (n < 0)
	{
		if (n == -1 && errno == EWOULDBLOCK)
		    return 0;
	    cout << "Receive error: " << errno << " on " << socket->fd << endl;
	    perror("");
	    return -1;
	}

	buf[n] = 0;

	socket->servAddr.sin_port = servAddr.sin_port;

	if (!n) return 0; // Received null message

	message = buf;
	return 1;
}

void OnlineConnection::setLocalBuffer(std::string &message)
{
	localBuffer = message;
}

