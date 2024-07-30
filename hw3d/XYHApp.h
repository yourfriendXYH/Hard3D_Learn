#pragma once
#include "Window.h"
#include "XyhTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "Light/PointLight.h"
#include "Drawable/Mesh.h"
#include "Drawable/TestPlane.h"
#include "Drawable/TestCube.h"

class XYHApp
{
public:
	XYHApp(const std::string& commandLine = "");

	int Go();

	~XYHApp();

private:
	void DoFrame();
	void ShowRawInputWindow();

private:

	std::string m_commandLine;

	int m_rawInputX = 0, m_rawInputY = 0;

	ImguiManager m_imguiMgr;

	Window m_wnd;
	XyhTimer m_timer;

	Camera m_camera;

	// 点光源对象
	PointLight m_pointLight;

	std::vector<std::unique_ptr<class Drawable>> m_drawables;

	static constexpr size_t m_drawablesNum = 180;

	Model m_model{ m_wnd.GetGfx(), "Models\\nano_textured\\nanosuit.obj", 2.0f };
	TestPlane m_testPlane;

	TestCube m_testCube;

	Model m_TestCube1{ m_wnd.GetGfx(), "Models\\brick_wall\\brick_wall.obj", 6.0f };

	// 哥布林
	Model m_gobber{ m_wnd.GetGfx(), "Models\\gobber\\GoblinX.obj", 6.0f };
};
