#include "Graphics.h"
#include "dxerr.h"
#include "GraphicsThrowMacros.h"
#include <sstream>
#include <d3dcompiler.h>
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "Bindable/DepthStencil.h"

namespace wrl = Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib") //拥有加载着色器的功能

//#define GFX_THROW_FAILED(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, hr)
//#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))

Graphics::Graphics(HWND hWnd, int width, int height)
	:
	m_width(width),
	m_height(height)
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

	//// create depth stensil texture
	//wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	//D3D11_TEXTURE2D_DESC descDepth = {};
	//descDepth.Width = width;
	//descDepth.Height = height;
	//descDepth.MipLevels = 1u;
	//descDepth.ArraySize = 1u;
	//descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//descDepth.SampleDesc.Count = 1u;
	//descDepth.SampleDesc.Quality = 0u;
	//descDepth.Usage = D3D11_USAGE_DEFAULT;
	//descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	//GFX_THROW_INFO(m_pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

	//// create view of depth stensil texture
	//D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	//descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//descDSV.Texture2D.MipSlice = 0u;

	//// 创建深度模具视图
	//m_pDevice->CreateDepthStencilView(
	//	pDepthStencil.Get(), &descDSV, &m_pDSV
	//);

	//// bind depth stensil view to OM (输出合并阶段 Output Merger)
	//m_pDeviceContext->OMSetRenderTargets(1u, m_pRenderTargetView.GetAddressOf(), m_pDSV.Get());


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

	const float color[] = { red,green,blue,0.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView.Get(), color);
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

UINT Graphics::GetWidth() const noexcept
{
	return m_width;
}

UINT Graphics::GetHeight() const noexcept
{
	return m_height;
}

void Graphics::BindSwapBuffer() noexcept
{
	m_pDeviceContext->OMSetRenderTargets(1u, m_pRenderTargetView.GetAddressOf(), nullptr);
}

void Graphics::BindSwapBuffer(const DepthStencil& ds) noexcept
{
	m_pDeviceContext->OMSetRenderTargets(1u, m_pRenderTargetView.GetAddressOf(), ds.m_pDepthStencilView.Get());
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
