#include "TransformCBuf.h"

namespace Bind
{
	std::unique_ptr<VertexConstantBuffer<TransformCBuf::Transforms>> TransformCBuf::m_vertexConstantBuffer;

	TransformCBuf::TransformCBuf(Graphics& gfx, const Drawable& parent, UINT slot)
		:
		m_parent(parent)
	{
		if (nullptr == m_vertexConstantBuffer)
		{
			m_vertexConstantBuffer = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}
	}

	void TransformCBuf::Bind(Graphics& gfx) noexcept
	{
		const auto model = m_parent.GetTransformXM();
		const auto modelView = m_parent.GetTransformXM() * gfx.GetCamera();
		const Transforms transform =
		{
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(m_parent.GetTransformXM() * gfx.GetCamera() * gfx.GetProjection())
		};
		m_vertexConstantBuffer->Update(gfx, transform);
		m_vertexConstantBuffer->Bind(gfx);
	}
}



