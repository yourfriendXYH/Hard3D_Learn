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
		TransformCBuf(Graphics& gfx, const Drawable& parent, UINT slot = 0u);

		// ������任�󶨵�������ɫ����
		void Bind(Graphics& gfx) noexcept override;

	protected:
		// ������ɫ�� ����������
		void UpdateBindImpl(Graphics& gfx, const Transforms& tranforms) noexcept;

		// ��ȡ������ɫ���ϵľ���
		Transforms GetTransforms(Graphics& gfx) noexcept;

	private:
		// ���㳣������
		// static std::unique_ptr<VertexConstantBuffer<Transforms>> m_pVertexCBuf;
		// ��ͬһ�������������ÿһ��parent
		static std::unique_ptr<VertexConstantBuffer<Transforms>> m_vertexConstantBuffer;

		const Drawable& m_parent;

		// const Drawable* m_parent = nullptr;
	};
}

