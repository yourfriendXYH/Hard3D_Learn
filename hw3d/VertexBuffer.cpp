#include "VertexBuffer.h"
#include "BindableCodex.h"

namespace Bind
{
	VertexBuffer::VertexBuffer(Graphics& gfx, const DynamicData::VerticesBuffer& verticesBuf)
		:
		VertexBuffer(gfx, "?", verticesBuf)
	{
	}

	VertexBuffer::VertexBuffer(Graphics& gfx, const std::string& tag, const DynamicData::VerticesBuffer& verticesBuf)
		:
		m_tag(tag),
		m_stride((UINT)verticesBuf.GetVertexLayout().Size())
	{
		// 创建并设置顶点缓存
		D3D11_BUFFER_DESC bd = {};	//缓冲描述
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(verticesBuf.Size());
		bd.StructureByteStride = m_stride;
		D3D11_SUBRESOURCE_DATA sd = {};	//子资源数据
		sd.pSysMem = verticesBuf.GetData();
		GetDevice(gfx)->CreateBuffer(&bd, &sd, &m_pVertexBuffer);
	}

	void VertexBuffer::Bind(Graphics& gfx) noexcept
	{
		UINT offset = 0u;
		GetContext(gfx)->IASetVertexBuffers(0u, 1u, m_pVertexBuffer.GetAddressOf(), &m_stride, &offset);
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Resolve(Graphics& gfx, const std::string& tag, const DynamicData::VerticesBuffer& verticesBuf)
	{
		assert(tag != "?");
		return BindCodex::Resolve<VertexBuffer>(gfx, tag, verticesBuf);
	}

	std::string VertexBuffer::GetUID() const noexcept
	{
		return GenerateUID(m_tag);
	}

	std::string VertexBuffer::GenerateUID_(const std::string& tag)
	{
		using namespace std::string_literals;
		return typeid(VertexBuffer).name() + "#"s + tag;
	}

}


