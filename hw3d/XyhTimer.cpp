#include "XyhTimer.h"

XyhTimer::XyhTimer()
{
	m_last = std::chrono::steady_clock::now();
}

float XyhTimer::Mark()
{
	const auto old = m_last;
	m_last = std::chrono::steady_clock::now();
	const std::chrono::duration<float> frameTime = m_last - old;
	return frameTime.count();
}

float XyhTimer::Peek() const
{
	return std::chrono::duration<float>(std::chrono::steady_clock::now() - m_last).count();
}
