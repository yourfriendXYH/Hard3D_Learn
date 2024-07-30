#include <Windows.h>
#include <string>
#include <iostream>
#include <sstream>
#include "Window.h"
#include "XuYuHaoException.h"
#include "CommonFunction.h"
#include "XYHApp.h"

//LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	switch (msg)
//	{
//	case WM_CLOSE:
//		PostQuitMessage(69);
//		break;
//	case WM_KEYDOWN:
//		if (wParam == 'F')
//		{
//			SetWindowText(hWnd, TEXT("Respects"));
//		}
//		break;
//	case WM_KEYUP:
//		if (wParam == 'F')
//		{
//			SetWindowText(hWnd, TEXT("Dangerfield"));
//		}
//		break;
//	case WM_CHAR:
//	{
//		static std::string title;
//		title.push_back((char)wParam);
//		SetWindowText(hWnd, stringToLPCWSTR(title));
//	}
//	break;
//	case WM_LBUTTONDOWN:
//	{
//		const POINTS mousePos = MAKEPOINTS(lParam);
//		std::ostringstream oss;
//		oss << "(" << mousePos.x << "," << mousePos.y << ")";
//		SetWindowText(hWnd, stringToLPCWSTR(oss.str().c_str()));
//	}
//	break;
//	default:
//		break;
//	}
//	return DefWindowProc(hWnd, msg, wParam, lParam);
//}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{


	try
	{
		return XYHApp{ lpCmdLine }.Go();
		//Window XYHWindow(640, 480, "XYH");

		//MSG msg;
		//BOOL gResult;
		//while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
		//{
		//	TranslateMessage(&msg);
		//	DispatchMessage(&msg);

		//	if (XYHWindow.kbd.KeyIsPressed(VK_MENU))
		//	{
		//		MessageBox(nullptr, "123 space", "XYH", MB_OK | MB_ICONEXCLAMATION);
		//	}

		//	while (!XYHWindow.m_mouse.IsEmpty())
		//	{
		//		const Mouse::Event& eventResult = XYHWindow.m_mouse.Read();

		//		switch (eventResult.GetType())
		//		{
		//		case Mouse::Event::Type::Leave:
		//		{
		//			XYHWindow.SetTitle("Gone!");
		//			break;
		//		}
		//		case Mouse::Event::Type::Move:
		//		{
		//			std::ostringstream oss;
		//			oss << "Mouse Position: (" << eventResult.GetPosX() << "," << eventResult.GetPosY() << ")";
		//			XYHWindow.SetTitle(oss.str());
		//			break;
		//		}
		//		case Mouse::Event::Type::WheelUp:
		//		{

		//			break;
		//		}
		//		case Mouse::Event::Type::WheelDown:
		//		{
		//			break;
		//		}
		//		default:
		//			break;
		//		}
		//	}
		//}
		//if (gResult == -1)
		//{
		//	return -1;
		//}

		//return (int)msg.wParam;
	}
	catch (const XyhException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}