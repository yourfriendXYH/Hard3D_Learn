#include "BlurPack.h"
#include "../XYHMath.h"
#include "../imgui/imgui.h"

BlurPack::BlurPack(Graphics& gfx, int radius /*= 5*/, float sigma /*= 1.0f*/)
	:
	m_shader(gfx, "Blur_PS.cso"),
	m_pcb(gfx, 0u),
	m_ccb(gfx, 1u)
{
	SetKernel(gfx, radius, sigma);
}

void BlurPack::Bind(Graphics& gfx)
{
	m_shader.Bind(gfx);
	m_pcb.Bind(gfx);
	m_ccb.Bind(gfx);
}

void BlurPack::SetHorizontal(Graphics& gfx)
{
	m_ccb.Update(gfx, { TRUE });
}

void BlurPack::SetVertical(Graphics& gfx)
{
	m_ccb.Update(gfx, { FALSE });
}

void BlurPack::SetKernel(Graphics& gfx, int radius, float sigma)
{
	assert(radius <= 15);
	Kernel k;
	k.m_nTaps = radius * 2 + 1;
	float sum = 0.0f;
	for (int i = 0; i < k.m_nTaps; ++i)
	{
		const auto x = float(i - radius); // 像素坐标值
		const auto g = gauss(x, sigma); // 计算该坐标的权重值
		sum += g; // 计算权重值的总和
		k.m_coefficients[i].x = g;
	}
	// 因为权重值的总和不等于1，所以需要规范化
	for (int i = 0; i < k.m_nTaps; ++i)
	{
		k.m_coefficients[i].x /= sum;
	}

	// 更新权重值
	m_pcb.Update(gfx, k);
}

void BlurPack::ShowWindow(Graphics& gfx)
{
	ImGui::Begin("Blur");
	bool radChange = ImGui::SliderInt("Radius", &m_radius, 0, 15);
	bool sigChange = ImGui::SliderFloat("Sigma", &m_sigma, 0.1f, 10.0f);
	if (radChange || sigChange)
	{
		SetKernel(gfx, m_radius, m_sigma);
	}
	ImGui::End();
}

