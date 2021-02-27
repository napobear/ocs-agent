/*
 * Support.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 *      Code for popen_streambuf found here:
 *      http://stackoverflow.com/questions/1683051/file-and-istream-connect-the-two
 */


#include "Support.h"

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <streambuf>
#include <string>


CommandStreamBuffer::CommandStreamBuffer()
	:
	fFile(NULL),
	fBuffer(NULL)
{
}


CommandStreamBuffer::CommandStreamBuffer(const char* fileName, const char* mode)
	:
	fFile(NULL),
	fBuffer(NULL)
{
	CommandStreamBuffer* buf = open(fileName, mode);
	if (buf == NULL)
		throw std::runtime_error("CommandStreamBuffer: cannot open file");
}


CommandStreamBuffer::~CommandStreamBuffer()
{
	close();
}


CommandStreamBuffer*
CommandStreamBuffer::open(const char* fileName, const char* mode)
{
	fFile = ::popen(fileName, mode);
	if (fFile == NULL)
		return NULL;

	fBuffer = new char[512];
	if (fBuffer == NULL)
		return NULL;

	setg(fBuffer, fBuffer, fBuffer);
	return this;
}


void
CommandStreamBuffer::close()
{
	if (fFile != NULL) {
		::pclose(fFile);
		fFile = NULL;
	}

	delete[] fBuffer;
}


std::streamsize
CommandStreamBuffer::xsgetn(char_type* ptr, std::streamsize num)
{
	std::streamsize howMany = showmanyc();
	if (num < howMany) {
		::memcpy(ptr, gptr(), num * sizeof(char_type));
		gbump(num);
		return num;
	}

	::memcpy(ptr, gptr(), howMany * sizeof(char_type));
	gbump(howMany);

	if (traits_type::eof() == underflow())
		return howMany;

	return howMany + xsgetn(ptr + howMany, num - howMany);
}


CommandStreamBuffer::traits_type::int_type
CommandStreamBuffer::underflow()
{
	if (gptr() == 0)
		return traits_type::eof();

	if (gptr() < egptr())
		return traits_type::to_int_type(*gptr());

	size_t len = ::fread(eback(), sizeof(char_type), 512, fFile);
	setg(eback(), eback(), eback() + sizeof(char_type) * len);
	if (len == 0)
		return traits_type::eof();
	return traits_type::to_int_type(*gptr());
}


std::streamsize
CommandStreamBuffer::showmanyc()
{
	if (gptr() == 0)
		return 0;
	if (gptr() < egptr())
		return egptr() - gptr();
	return 0;
}


bool
CommandExists(const char* command)
{
	std::string fullCommand;
	fullCommand.append("type ").append(command).append(" > /dev/null 2>&1");

	int result = -1;
	int systemStatus = ::system(fullCommand.c_str());
	if (systemStatus == 0)
		result = WEXITSTATUS(systemStatus);
		
	return result == 0;
}

