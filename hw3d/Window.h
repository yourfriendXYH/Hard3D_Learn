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

	// �������д��ڵ���Ϣ
	static std::optional<int> ProcessMessages();

	// ��ȡGraphics
	Graphics& GetGfx()const;

	// ����������
	void EnableCursor();
	void DisableCursor();
	// ��ȡ����Ƿ�����
	bool CursorEnabled() const noexcept;

private:
	// ������ꣿ��
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;

	// ���غ���ʾ���
	void HideCursor();
	void ShowCursor();

	// �򿪺͹ر�������UI
	void EnableImGuiMouse();
	void DisableImGuiMouse();

	// ������Ϣ����
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard kbd;
	Mouse m_mouse;
private:
	int width;
	int height;
	HWND hWnd; // �����ھ��
	bool m_cursorEnabled = true; // ����Ƿ�����
	std::unique_ptr<Graphics> m_upGraphics; // ͼ����

	std::vector<BYTE> m_rawBuffer; // ???
};

