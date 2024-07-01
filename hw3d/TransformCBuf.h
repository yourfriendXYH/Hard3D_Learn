#pragma once
#include "Bindable.h"
#include "Drawable.h"
#include "ConstantBuffers.h"

namespace Bind
{
	class TransformCBuf : public Bindable
	{
	protected:
		//struct Transforms
		//{
		//	DirectX::XMMATRIX m_model;
		//	DirectX::XMMATRIX m_modelView;
		//	DirectX::XMMATRIX m_modelViewProj;
		//};

	public:
		// TransformCBuf(Graphics& gfx);
		TransformCBuf(Graphics& gfx, const Drawable& parent, UINT slot = 0u);
		void Bind(Graphics& gfx) noexcept override;
		//void InitializeParentReference(const Drawable& parent) noexcept override;
	private:
		struct Transforms
		{
			DirectX::XMMATRIX model;
			DirectX::XMMATRIX modelViewProj;
		};
	private:
		// 顶点常量缓存
		// static std::unique_ptr<VertexConstantBuffer<Transforms>> m_pVertexCBuf;
		// 用同一个常量缓存更新每一个parent
		static std::unique_ptr<VertexConstantBuffer<Transforms>> m_vertexConstantBuffer;

		const Drawable& m_parent;

		// const Drawable* m_parent = nullptr;
	};
}

