#pragma once

#include "XYHWin.h"
#include "XuYuHaoException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <optional>
#include <memory>


class Window
{
public:
	class Exception : public XyhException
	{
		using XyhException::XyhException;
	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;

		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;

	private:
		HRESULT hr;
	};

	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
	};

private:
	class WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
	private:
		static constexpr const char* wndClassName = "XYH Direct3D Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};

public:
	Window(int width, int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	void SetTitle(const std::string& titleName);

	// 处理所有窗口的消息
	static std::optional<int> ProcessMessages();

	// 获取Graphics
	Graphics& GetGfx()const;

	// 鼠标禁用启用
	void EnableCursor();
	void DisableCursor();
	// 获取鼠标是否启用
	bool CursorEnabled() const noexcept;

private:
	// 限制鼠标？？
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;

	// 隐藏和显示鼠标
	void HideCursor();
	void ShowCursor();

	// 打开和关闭鼠标控制UI
	void EnableImGuiMouse();
	void DisableImGuiMouse();

	// 窗口消息接收
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard kbd;
	Mouse m_mouse;
private:
	int width;
	int height;
	HWND hWnd; // 主窗口句柄
	bool m_cursorEnabled = true; // 鼠标是否启用
	std::unique_ptr<Graphics> m_upGraphics; // 图形类

	std::vector<BYTE> m_rawBuffer; // ???
};

