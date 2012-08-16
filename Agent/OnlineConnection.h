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

#ifndef __ONLINE_CONNECTION_H
#define __ONLINE_CONNECTION_H

#include <Connection.h>
#include <netinet/in.h>

#ifndef NULL
#	define NULL 0
#endif // NULL

class OnlineConnection: public Connection
{
protected:
	struct Socket
	{
	    int fd;
	    sockaddr_in servAddr;
	};

	Socket *socket;
	std::string host;
	int port;

	virtual bool init();

	std::string localBuffer;

public:
	OnlineConnection(const std::string &host, int port);
	virtual ~OnlineConnection();

	virtual void send(const std::string &message);
	virtual int receive(std::string &message);

	void setLocalBuffer(std::string &message);
};

#endif // __ONLINE_CONNECTION_H
