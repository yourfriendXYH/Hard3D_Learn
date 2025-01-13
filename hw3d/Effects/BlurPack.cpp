#include "BlurPack.h"
#include "../XYHMath.h"

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
	assert(radius <= 7);
	Kernel k;
	k.m_nTaps = radius * 2 + 1;
	float sum = 0.0f;
	for (int i = 1; i < k.m_nTaps + 1; ++i)
	{
		const auto x = float(i - (radius + 1)); // ��������ֵ
		const auto g = gauss(x, sigma); // ����������Ȩ��ֵ
		sum += g; // ����Ȩ��ֵ���ܺ�
		k.m_coefficients[i].x = g;
	}
	// ��ΪȨ��ֵ���ܺͲ�����1��������Ҫ�淶��
	for (int i = 1; i < k.m_nTaps + 1; ++i)
	{
		k.m_coefficients[i].x /= sum;
	}

	// ����Ȩ��ֵ
	m_pcb.Update(gfx, k);
}

