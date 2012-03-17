#pragma once

#include <string>

class Exception
{
public:
	Exception()
	{
	}

	Exception(const std::string &msg) : msg(msg)
	{
	}

	const std::string &GetMessage()
	{
		return msg;
	}

private:
	std::string msg;
};

class ArgumentException : public Exception
{
public:
	ArgumentException() : Exception()
	{
	}

	ArgumentException(const std::string &msg) : Exception(msg)
	{
	}
};

class ArgumentNullException : public ArgumentException
{
public:
	ArgumentNullException() : ArgumentException()
	{
	}

	ArgumentNullException(const std::string &msg) : ArgumentException(msg)
	{
	}
};

class ArgumentOutOfRangeException : public ArgumentException
{
public:
	ArgumentOutOfRangeException() : ArgumentException()
	{
	}

	ArgumentOutOfRangeException(const std::string &msg) : ArgumentException(msg)
	{
	}
};

class FileNotFoundException : public Exception
{
public:
	FileNotFoundException() : Exception()
	{
	}

	FileNotFoundException(const std::string &msg) : Exception(msg)
	{
	}
};

class IOException : public Exception
{
public:
	IOException() : Exception()
	{
	}

	IOException(const std::string &msg) : Exception(msg)
	{
	}
};

class NotSupportedException : public Exception
{
public:
	NotSupportedException() : Exception()
	{
	}

	NotSupportedException(const std::string &msg) : Exception(msg)
	{
	}
};

class OutOfMemoryException : public Exception
{
public:
	OutOfMemoryException() : Exception()
	{
	}

	OutOfMemoryException(const std::string &msg) : Exception(msg)
	{
	}
};
