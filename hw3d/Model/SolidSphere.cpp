#include "SolidSphere.h"
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

SolidSphere::SolidSphere(Graphics& gfx, float radius)
{
	using namespace Bind;

	auto model = Sphere::Make();

	model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));
	// ���㻺��
	const auto geometryTag = "$sphere." + std::to_string(radius);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.m_vertices));
	// ��������
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.m_indices));

	// ������ɫ��
	auto pVertexShader = VertexShader::Resolve(gfx, "SolidVS.cso");
	auto pVertexShaderByteCode = pVertexShader->GetByteCode();
	AddBind(std::move(pVertexShader));
	// ������ɫ��
	AddBind(PixelShader::Resolve(gfx, "SolidPS.cso"));

	// ������ɫ�� �õ��� ��������
	struct PSColorConstant
	{
		DirectX::XMFLOAT3 m_color = { 1.0f, 1.0f, 1.0f };
		float m_padding;
	} colorConstant;
	AddBind(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConstant, 1u));

	// ������ɫ�� ����Ĳ���
	AddBind(InputLayout::Resolve(gfx, model.m_vertices.GetVertexLayout(), pVertexShaderByteCode));

	// ͼԪ����
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	// ����任
	AddBind(std::make_shared<TransformCBuf>(gfx, *this));

	AddBind(Blender::Resolve(gfx, false));
	AddBind(Rasterizer::Resolve(gfx, false));

	AddBind(std::make_shared<Stencil>(gfx, Stencil::Mode::Off));
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
}

void SolidSphere::SetPos(const DirectX::XMFLOAT3& pos) noexcept
{
	m_position = pos;
}
