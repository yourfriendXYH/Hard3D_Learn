#include "Graphics.h"
#include "dxerr.h"
#include "GraphicsThrowMacros.h"
#include <sstream>
#include <d3dcompiler.h>
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

namespace wrl = Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib") //拥有加载着色器的功能

//#define GFX_THROW_FAILED(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, hr)
//#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))

Graphics::Graphics(HWND hWnd, int width, int height)
{
	// 交换链描述
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd/*(HWND)666666*/;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr;
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,	// 使用默认适配器
		D3D_DRIVER_TYPE_HARDWARE,	// 使用硬件设备
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&m_pSwapChain, //先释放，再取地址 ->  //m_pSwapChain.GetAddressOf();
		&m_pDevice,							  //m_pSwapChain->Release();
		nullptr,
		&m_pDeviceContext
	));

	// 获取后缓冲纹理
	//ID3D11Resource* pBackBuffer = nullptr;
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	//GFX_THROW_INFO(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	GFX_THROW_INFO(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	// 通过纹理创建渲染目标
	GFX_THROW_INFO(m_pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),
		nullptr,
		&m_pRenderTargetView
	));

	// create depth stensil texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO(m_pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

	// create view of depth stensil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;

	// 创建深度模具视图
	m_pDevice->CreateDepthStencilView(
		pDepthStencil.Get(), &descDSV, &m_pDSV
	);

	// bind depth stensil view to OM (输出合并阶段 Output Merger)
	m_pDeviceContext->OMSetRenderTargets(1u, m_pRenderTargetView.GetAddressOf(), m_pDSV.Get());


	// 配置视口
	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	// （光栅化阶段 Rasterizer Stage）
	m_pDeviceContext->RSSetViewports(1u, &vp);

	ImGui_ImplDX11_Init(m_pDevice.Get(), m_pDeviceContext.Get());
}

void Graphics::BeginFrame(float red, float green, float blue)
{
	// imgui begin frame
	if (m_imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	const float color[] = { red,green,blue,1.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView.Get(), color);
	m_pDeviceContext->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
}

void Graphics::EndFrame()
{
	if (m_imguiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif

	while (FAILED(hr = m_pSwapChain->Present(1u, 0u)))
	{
		if (DXGI_ERROR_DEVICE_REMOVED == hr)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(m_pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red, green, blue, 1.f };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView.Get(), color);
	m_pDeviceContext->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

//void Graphics::DrawTestTriangle(double rotAngle)
//{
//	namespace wrl = Microsoft::WRL;
//	HRESULT hr;
//
//	struct Vertex
//	{
//		float x;
//		float y;
//		float r;
//		float g;
//		float b;
//	};
//
//	struct OneTrangle
//	{
//		Vertex vertices[3];
//	};
//
//	const Vertex vertices[] = {
//		{0.0f, 0.5f, 1.f, 0.f, 0.f},
//		{0.5f, -0.5f, 0.f, 1.f, 0.f},
//		{-0.5f, -0.5f, 0.f, 0.f, 1.f},
//
//		{-0.3f, 0.3f, 1.f, 0.f, 0.f},
//		{0.3f, 0.3f, 0.f, 1.f, 0.f},
//		{0.0f, -0.8f, 0.f, 0.f, 1.f}
//	};
//
//	 创建并设置顶点缓存
//	D3D11_BUFFER_DESC bd = {};	//缓冲描述
//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.CPUAccessFlags = 0u;
//	bd.MiscFlags = 0u;
//	bd.ByteWidth = sizeof(vertices);
//	bd.StructureByteStride = sizeof(Vertex);
//	D3D11_SUBRESOURCE_DATA sd = {};	//子资源数据
//	sd.pSysMem = vertices;
//	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
//	GFX_THROW_INFO(m_pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
//	IA：输入装配器 Input Assembler
//	const UINT stride = sizeof(Vertex);
//	const UINT offset = 0u; // 只有顶点一种数据，偏移量为0
//	m_pDeviceContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
//
//	 创建并生成索引缓存
//	const unsigned short indices[] =
//	{
//		0, 1, 2,
//		0, 2, 3,
//		0, 4, 1,
//		2, 1, 5,
//	};
//	D3D11_BUFFER_DESC ibd = {};
//	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	ibd.Usage = D3D11_USAGE_DEFAULT;
//	ibd.CPUAccessFlags = 0u;
//	ibd.MiscFlags = 0u;
//	ibd.ByteWidth = sizeof(indices);
//	ibd.StructureByteStride = sizeof(unsigned short);
//	D3D11_SUBRESOURCE_DATA isd = {};
//	isd.pSysMem = indices;
//	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
//	GFX_THROW_INFO(m_pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));
//	m_pDeviceContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
//
//
//	 创建常量缓存，并存储矩阵
//	struct ConstantBuffer
//	{
//		struct
//		{
//			float element[4][4];
//		}transformation;
//	};
//	const ConstantBuffer cb =
//	{
//		{
//			(3.f / 4.f) * std::cos(rotAngle), std::sin(rotAngle), 0.0f, 0.0f,
//			(3.f / 4.f) * -std::sin(rotAngle), std::cos(rotAngle), 0.0f, 0.0f,
//			0.0f, 0.0f, 1.0f, 0.0f,
//			0.0f, 0.0f, 0.0f, 1.0f,
//		}
//	};
//	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
//	D3D11_BUFFER_DESC cbd = {};
//	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	cbd.Usage = D3D11_USAGE_DYNAMIC;
//	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	cbd.MiscFlags = 0u;
//	cbd.ByteWidth = sizeof(cb);
//	cbd.StructureByteStride = 0u;
//	D3D11_SUBRESOURCE_DATA csd = {};
//	csd.pSysMem = &cb;
//	GFX_THROW_INFO(m_pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));
//	m_pDeviceContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
//
//	 着色器被编译成cso字节码文件
//
//	wrl::ComPtr<ID3DBlob> pBlob;
//
//	 创建像素着色器
//	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
//	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
//	GFX_THROW_INFO(m_pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
//	m_pDeviceContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);
//
//	 创建顶点着色器
//	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
//	 你可以使用narrow string，只能使用wild string
//	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
//	 创建着色器，第三个参数后面讨论
//	GFX_THROW_INFO(m_pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));
//	 将着色器绑定到管道
//	m_pDeviceContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);
//
//	创建输入布局
//	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
//	const D3D11_INPUT_ELEMENT_DESC ied[] =
//	{
//		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		{"Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8u, D3D11_INPUT_PER_VERTEX_DATA, 0}
//	};
//	GFX_THROW_INFO(m_pDevice->CreateInputLayout(ied, (UINT)std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout));
//
//	 设置输入布局
//	m_pDeviceContext->IASetInputLayout(pInputLayout.Get());
//
//	 设置渲染目标
//	m_pDeviceContext->OMSetRenderTargets(1u, m_pRenderTargetView.GetAddressOf(), nullptr);
//
//	 设置图元拓扑
//	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	 配置视口
//	D3D11_VIEWPORT vp;
//	vp.Width = 800;
//	vp.Height = 600;
//	vp.MinDepth = 0;
//	vp.MaxDepth = 1;
//	vp.TopLeftX = 0;
//	vp.TopLeftY = 0;
//	m_pDeviceContext->RSSetViewports(1u, &vp);
//
//	GFX_THROW_INFO_ONLY(m_pDeviceContext->Draw((UINT)std::size(vertices), 0u)); // 绘制三个顶点，从索引0开始
//
//	GFX_THROW_INFO_ONLY(m_pDeviceContext->DrawIndexed((UINT)std::size(indices), 0u, 0u));
//
//
//}

void Graphics::DrawIndexed(UINT count)
{
	m_pDeviceContext->DrawIndexed(count, 0u, 0u);
}

void Graphics::SetProjection(DirectX::FXMMATRIX projectionMatrix) noexcept
{
	m_projMatrix = projectionMatrix;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return m_projMatrix;
}

void Graphics::SetCamera(DirectX::FXMMATRIX cameraMatrix) noexcept
{
	m_cameraMatrix = cameraMatrix;
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return m_cameraMatrix;
}

Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	m_hr(hr)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "XYH Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return m_hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(m_hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(m_hr, buf, sizeof(buf));
	return buf;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "XYH Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}


const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Chili Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}
