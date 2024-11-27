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

		// ������任�󶨵�������ɫ����
		void Bind(Graphics& gfx) noexcept override;

		void InitializeParentReference(const Drawable& parent) noexcept override;

	protected:
		// ������ɫ�� ����������
		void UpdateBindImpl(Graphics& gfx, const Transforms& tranforms) noexcept;

		// ��ȡ������ɫ���ϵľ���
		Transforms GetTransforms(Graphics& gfx) noexcept;

	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> m_vertexConstantBuffer;

		const Drawable* m_parent = nullptr;
	};
}

