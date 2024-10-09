#include "PointLight.h"
#include "../imgui/imgui.h"

PointLight::PointLight(Graphics& gfx, float radius /*= 0.0f*/)
	:
	m_mesh(gfx, radius),
	m_cBuf(gfx)
{
	Reset();
}

void PointLight::SpawnControlWindow()
{
	if (ImGui::Begin("Light"))
	{
		// 灯光位置
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &m_cbufData.m_position.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &m_cbufData.m_position.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &m_cbufData.m_position.z, -60.0f, 60.0f, "%.1f");

		// 光强/颜色
		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &m_cbufData.m_diffuseIntensity, 0.01f, 2.0f, "%.2f");
		ImGui::ColorEdit3("Diffuse Color", &m_cbufData.m_diffuseColor.x);
		ImGui::ColorEdit3("Ambient", &m_cbufData.m_ambient.x);

		// 点光源常量
		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &m_cbufData.m_attConst, 0.05f, 10.0f, "%.2f");
		ImGui::SliderFloat("Linear", &m_cbufData.m_attLin, 0.0001f, 4.0f, "%.4f");
		ImGui::SliderFloat("Quadratic", &m_cbufData.m_attQuad, 0.0000001f, 10.0f, "%.7f");


		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	m_cbufData = {
		{ 2.0f,9.0f,0.0f },
		{ 0.4f,0.4f,0.4f },
		{ 1.0f,1.0f,1.0f },
		1.0f,
		0.0075f,
		0.045f,
		1.0f,
	};
}

void PointLight::Draw(Graphics& gfx) noexcept
{
	m_mesh.SetPos(m_cbufData.m_position);
	m_mesh.Draw(gfx);
}

void PointLight::Bind(Graphics& gfx, DirectX::XMMATRIX cameraMatrix) noexcept
{
	auto copyCBufData = m_cbufData;
	auto posFloat3 = DirectX::XMLoadFloat3(&copyCBufData.m_position);
	DirectX::XMStoreFloat3(&copyCBufData.m_position, DirectX::XMVector3Transform(posFloat3, cameraMatrix));

	m_cBuf.Update(gfx, copyCBufData);
	m_cBuf.Bind(gfx);
}

