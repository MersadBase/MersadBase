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
 *  	and is modified by: Ahmad Boorghany
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __LOGGER_H
#define __LOGGER_H

#include <Timer.h>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <iomanip>

#define FIELD_FILL '0'
#define FIELD_WIDTH 4
#define PRECISION 2

#define LOG logger["MainLog"]
#define FAKELOG logger["Fake"]
#define OFFLOG logger["OfflineLog"]
#define DRAW   logger["DrawLog"]

class LogStream
{
protected:
	std::ostream *out;
	const Timer *timer;
	bool first;

public:
	LogStream(std::ostream *out, const Timer *timer);
	virtual ~LogStream();

	bool isOutValid();
	void newCycle();

	template<class T>
	LogStream &operator<<(const T &value)
	{
		if (out)
		{
			if (first)
			{
				first = false;
				unsigned now = timer ? timer->now() : 0;
				*out << "  " << std::setw(FIELD_WIDTH) << std::setfill(FIELD_FILL)
					<< now << ". ";
			}
			*out << value;
		}
		return *this;
	}

	LogStream &operator<<(std::ostream &(*man)(std::ostream &));
	LogStream &operator<<(std::ios_base &(*man)(std::ios_base &));
};

class LogNull: public LogStream
{
public:
	LogNull();
	virtual ~LogNull();
};

class LogFile: public LogStream
{
protected:
	std::string fileName;

public:
	LogFile(const std::string &fileName, const Timer *timer);
	virtual ~LogFile();
};

class LogStdOut: public LogStream
{
public:
	LogStdOut(const Timer *timer);
	virtual ~LogStdOut();
};

class LogStdErr: public LogStream
{
public:
	LogStdErr(const Timer *timer);
	virtual ~LogStdErr();
};

class Logger
{
protected:
	class LogStreams
	{
	protected:
		std::map<std::string, LogStream *> streams;
		LogNull logNull;

	public:
		LogStreams();
		virtual ~LogStreams();

		void add(const std::string &name, LogStream *stream);
		LogStream &operator[](const std::string &name) const;
	};

	static LogStreams *logStreams;

public:
	Logger();
	virtual ~Logger();

	void add(const std::string &name, LogStream *stream);
	LogStream &operator[](const std::string &name) const;
};

extern Logger logger;

#endif // __LOGGER_H
