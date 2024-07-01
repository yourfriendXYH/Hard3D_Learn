#pragma once
#include <chrono>

class XyhTimer
{
public:
	XyhTimer();

	float Mark();
	float Peek() const;

private:
	std::chrono::steady_clock::time_point m_last;
};