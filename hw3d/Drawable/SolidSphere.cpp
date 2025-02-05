#include "../Sphere.h"
#include "../VertexBuffer.h"
#include "../VertexShader.h"
#include "../PixelShader.h"
#include "../ConstantBuffers.h"
#include "../InputLayout.h"
#include "../Topology.h"
#include "../TransformCBuf.h"
#include "../IndexBuffer.h"
#include "../Bindable/Blender.h"
#include "../Bindable/Rasterizer.h"
#include "../Bindable/Stencil.h"
#include "SolidSphere.h"

SolidSphere::SolidSphere(Graphics& gfx, float radius)
{
	using namespace Bind;

	auto model = Sphere::Make();

	model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));
	// ���㻺��
	const auto geometryTag = "$sphere." + std::to_string(radius);
	m_pVertexBuffer = VertexBuffer::Resolve(gfx, geometryTag, model.m_vertices);
	// ��������
	m_pIndexBuffer = IndexBuffer::Resolve(gfx, geometryTag, model.m_indices);
	// ͼԪ����
	m_pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique solid;
		Step only(0u);

		// ������ɫ��
		auto pVertexShader = VertexShader::Resolve(gfx, "Solid_VS.cso");
		auto pVertexShaderByteCode = pVertexShader->GetByteCode();
		only.AddBindable(std::move(pVertexShader));
		// ������ɫ��
		only.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso"));

		// ������ɫ�� �õ��� ��������
		struct PSColorConstant
		{
			DirectX::XMFLOAT3 m_color = { 1.0f, 1.0f, 1.0f };
			float m_padding;
		} colorConstant;

		only.AddBindable(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConstant, 1u));
		only.AddBindable(InputLayout::Resolve(gfx, model.m_vertices.GetVertexLayout(), pVertexShaderByteCode));
		only.AddBindable(std::make_shared<TransformCBuf>(gfx));
		only.AddBindable(Blender::Resolve(gfx, false));
		only.AddBindable(Rasterizer::Resolve(gfx, false));
		only.AddBindable(std::make_shared<Stencil>(gfx, Stencil::Mode::Off));

		solid.AddStep(std::move(only));
		AddTechnique(std::move(solid));
	}
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
}

void SolidSphere::SetPos(const DirectX::XMFLOAT3& pos) noexcept
{
	m_position = pos;
}
