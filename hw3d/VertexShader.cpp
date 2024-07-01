#include "VertexShader.h"
#include <d3dcompiler.h>
#include <typeinfo>
#include "BindableCodex.h"
#include "XyhUtil.h"

namespace Bind
{
	VertexShader::VertexShader(Graphics& gfx, const std::string& path)
		:
		m_path(path)
	{
		// stringתwstring
		D3DReadFileToBlob(std::wstring{ path.begin(), path.end() }.c_str(), &m_pBlob);
		GetDevice(gfx)->CreateVertexShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	}

	void VertexShader::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->VSSetShader(m_pVertexShader.Get(), nullptr, 0u);
	}

	ID3DBlob* VertexShader::GetByteCode() const noexcept
	{
		return m_pBlob.Get();
	}

	std::shared_ptr<VertexShader> VertexShader::Resolve(Graphics& gfx, const std::string& path)
	{
		return BindCodex::Resolve<VertexShader>(gfx, path);
	}

	std::string VertexShader::GenerateUID(const std::string& path)
	{
		using namespace std::string_literals;
		return typeid(VertexShader).name() + "#"s + path;
	}

	std::string VertexShader::GetUID() const noexcept
	{
		return GenerateUID(m_path);
	}
}


