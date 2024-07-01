#include "Mouse.h"
#include <windows.h>

std::pair<int, int> Mouse::GetPos() const noexcept
{
	return { x, y };
}

std::optional<Mouse::RawDelta> Mouse::ReadRawDelta() noexcept
{
	if (m_rawDeltaBuffer.empty())
	{
		return std::nullopt;
	}
	const RawDelta delta = m_rawDeltaBuffer.front();
	m_rawDeltaBuffer.pop();
	return delta;
}

int Mouse::GetPosX() const noexcept
{
	return x;
}

int Mouse::GetPosY() const noexcept
{
	return y;
}

bool Mouse::LeftIsPressed() const noexcept
{
	return leftIsPressed;
}

bool Mouse::RightIsPressed() const noexcept
{
	return rightIsPressed;
}

Mouse::Event Mouse::Read() noexcept
{
	if (buffer.size() > 0u)
	{
		Mouse::Event e = buffer.front();
		buffer.pop();
		return e;
	}
	else
	{
		return Mouse::Event();
	}
}

void Mouse::Flush() noexcept
{
	buffer = std::queue<Event>();
}

bool Mouse::IsInWindow() noexcept
{
	return m_isInWindow;
}

void Mouse::EnableRaw() noexcept
{
	m_rawEnabled = true;
}

void Mouse::DisableRaw() noexcept
{
	m_rawEnabled = false;
}

bool Mouse::RawEnabled() const noexcept
{
	return m_rawEnabled;
}

void Mouse::OnMouseMove(int x, int y) noexcept
{
	this->x = x;
	this->y = y;

	buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
	TrimBuffer();
}

void Mouse::OnLeftPressed(int x, int y) noexcept
{
	leftIsPressed = true;

	buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
	TrimBuffer();
}

void Mouse::OnLeftReleased(int x, int y) noexcept
{
	leftIsPressed = false;

	buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
	TrimBuffer();
}

void Mouse::OnRightPressed(int x, int y) noexcept
{
	rightIsPressed = true;

	buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
	TrimBuffer();
}

void Mouse::OnRightReleased(int x, int y) noexcept
{
	rightIsPressed = false;

	buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
	TrimBuffer();
}

void Mouse::OnWheelUp(int x, int y) noexcept
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	TrimBuffer();
}

void Mouse::OnWheelDown(int x, int y) noexcept
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
	TrimBuffer();
}

void Mouse::TrimBuffer() noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}

void Mouse::OnMouseEnter() noexcept
{
	m_isInWindow = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
	TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
	m_isInWindow = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
	TrimBuffer();
}

void Mouse::OnWheelDelta(int x, int y, int delta) noexcept
{
	m_wheelDeltaCarry += delta;
	while (m_wheelDeltaCarry >= WHEEL_DELTA)
	{
		m_wheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp(x, y);
	}

	while (m_wheelDeltaCarry <= -WHEEL_DELTA)
	{
		m_wheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown(x, y);
	}
}

void Mouse::OnRawDelta(int dx, int dy) noexcept
{
	m_rawDeltaBuffer.push({ dx, dy });
	TrimRawInputBuffer();
}

void Mouse::TrimRawInputBuffer() noexcept
{
	while (m_rawDeltaBuffer.size() > bufferSize)
	{
		m_rawDeltaBuffer.pop();
	}
}
