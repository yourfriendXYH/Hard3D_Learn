#include "XYHApp.h"
#include "TestBox.h"
#include "Melon.h"
#include "Sheet.h"
#include "XYHMath.h"
#include "Surface.h"
#include "SkinnedBox.h"
#include "Drawable/AssimpTest.h"
#include "GDIPlusManager.h"
#include "DynamicData/DynamicVertex.h"
#include "Utils/NormalMapTwerker.h"
#include "Utils/TexturePreprocessor.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#include "dxtex/include/dxtex/DirectXTex.h"

#include <shellapi.h>

//#include "assimp/include/assimp/Importer.hpp"
//#include "assimp/include/assimp/scene.h"
//#include "assimp/include/assimp/postprocess.h"
using namespace DynamicData;

// 动态顶点测试
void TestFun()
{
	VertexLayout vertexLayout;
	vertexLayout.Append(VertexLayout::Position3D).Append(VertexLayout::Normal);
	VerticesBuffer verticesBuf(std::move(vertexLayout));
	verticesBuf.EmplaceBack(DirectX::XMFLOAT3{ 1.0f,1.0f,0.5f }, DirectX::XMFLOAT3{ 2.0f,1.0f,4.0f });

	const auto attribute = verticesBuf[0].Attr<VertexLayout::Position3D>();
	const auto attribute1 = verticesBuf[0].Attr<VertexLayout::Normal>();

	auto LayoutDescArr = verticesBuf.GetVertexLayout().GetD3DLayout();

	if (1)
	{
	}
}

XYHApp::XYHApp(const std::string& commandLine)
	:
	m_wnd(1280, 720, "XYH"),
	m_pointLight(m_wnd.GetGfx()),
	//m_testPlane(m_wnd.GetGfx(), 3.0f),
	//m_testCube(m_wnd.GetGfx(), 4.0f),
	m_commandLine(commandLine)
{
	auto scratch = DirectX::ScratchImage{};
	DirectX::LoadFromWICFile(L"Resources\\Wall\\brickwall.jpg", DirectX::WIC_FLAGS_NONE, nullptr, scratch);
	auto testImage = scratch.GetImage(0, 0, 0);
	auto a = testImage->pixels[0];
	auto b = testImage->pixels[1];
	auto c = testImage->pixels[2];
	auto d = testImage->pixels[3];

	// 处理命令行
	if (this->m_commandLine != "")
	{
		int nArgs;
		const auto pLineW = GetCommandLineW(); // 获取WinMain的命令行字符
		const auto pArgs = CommandLineToArgvW(pLineW, &nArgs); // 解析命令行字符串
		// 扭转法线贴图的方向
		if (nArgs >= 3 && std::wstring(pArgs[1]) == L"--twerk-objnorm")
		{
			const std::wstring pathInWide = pArgs[2];

			// 处理法线纹理（处理所有的）
			TexturePreprocessor::FlipYAllNormalMapsInObj(std::string(pathInWide.begin(), pathInWide.end()));

			throw std::runtime_error("Normal map processed successfully. Just kidding about that whole runtime error thing.");
		}
		else if (nArgs >= 3 && std::wstring(pArgs[1]) == L"--twerk-flipy")
		{
			const std::wstring pathInWide = pArgs[2];
			const std::wstring pathOutWide = pArgs[3];

			// 处理单个法线纹理
			TexturePreprocessor::FlipYNormalMap(std::string(pathInWide.begin(), pathInWide.end()), std::string(pathOutWide.begin(), pathOutWide.end()));
		}
		// 验证法线贴图的有效性（处理单个）
		else if (nArgs >= 4 && std::wstring(pArgs[1]) == L"--twerk-validate")
		{
			const std::wstring minWide = pArgs[2];
			const std::wstring maxWide = pArgs[3];
			const std::wstring pathWide = pArgs[4];

			TexturePreprocessor::ValidateNormalMap(std::string(pathWide.begin(), pathWide.end()), std::stof(minWide), std::stof(maxWide));

			throw std::runtime_error("Normal map validated successfully. Just kidding about that whole runtime error thing.");
		}
	}

	// 测试资源导入
	// asset import
	//Assimp::Importer imp;
	//auto model = imp.ReadFile("Models\\suzanne.obj",
	//	aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	// 测试动态顶点
	//TestFun();

	// 隐藏鼠标
	m_wnd.DisableCursor();

	//class Factory
	//{
	//public:
	//	Factory(Graphics& gfx)
	//		:
	//		gfx(gfx)
	//	{
	//	}

	//	std::unique_ptr<Drawable> operator()()
	//	{
	//		// 随机颜色
	//		DirectX::XMFLOAT3 materialColor = { colorDist(rng), colorDist(rng), colorDist(rng) };

	//		//return std::make_unique<TestBox>(
	//		//	gfx, 
	//		//	rng, 
	//		//	adist, 
	//		//	ddist, 
	//		//	odist, 
	//		//	rdist, 
	//		//	bdist, 
	//		//	materialColor);

	//		return std::make_unique<AssimpTest>(gfx,
	//			rng,
	//			adist,
	//			ddist,
	//			odist,
	//			rdist, 
	//			materialColor,
	//			1.5f);
	//	}

	//private:
	//	Graphics& gfx;
	//	// 随机值
	//	std::mt19937 rng{ std::random_device{}() };
	//	std::uniform_real_distribution<float> adist{ 0.0f, PI * 2.0f };
	//	std::uniform_real_distribution<float> ddist{ 0.0f, PI * 2.0f };
	//	std::uniform_real_distribution<float> odist{ 0.0f, PI * 0.08f };
	//	std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
	//	std::uniform_real_distribution<float> bdist{ 0.4f, 3.0f };
	//	std::uniform_int_distribution<int> latdist{ 5, 20 };
	//	std::uniform_int_distribution<int> longdist{ 10, 40 };
	//	std::uniform_int_distribution<int> typedist{ 0, 3 };
	//	std::uniform_real_distribution<float> colorDist{ 0.0f, 1.0f };
	//};

	//Factory factory(m_wnd.GetGfx());
	//m_drawables.reserve(m_drawablesNum); //使vector的最小容量为m_drawablesNum
	//std::generate_n(std::back_inserter(m_drawables), m_drawablesNum, factory);

	//const auto surface = Surface::FromFile("pix_1.png");

	// 用命令行扭转法线贴图
	if (this->m_commandLine != "")
	{
		int nArgs;
		const auto pLineW = GetCommandLineW();
		const auto pArgs = CommandLineToArgvW(pLineW, &nArgs);
		if (nArgs >= 4 && std::wstring(pArgs[1]) == L"--ntwerk-rotx180")
		{
			const std::wstring pathInWide = pArgs[2];
			const std::wstring pathOutWide = pArgs[3];
			NormalMapTwerker::RotateXAxis180(
				std::string(pathInWide.begin(), pathInWide.end()),
				std::string(pathOutWide.begin(), pathOutWide.end())
			);
			throw std::runtime_error("Normal map processed successfully. Just kidding about that whole runtime error thing.");
		}
	}

	//m_testPlane.SetPos({ 1.0f, 17.0f, -1.0f });

	//m_testCube.SetPos({ 0.0f, 0.0f, 0.0f });

	m_testStripey.SetModelRootTransform(DirectX::XMMatrixTranslation(-13.5f, 6.0f, 3.5f));

	//m_TestCube1.SetModelRootTransform(DirectX::XMMatrixTranslation(-5.0f, 0.0f, 0.0f));

	m_testBluePlane.SetPos(m_camera.GetPos());

	m_testRedPlane.SetPos(m_camera.GetPos());

	// 给Graphic设置投影矩阵
	m_wnd.GetGfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.f, 9.f / 16.f, 0.5f, 400.f));
	//m_wnd.GetGfx().SetProjection(DirectX::XMMatrixPerspectiveFovRH(PI / 2.f, 4.f / 3.f, 0.5f, 100.f));

}

int XYHApp::Go()
{
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}
		DoFrame();
	}
}

XYHApp::~XYHApp()
{

}

void XYHApp::DoFrame()
{
	// 计时器显示贼卡
	//const float currentTime = m_timer.Peek();
	//std::ostringstream oss;
	//oss << "Time elapsed: " << std::setprecision(1) << std::fixed << currentTime << "s";
	//m_wnd.SetTitle(oss.str());

	//// 0到1的随机浮点数(非匀速变化)
	//const float c = sin(m_timer.Peek()) / 2.f + 0.5f;

	//// 修改后缓存的红绿色
	//m_wnd.GetGfx().ClearBuffer(c, c, 1.f);

	//// 绘制三角形
	//m_wnd.GetGfx().DrawTestTriangle(m_timer.Peek());

	m_wnd.GetGfx().BeginFrame(0.07f, 0.0f, 0.12f);
	m_wnd.GetGfx().SetCamera(m_camera.GetMatrix()); // 设置相机矩阵

	m_pointLight.Bind(m_wnd.GetGfx(), m_camera.GetMatrix());

	// 绘制模型
	//m_model.Draw(m_wnd.GetGfx());

	// 绘制用model加载的模型
	//m_TestCube1.Draw(m_wnd.GetGfx());

	//
	//m_testPlane.Draw(m_wnd.GetGfx());
	//m_testCube.Draw(m_wnd.GetGfx());

	// 绘制哥布林
	//m_gobber.Draw(m_wnd.GetGfx());

	auto delta = m_timer.Mark();

	// 绘制点光源
	m_pointLight.Draw(m_wnd.GetGfx());

	// 测试场景
	m_testSponza.Draw(m_wnd.GetGfx());

	// mipmap的测试条纹
	m_testStripey.Draw(m_wnd.GetGfx());

	// 蓝色面片
	m_testBluePlane.Draw(m_wnd.GetGfx());
	// 红色面片
	m_testRedPlane.Draw(m_wnd.GetGfx());

	while (const auto keyEvent = m_wnd.kbd.ReadKey())
	{
		if (!keyEvent->IsPressed())
		{
			continue;
		}

		switch (keyEvent->GetCode())
		{
		case VK_ESCAPE:
		{
			// 鼠标如果显示，则隐藏鼠标，并可键盘控制相机
			if (m_wnd.CursorEnabled())
			{
				m_wnd.DisableCursor();
				m_wnd.m_mouse.EnableRaw();
			}
			else // 否则显示鼠标，停止键盘控制相机
			{
				m_wnd.EnableCursor();
				m_wnd.m_mouse.DisableRaw();
			}
			break;
		}
		default:
			break;
		}
	}

	// 控制相机平移
	if (!m_wnd.CursorEnabled())
	{
		if (m_wnd.kbd.KeyIsPressed('W'))
		{
			m_camera.Translate({ 0.0f, 0.0f, delta });
		}
		if (m_wnd.kbd.KeyIsPressed('A'))
		{
			m_camera.Translate({ -delta,0.0f,0.0f });
		}
		if (m_wnd.kbd.KeyIsPressed('S'))
		{
			m_camera.Translate({ 0.0f,0.0f,-delta });
		}
		if (m_wnd.kbd.KeyIsPressed('D'))
		{
			m_camera.Translate({ delta,0.0f,0.0f });
		}
		if (m_wnd.kbd.KeyIsPressed('R'))
		{
			m_camera.Translate({ 0.0f,delta,0.0f });
		}
		if (m_wnd.kbd.KeyIsPressed('F'))
		{
			m_camera.Translate({ 0.0f,-delta,0.0f });
		}
	}

	// 控制相机旋转
	while (const auto delta = m_wnd.m_mouse.ReadRawDelta())
	{
		if (!m_wnd.CursorEnabled())
		{
			m_camera.Rotate((float)delta->x, (float)delta->y);
		}
	}

	//imgui stuff
	//	ImGui_ImplDX11_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	//ImGui::NewFrame();

	//static bool show_demo_window = true;
	//if (show_demo_window)
	//{
	//	ImGui::ShowDemoWindow(&show_demo_window);
	//}
	//ImGui::Render();
	//ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// 控制相机的UI
	m_camera.SpawnControlWindow();
	// 控制灯的UI
	m_pointLight.SpawnControlWindow();

	// 测试场景
	m_testSponza.ShowWindow(m_wnd.GetGfx(), "Sponza");

	m_testStripey.ShowWindow(m_wnd.GetGfx(), "Stripey");

	m_testBluePlane.SpawnControlWindow(m_wnd.GetGfx(), "Blue Plane");
	m_testRedPlane.SpawnControlWindow(m_wnd.GetGfx(), "Red Plane");

	//m_gobber.ShowWindow(m_wnd.GetGfx(), "gobber");

	//m_model.ShowWindow(m_wnd.GetGfx());
	//m_TestCube1.ShowWindow(m_wnd.GetGfx());

	//m_testPlane.SpawnControlWindow(m_wnd.GetGfx());

	//m_testCube.SpawnControlWindow(m_wnd.GetGfx());

	ShowRawInputWindow(); // 显示鼠标偏移

	// 将后缓存给到前缓存
	m_wnd.GetGfx().EndFrame();
}

void XYHApp::ShowRawInputWindow()
{
	while (const auto delta = m_wnd.m_mouse.ReadRawDelta())
	{
		m_rawInputX += delta->x;
		m_rawInputY += delta->y;
	}

	if (ImGui::Begin("Raw Input"))
	{
		ImGui::Text("Tally: (%d, %d)", m_rawInputX, m_rawInputY);
		ImGui::Text("Cursor: %s", m_wnd.m_mouse.RawEnabled() ? "enabled" : "disabled");
	}
	ImGui::End();
}

