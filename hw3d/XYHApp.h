#pragma once
#include "Window.h"
#include "XyhTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "Light/PointLight.h"
#include "Drawable/Mesh.h"
#include "Drawable/TestPlane.h"
#include "Drawable/TestCube.h"
#include "Utils/ScriptCommander.h"
#include "Bindable/FrameCommander.h"
#include "Drawable/Model.h"

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

	// ���Դ����
	PointLight m_pointLight;

	ScriptCommander m_scriptCommander;

	FrameCommander m_frameCommader;

	std::vector<std::unique_ptr<class Drawable>> m_drawables;

	static constexpr size_t m_drawablesNum = 180;

	//Model m_model{ m_wnd.GetGfx(), "Models\\nano_textured\\nanosuit.obj", 2.0f };
	//TestPlane m_testPlane;

	TestCube m_testCube{ m_wnd.GetGfx(), 4.0f };

	//Model m_TestCube1{ m_wnd.GetGfx(), "Models\\brick_wall\\brick_wall.obj", 6.0f };

	// �粼��
	//Model m_gobber{ m_wnd.GetGfx(), "Models\\gobber\\GoblinX.obj", 6.0f };

	// ����
	Model m_testStripey{ m_wnd.GetGfx(), "Models\\stripey\\stripey.obj", 2.0f };

	// Sponza����
	Model m_testSponza{ m_wnd.GetGfx(), "Models\\Sponza\\sponza.obj", 1.0f / 20.0f };

	// Alpha��ϲ���Model
	//TestPlane m_testBluePlane{ m_wnd.GetGfx(), 6.0f, {0.3f, 0.3f, 1.0f, 0.0f} };
	//TestPlane m_testRedPlane{ m_wnd.GetGfx(), 6.0f, {1.0f, 0.3f, 0.3f, 0.0f} };

	std::unique_ptr<Mesh> m_pLoaded;
};
