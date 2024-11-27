#include "TransformCBuf.h"

namespace Bind
{
	std::unique_ptr<VertexConstantBuffer<TransformCBuf::Transforms>> TransformCBuf::m_vertexConstantBuffer;

	TransformCBuf::TransformCBuf(Graphics& gfx, UINT slot)
	{
		if (nullptr == m_vertexConstantBuffer)
		{
			m_vertexConstantBuffer = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}
	}

	void TransformCBuf::Bind(Graphics& gfx) noexcept
	{
		UpdateBindImpl(gfx, GetTransforms(gfx));
	}

	void TransformCBuf::InitializeParentReference(const Drawable& parent) noexcept
	{
		m_parent = &parent;
	}

	void TransformCBuf::UpdateBindImpl(Graphics& gfx, const Transforms& tranforms) noexcept
	{
		m_vertexConstantBuffer->Update(gfx, tranforms);
		m_vertexConstantBuffer->Bind(gfx);
	}

	Bind::TransformCBuf::Transforms TransformCBuf::GetTransforms(Graphics& gfx) noexcept
	{
		assert(m_parent != nullptr);
		const auto modelView = m_parent->GetTransformXM() * gfx.GetCamera();
		return { DirectX::XMMatrixTranspose(modelView), DirectX::XMMatrixTranspose(modelView * gfx.GetProjection()) };
	}

}



