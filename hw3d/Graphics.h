#pragma once
#include "XYHWin.h"
#include "XuYuHaoException.h"
#include <d3d11.h>
#include "DxgiInfoManager.h"
#include <vector>
#include <DirectXMath.h>

namespace Bind
{
	class Bindable;
}

class Graphics
{
	friend class GraphicsResource;

public:
	class Exception : public XyhException
	{
		using XyhException::XyhException;
	};
	// ����쳣
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override; //std::exception����
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT m_hr;
		std::string info;
	};
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	// �豸�Ƴ��쳣���������Ƴ���
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	};

public:
	Graphics(HWND hWndm, int width, int height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;

	void BeginFrame(float red, float green, float blue);
	// ��ǰ֡����
	void EndFrame();

	void ClearBuffer(float red, float green, float blue) noexcept;

	//void DrawTestTriangle(double rotAngle);

	void DrawIndexed(UINT count);

	// ���úͻ�ȡͶӰ����
	void SetProjection(DirectX::FXMMATRIX projectionMatrix) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;

	void SetCamera(DirectX::FXMMATRIX cameraMatrix) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

private:
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice = nullptr;	// �豸, ���ã������ڴ桢������Դ
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain = nullptr;	// ������	
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext = nullptr;	// ������, ���ã�������Ⱦ����
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView = nullptr; // ��ȾĿ��

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDSV;

	DirectX::XMMATRIX m_projMatrix;

	DirectX::XMMATRIX m_cameraMatrix;

	bool m_imguiEnabled = true;
};