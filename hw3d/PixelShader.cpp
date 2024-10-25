#include "PixelShader.h"
#include <d3dcompiler.h>
#include "BindableCodex.h"

namespace Bind
{
	PixelShader::PixelShader(Graphics& gfx, const std::string& path)
		:
		m_path(path)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		D3DReadFileToBlob(std::wstring{ path.begin(), path.end() }.c_str(), &pBlob);
		GetDevice(gfx)->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pPixelShader);
	}

	void PixelShader::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetShader(m_pPixelShader.Get(), nullptr, 0u);
	}

	std::shared_ptr<PixelShader> PixelShader::Resolve(Graphics& gfx, const std::string& path)
	{
		return BindCodex::Resolve<PixelShader>(gfx, path);
	}

	std::string PixelShader::GenerateUID(const std::string& path)
	{
		using namespace std::string_literals;
		return typeid(PixelShader).name() + "#"s + path;
	}

	std::string PixelShader::GetUID() const noexcept
	{
		return GenerateUID(m_path);
	}
}



