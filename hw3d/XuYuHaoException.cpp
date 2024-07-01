#include "XuYuHaoException.h"
#include <sstream>

XyhException::XyhException(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{
}

const char* XyhException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* XyhException::GetType() const noexcept
{
	return "XYH Exception";
}

int XyhException::GetLine() const noexcept
{
	return line;
}

const std::string& XyhException::GetFile() const noexcept
{
	return file;
}

std::string XyhException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File]" << file << std::endl
		<< "[Line]" << line;

	return oss.str();
}

