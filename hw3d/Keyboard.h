#pragma once

#include <bitset>
#include <queue>
#include <optional>

class Keyboard
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press = 0,
			Release,
		};
	public:
		Event(Type type, unsigned char code) noexcept
			:
			type(type),
			code(code)
		{
		}
		bool IsPressed() const noexcept
		{
			return type == Type::Press;
		}
		bool IsReleased() const noexcept
		{
			return type == Type::Release;
		}
		unsigned char GetCode() const noexcept
		{
			return code;
		}
	private:
		Type type;
		unsigned char code;
	};

public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;
	//key event stuff
	bool KeyIsPressed(unsigned char keycode) const noexcept;
	std::optional<Event> ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void FlushKey() noexcept;
	//char event stuff
	std::optional<char> ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;

	void Flush() noexcept;

	//autorepeat control
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatIsEnable() const noexcept;

private:
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;

private:
	//
	static constexpr unsigned int nKeys = 256u; //��������
	static constexpr unsigned int bufferSize = 16u; //�����С
	bool autorepeatEnable = false; //�Ƿ�Ĭ���ظ�
	std::bitset<nKeys> keystates; //����״̬
	std::queue<Event> keybuffer; //�����¼��Ļ������
	std::queue<char> charbuffer; //???
};