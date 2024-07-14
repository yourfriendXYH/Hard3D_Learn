#include "InputLayout.h"
#include "BindableCodex.h"

namespace Bind
{
	// 输入布局的描述、顶点着色器的字节码
	InputLayout::InputLayout(Graphics& gfx, DynamicData::VertexLayout layout_in, ID3DBlob* pVextexShaderBytecode)
		:
		m_layout(std::move(layout_in))
	{
		const auto d3dLayout = m_layout.GetD3DLayout();

		GetDevice(gfx)->CreateInputLayout(
			d3dLayout.data(),
			(UINT)d3dLayout.size(),
			pVextexShaderBytecode->GetBufferPointer(),
			pVextexShaderBytecode->GetBufferSize(),
			&m_pInputLayout);
	}

	void InputLayout::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->IASetInputLayout(m_pInputLayout.Get());
	}

	std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& gfx, const DynamicData::VertexLayout& layout, ID3DBlob* pVextexShaderBytecode)
	{
		return BindCodex::Resolve<InputLayout>(gfx, layout, pVextexShaderBytecode);
	}

	std::string InputLayout::GenerateUID(const DynamicData::VertexLayout& layout, ID3DBlob* pVextexShaderBytecode)
	{
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout.GetCode(); // GetCode是模板Map中的字符串
	}

	std::string InputLayout::GetUID() const noexcept
	{
		return GenerateUID(m_layout);
	}
}

