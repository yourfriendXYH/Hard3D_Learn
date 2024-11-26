#pragma once
#include "Bindable.h"
#include "DynamicData/DynamicVertex.h"
#include <memory>

namespace Bind
{
	//  ‰»Î≤ºæ÷
	class InputLayout : public Bindable
	{
	public:
		InputLayout(Graphics& gfx, DynamicData::VertexLayout layout_in, ID3DBlob* pVextexShaderBytecode);
		void Bind(Graphics& gfx) noexcept override;

		const DynamicData::VertexLayout GetLayout() const noexcept;

		static std::shared_ptr<InputLayout> Resolve(Graphics& gfx, const DynamicData::VertexLayout& layout, ID3DBlob* pVextexShaderBytecode);
		static std::string GenerateUID(const DynamicData::VertexLayout& layout, ID3DBlob* pVextexShaderBytecode = nullptr);
		std::string GetUID() const noexcept;

	protected:
		DynamicData::VertexLayout m_layout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
	};
}

