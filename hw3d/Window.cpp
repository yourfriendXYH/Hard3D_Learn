#include "Window.h"
#include "CommonFunction.h"
#include "resource.h"
#include <sstream>
#include "WindowsThrowMacros.h"
#include "imgui/imgui_impl_win32.h"

//Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
	//注册窗口类
	RegisterClassEx(&wc);

}

Window::WindowClass::~WindowClass()
{
	//取消注册
	UnregisterClass(wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

Window::Window(int width, int height, const char* name) : width(width), height(height)
{
	RECT wr;
	wr.left = 100;
	wr.top = 100;
	wr.right = width + wr.left;
	wr.bottom = height + wr.top;

	if (0 == AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, false))
	{
		throw XYHWND_LAST_EXCEPT();
	}

	this->width = width;
	this->height = height;

	//throw XYHWND_EXCEPT(ERROR_ARENA_TRASHED);
	//throw std::runtime_error("butts butts sssssssssssssssssssss");
	//throw 123456;

	//throw Window::Exception(__LINE__, __FILE__, ERROR_ARENA_TRASHED);

	//创建窗口 并获取窗口句柄
	//比CreateWindowEx少了第一个参数？
	hWnd = CreateWindow(
		/*"123456"*/WindowClass::GetName(),
		name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this //将自身传过去，后面会用？
	);

	if (hWnd == nullptr)
	{
		throw XYHWND_LAST_EXCEPT();
	}

	//显示窗口
	ShowWindow(hWnd, SW_SHOWDEFAULT);

	//初始化 imgui
	ImGui_ImplWin32_Init(hWnd);

	//创建图形设备
	m_upGraphics = std::make_unique<Graphics>(hWnd, width, height);

	// 注册鼠标原始输入设备
	RAWINPUTDEVICE rawInputDevice;
	rawInputDevice.usUsagePage = 0x01;
	rawInputDevice.usUsage = 0x02;
	rawInputDevice.dwFlags = 0;
	rawInputDevice.hwndTarget = nullptr;
	if (RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice)) == FALSE)
	{

	}
}

void Window::SetTitle(const std::string& titleName)
{
	if (SetWindowText(hWnd, titleName.c_str()) == 0)
	{
		throw XYHWND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return msg.wParam;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

Graphics& Window::GetGfx() const
{
	if (nullptr == m_upGraphics)
	{
		XYHWND_NOGFX_EXCEPT();
	}
	return *m_upGraphics;
}

void Window::EnableCursor()
{
	m_cursorEnabled = true;
	ShowCursor();
	EnableImGuiMouse();
	FreeCursor();
}

void Window::DisableCursor()
{
	m_cursorEnabled = false;
	HideCursor();
	DisableImGuiMouse();
	ConfineCursor();
}

bool Window::CursorEnabled() const noexcept
{
	return m_cursorEnabled;
}

Window::~Window()
{
	// 关闭imgui
	ImGui_ImplWin32_Shutdown();

	DestroyWindow(hWnd);
}

void Window::ConfineCursor() noexcept
{
	// 限制鼠标的范围
	RECT rect;
	GetClientRect(hWnd, &rect);
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	ClipCursor(&rect);
}

void Window::FreeCursor() noexcept
{
	ClipCursor(nullptr);
}

void Window::HideCursor()
{
	// < 0是隐藏鼠标
	while(::ShowCursor(FALSE) >= 0);
}

void Window::ShowCursor()
{
	// >= 0是显示鼠标
	while(::ShowCursor(TRUE) < 0);
}

void Window::EnableImGuiMouse()
{
	ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void Window::DisableImGuiMouse()
{
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));

		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	break;
	case WM_KILLFOCUS:
	{
		kbd.ClearState();
	}
	break;
	case WM_ACTIVATE:
	{
		OutputDebugString("activate\n");
		if (!m_cursorEnabled) // 鼠标隐藏
		{
			if (wParam & WA_ACTIVE)
			{
				ConfineCursor();
				HideCursor();
			} 
			else
			{
				FreeCursor();
				ShowCursor();
			}
		}
		break;
	}
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnable())
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
	}
	break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	}
	case WM_CHAR:
	{
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
	}
	case WM_MOUSEMOVE:	//鼠标移动
	{
		const POINTS point = MAKEPOINTS(lParam);
		if (!m_cursorEnabled && !m_mouse.IsInWindow()) //
		{
			SetCapture(hWnd);
			// 鼠标进入窗口时隐藏
			m_mouse.OnMouseEnter();
			HideCursor();

			break;
		}

		const POINTS pt = MAKEPOINTS(lParam);
		if (pt.x >= 0 && pt.x < this->width && pt.y >= 0 && pt.y < this->height)
		{
			m_mouse.OnMouseMove(pt.x, pt.y);
			if (!m_mouse.IsInWindow())
			{
				SetCapture(hWnd);
				m_mouse.OnMouseEnter();
			}
		}
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON)) // 有bug
			{
				m_mouse.OnMouseMove(pt.x, pt.y);
			}
			else
			{
				ReleaseCapture();
				m_mouse.OnMouseLeave();
			}
		}

		break;
	}
	case WM_LBUTTONDOWN:	//鼠标左键按下
	{
		SetForegroundWindow(hWnd);
		if (!m_cursorEnabled)
		{
			ConfineCursor();
			HideCursor();
		}

		const POINTS pt = MAKEPOINTS(lParam);
		m_mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:	//鼠标右键按下
	{
		const POINTS pt = MAKEPOINTS(lParam);
		m_mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:	//鼠标左键抬起
	{
		const POINTS pt = MAKEPOINTS(lParam);
		m_mouse.OnLeftReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP:	//鼠标右键抬起
	{
		const POINTS pt = MAKEPOINTS(lParam);
		m_mouse.OnRightReleased(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL:	//鼠标滚轮
	{
		const POINTS pt = MAKEPOINTS(lParam);
		const int tempDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		m_mouse.OnWheelDelta(pt.x, pt.y, tempDelta);
		break;
	}
	case WM_INPUT:
	{
		if (!m_mouse.RawEnabled())
		{
			break;
		}

		UINT size;
		// 获取原始输入的大小
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam), 
			RID_INPUT, 
			nullptr, 
			&size, 
			sizeof(RAWINPUTHEADER)) == -1)
		{
			break;
		}
		m_rawBuffer.resize(size);
		// 获取原始输入的数据
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT, 
			m_rawBuffer.data(),
			&size,
			sizeof(RAWINPUTHEADER)) != size)
		{
			break;
		}
		// 获取设备原始输入数据
		auto& rawInput = reinterpret_cast<const RAWINPUT&>(*m_rawBuffer.data());
		if (rawInput.header.dwType == RIM_TYPEMOUSE && 
			(rawInput.data.mouse.lLastX != 0 || rawInput.data.mouse.lLastY != 0))
		{
			m_mouse.OnRawDelta(rawInput.data.mouse.lLastX, rawInput.data.mouse.lLastY);
		}

		break;
	}
	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file), hr(hr)
{
}

const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code]" << GetErrorCode() << std::endl
		<< "[Description]" << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
	return "XYH Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);

	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	std::string theErrorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return theErrorString;
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return TranslateErrorCode(hr);
}

const char* Window::NoGfxException::GetType() const noexcept
{
	return "XYH Window Exception [No Graphics]";
}
