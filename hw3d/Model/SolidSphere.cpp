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
	// 顶点缓存
	const auto geometryTag = "$sphere." + std::to_string(radius);
	m_pVertexBuffer = VertexBuffer::Resolve(gfx, geometryTag, model.m_vertices);
	// 索引缓存
	m_pIndexBuffer = IndexBuffer::Resolve(gfx, geometryTag, model.m_indices);
	// 图元拓扑
	m_pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique solid;
		Step only(0u);

		// 顶点着色器
		auto pVertexShader = VertexShader::Resolve(gfx, "SolidVS.cso");
		auto pVertexShaderByteCode = pVertexShader->GetByteCode();
		only.AddBindable(std::move(pVertexShader));
		// 像素着色器
		only.AddBindable(PixelShader::Resolve(gfx, "SolidPS.cso"));

		// 像素着色器 用到的 常数缓存
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
