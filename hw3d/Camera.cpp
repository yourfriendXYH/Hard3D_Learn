#include "Camera.h"
#include "imgui/imgui.h"
#include "XYHMath.h"
#include <algorithm>

Camera::Camera() noexcept
{
	Reset();
}

// 相机矩阵的计算！！！
DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	using namespace DirectX;

	const DirectX::XMVECTOR forwardBaseVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const auto lookVector = DirectX::XMVector3Transform(forwardBaseVector,
		DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f));

	const auto cameraPosition = XMLoadFloat3(&m_pos);
	const auto cameraTarget = cameraPosition + lookVector;

	return XMMatrixLookAtLH(cameraPosition, cameraTarget, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

// 控制相机的GUI
void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &m_pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &m_pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &m_pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &m_pitch, 0.995f * -90.0f, 0.995f * 90.0f);
		ImGui::SliderAngle("Yaw", &m_yaw, -180.0f, 180.0f);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	m_pos = { 0.0f, 7.5f, 0.0f };
	m_pitch = 0.0f;
	m_yaw = PI / 2.0f;
}

void Camera::Rotate(float dx, float dy) noexcept
{
	m_yaw = wrap_angle(m_yaw + dx * m_rotationSpeed);
	m_pitch = std::clamp(m_pitch + dy * m_rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	// 往相机所看的方向平移
	DirectX::XMStoreFloat3(&translation, DirectX::XMVector3Transform(
		DirectX::XMLoadFloat3(&translation),
		DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f) *
		DirectX::XMMatrixScaling(m_travelSpeed, m_travelSpeed, m_travelSpeed)));

	m_pos = {
		m_pos.x + translation.x,
		m_pos.y + translation.y,
		m_pos.z + translation.z
	};
}

DirectX::XMFLOAT3 Camera::GetPos() const noexcept
{
	return m_pos;
}
