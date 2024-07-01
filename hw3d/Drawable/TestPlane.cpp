#include "TestPlane.h"
#include "../Geometry/Plane.h"
#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../Texture.h"
#include "../VertexShader.h"
#include "../PixelShader.h"
#include "../ConstantBuffers.h"
#include "../InputLayout.h"
#include "../Topology.h"
#include "../TransformCBuf.h"

TestPlane::TestPlane(Graphics& gfx, float size)
{
	using namespace Bind;
	auto model = Plane::Make();
	model.Transform(DirectX::XMMatrixScaling(size, size, 1.0f));

	const auto geometryTag = "$plane." + std::to_string(size);
	// 顶点缓存
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.m_vertices));
	// 索引缓存
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.m_indices));
	// 纹理资源
	AddBind(Texture::Resolve(gfx, "Resources/Wall/brickwall.jpg"));

	// 顶点着色器
	auto spVertexShader = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pVertexShaderByteCode = spVertexShader->GetByteCode();
	AddBind(std::move(spVertexShader));
	// 像素着色器
	AddBind(PixelShader::Resolve(gfx, "PhongPS.cso"));
	
	// 像素着色器常量缓存
	struct PSMaterialConstant
	{
		float specularIntensity = 0.8f;
		float specularPower = 45.0f;
		float padding[2]; // 内存对齐（四字节对齐）
	} pmc;
	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));

	// 顶点输入布局
	AddBind(InputLayout::Resolve(gfx, model.m_vertices.GetVertexLayout(), pVertexShaderByteCode));
	// 图元拓扑
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	// 顶点变换
	AddBind(std::make_shared<TransformCBuf>(gfx, *this));
}

void TestPlane::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->m_pos = pos;
}

void TestPlane::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->m_roll = roll;
	this->m_pitch = pitch;
	this->m_yaw = yaw;
}

DirectX::XMMATRIX TestPlane::GetTransformXM() const noexcept
{
	// 先旋转再平移（自身旋转）
	return DirectX::XMMatrixRotationRollPitchYaw(m_roll, m_pitch, m_yaw) * 
		DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
}
