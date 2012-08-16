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

#include <Logger.h>
#include <cassert>

using namespace std;

Logger logger;

Logger::LogStreams *Logger::logStreams = NULL;

// class LogStream

LogStream::LogStream(ostream *out, const Timer *timer): out(out), timer(timer), first(true)
{
	if (out)
		*out << fixed << setprecision(2);
}

LogStream::~LogStream()
{
}

bool LogStream::isOutValid()
{
	return (bool)out;
}

void LogStream::newCycle()
{
	if (out)
	{
		unsigned cycle = timer ? timer->getCycle() : 0;
		*out << endl << "[" << setw(FIELD_WIDTH) << setfill(FIELD_FILL)
			<< cycle << "]" << endl;
	}
}

LogStream &LogStream::operator<<(ostream &(*man)(ostream &))
{
	if (out)
	{
		*out << man;
		if (man == (ostream &(*)(ostream &))endl)
			first = true;
	}
	return *this;
}

LogStream &LogStream::operator<<(ios_base &(*man)(ios_base &))
{
	if (out)
		*out << man;
	return *this;
}

// class LogNull

LogNull::LogNull(): LogStream(NULL, NULL)
{
}

LogNull::~LogNull()
{
}

// class LogFile

LogFile::LogFile(const string &fileName, const Timer *timer):
	LogStream(new ofstream(fileName.c_str()), timer),
	fileName(fileName)
{
}

LogFile::~LogFile()
{
	delete out;
}

// class LogStdOut

LogStdOut::LogStdOut(const Timer *timer): LogStream(&cout, timer)
{
}

LogStdOut::~LogStdOut()
{
}

// class LogStdErr

LogStdErr::LogStdErr(const Timer *timer): LogStream(&cerr, timer)
{
}

LogStdErr::~LogStdErr()
{
}

// class Logger::LogStreams

Logger::LogStreams::LogStreams()
{
}

Logger::LogStreams::~LogStreams()
{
	for (map<string, LogStream *>::iterator it = streams.begin();
		it != streams.end(); it++)
		delete it->second;
}

void Logger::LogStreams::add(const string &name, LogStream *stream)
{
	map<string, LogStream *>::iterator it = streams.find(name);
	if (it != streams.end()) return;

	streams[name] = stream;
}

LogStream &Logger::LogStreams::operator[](const string &name) const
{
	map<string, LogStream *>::const_iterator it = streams.find(name);
	if (it == streams.end())
		return (LogStream &)logNull;
	return *it->second;
}

// class Logger

Logger::Logger()
{
	if (!logStreams)
		logStreams = new LogStreams();
}

Logger::~Logger()
{
}

void Logger::add(const string &name, LogStream *stream)
{
	assert(logStreams);
	logStreams->add(name, stream);
}

LogStream &Logger::operator[](const string &name) const
{
	assert(logStreams);
	return (*logStreams)[name];
}
