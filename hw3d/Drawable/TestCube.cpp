#include "TestCube.h"
#include "../Geometry/Cube.h"
#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../Texture.h"
#include "../Bindable/TransformCBufDoubleboi.h"
#include "../VertexShader.h"
#include "../PixelShader.h"
#include "../InputLayout.h"
#include "../Topology.h"
#include "../imgui/imgui.h"
#include "../Bindable/Stencil.h"
#include "../Sampler.h"

TestCube::TestCube(Graphics& gfx, float size)
{
	using namespace Bind;

	auto model = Cube::MakeIndependentTextured();
	model.Transform(DirectX::XMMatrixScaling(size, size, size));
	model.SetNormalsIndependentFlat();

	const auto geomtryTag = "$cube." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geomtryTag, model.m_vertices));
	AddBind(IndexBuffer::Resolve(gfx, geomtryTag, model.m_indices));

	AddBind(Texture::Resolve(gfx, "Resources/Wall/brickwall.jpg"));
	//AddBind(Texture::Resolve(gfx, "Resources/Wall/brickwall_normal.jpg", 1u));
	AddBind(Sampler::Resolve(gfx));

	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pvsbc = pvs->GetByteCode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "PhongPS.cso"));

	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, m_pmc, 1u));

	AddBind(InputLayout::Resolve(gfx, model.m_vertices.GetVertexLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	auto tcbdb = std::make_shared<TransformCBufDoubleboi>(gfx, *this, 0u, 2u);

	AddBind(tcbdb);
	// 写入模板标记
	AddBind(std::make_shared<Stencil>(gfx, Stencil::Mode::Write));

	// 准备描边效果的管线数据
	m_outlineEffect.emplace_back(VertexBuffer::Resolve(gfx, geomtryTag, model.m_vertices));
	m_outlineEffect.emplace_back(IndexBuffer::Resolve(gfx, geomtryTag, model.m_indices));
	pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
	pvsbc = pvs->GetByteCode();
	m_outlineEffect.emplace_back(std::move(pvs));
	m_outlineEffect.emplace_back(PixelShader::Resolve(gfx, "SolidPS.cso"));
	struct SolidColorBuffer
	{
		DirectX::XMFLOAT4 m_outlineColor = { 1.0f, 0.4f, 0.4f, 1.0f };
	}outlinaPixelBuffer;
	m_outlineEffect.emplace_back(PixelConstantBuffer<SolidColorBuffer>::Resolve(gfx, outlinaPixelBuffer, 1u));
	m_outlineEffect.emplace_back(InputLayout::Resolve(gfx, model.m_vertices.GetVertexLayout(), pvsbc));
	m_outlineEffect.emplace_back(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	m_outlineEffect.emplace_back(std::move(tcbdb));
	m_outlineEffect.emplace_back(std::make_shared<Stencil>(gfx, Stencil::Mode::Mask));
}

void TestCube::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	m_pos = pos;
}

void TestCube::SetRotation(float roll, float pitch, float yaw) noexcept
{
	m_roll = roll;
	m_pitch = pitch;
	m_yaw = yaw;
}

DirectX::XMMATRIX TestCube::GetTransformXM() const noexcept
{
	auto cubeTransform = DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *
		DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);

	if (m_outlining) // 绘制描边效果时，需要调整缩放矩阵
	{
		cubeTransform = DirectX::XMMatrixScaling(1.03f, 1.03f, 1.03f) * cubeTransform;
	}

	return cubeTransform;
}

void TestCube::SpawnControlWindow(Graphics& gfx, const char* name) noexcept
{
	if (ImGui::Begin(name))
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
		bool changed0 = ImGui::SliderFloat("Spec. Int.", &m_pmc.specularIntensity, 0.0f, 1.0f);
		bool changed1 = ImGui::SliderFloat("Spec. Power", &m_pmc.specularPower, 0.0f, 100.0f);
		bool checkState = m_pmc.normalMappingEnabled == TRUE;
		bool changed2 = ImGui::Checkbox("Enable Normal Map", &checkState);
		m_pmc.normalMappingEnabled = checkState ? TRUE : FALSE;
		if (changed0 || changed1 || changed2)
		{
			QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, m_pmc);
		}
	}
	ImGui::End();
}

void TestCube::DrawOutline(Graphics& gfx) noexcept
{
	m_outlining = true;
	for (auto& bindable : m_outlineEffect)
	{
		bindable->Bind(gfx);
	}
	gfx.DrawIndexed(QueryBindable<Bind::IndexBuffer>()->GetCount());
	m_outlining = false;
}
