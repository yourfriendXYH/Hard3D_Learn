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
#include "../Bindable/TransformCBufDoubleboi.h"
#include "../imgui/imgui.h"

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
	AddBind(Texture::Resolve(gfx, "Resources/Wall/brickwall_normal.jpg", 2u));

	// 顶点着色器
	auto spVertexShader = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pVertexShaderByteCode = spVertexShader->GetByteCode();
	AddBind(std::move(spVertexShader));
	// 像素着色器
	AddBind(PixelShader::Resolve(gfx, "PhongPSNormalMap.cso"));

	// 像素着色器常量缓存
	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, m_pmc, 1u));

	// 顶点输入布局
	AddBind(InputLayout::Resolve(gfx, model.m_vertices.GetVertexLayout(), pVertexShaderByteCode));
	// 图元拓扑
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	// 顶点变换
	AddBind(std::make_shared<TransformCBufDoubleboi>(gfx, *this, 0u, 2u));
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

void TestPlane::SpawnControlWindow(Graphics& gfx) noexcept
{
	if (ImGui::Begin("Plane"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &m_pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &m_pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &m_pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &m_roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &m_pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &m_yaw, -180.0f, 180.0f);
		ImGui::Text("Shading");
		bool changed0 = ImGui::SliderFloat("Spec. Int.", &m_pmc.m_specularIntensity, 0.0f, 1.0f);
		bool changed1 = ImGui::SliderFloat("Spec. Power", &m_pmc.m_specularPower, 0.0f, 100.0f);
		bool checkState = m_pmc.m_normalMappingEnabled == TRUE;
		bool changed2 = ImGui::Checkbox("Enable Normal Map", &checkState);
		m_pmc.m_normalMappingEnabled = checkState ? TRUE : FALSE;
		if (changed0 || changed1 || changed2)
		{
			QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, m_pmc);
		}
	}
	ImGui::End();
}
