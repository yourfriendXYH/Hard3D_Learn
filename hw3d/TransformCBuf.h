#pragma once
#include "Bindable.h"
#include "Drawable.h"
#include "ConstantBuffers.h"

namespace Bind
{
	class TransformCBuf : public Bindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX modelView;
			DirectX::XMMATRIX modelViewProj;
		};

	public:
		TransformCBuf(Graphics& gfx, UINT slot = 0u);

		// 将矩阵变换绑定到顶点着色器上
		void Bind(Graphics& gfx) noexcept override;

		void InitializeParentReference(const Drawable& parent) noexcept override;

	protected:
		// 更新着色器 矩阵常数缓存
		void UpdateBindImpl(Graphics& gfx, const Transforms& tranforms) noexcept;

		// 获取给到着色器上的矩阵
		Transforms GetTransforms(Graphics& gfx) noexcept;

	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> m_vertexConstantBuffer;

		const Drawable* m_parent = nullptr;
	};
}

