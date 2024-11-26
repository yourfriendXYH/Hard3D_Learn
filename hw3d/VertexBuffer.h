#pragma once
#include <memory>
#include "Bindable.h"
#include "CommonType.h"
#include "DynamicData/DynamicVertex.h"

namespace Bind
{
	class VertexBuffer : public Bindable
	{
	public:
		// 静态顶点
		//template<class V>
		//VertexBuffer(Graphics& gfx, const std::vector<V>& verticesData) : m_stride(sizeof(V))
		//{
		//	// 创建并设置顶点缓存
		//	D3D11_BUFFER_DESC bd = {};	//缓冲描述
		//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		//	bd.Usage = D3D11_USAGE_DEFAULT;
		//	bd.CPUAccessFlags = 0u;
		//	bd.MiscFlags = 0u;
		//	bd.ByteWidth = UINT(sizeof(V) * verticesData.size());
		//	bd.StructureByteStride = sizeof(V);
		//	D3D11_SUBRESOURCE_DATA sd = {};	//子资源数据
		//	sd.pSysMem = verticesData.data();
		//	GetDevice(gfx)->CreateBuffer(&bd, &sd, &m_pVertexBuffer);
		//};

		// 传入动态顶点
		VertexBuffer(Graphics& gfx, const DynamicData::VerticesBuffer& verticesBuf);

		VertexBuffer(Graphics& gfx, const std::string& tag, const DynamicData::VerticesBuffer& verticesBuf);

		void Bind(Graphics& gfx) noexcept override;

		const DynamicData::VertexLayout& GetLayout() const noexcept;

		static std::shared_ptr<VertexBuffer> Resolve(Graphics& gfx, const std::string& tag, const DynamicData::VerticesBuffer& verticesBuf);
		template<typename...Ignore>
		static std::string GenerateUID(const std::string& tag, Ignore...ignore)
		{
			return GenerateUID_(tag);
		}
		std::string GetUID() const noexcept;

	private:
		static std::string GenerateUID_(const std::string& tag);

	private:
		std::string m_tag;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
		UINT m_stride;
		DynamicData::VertexLayout m_layout;
	};
}

