#pragma once
#include "..\Graphics.h"
#include "..\PixelShader.h"
#include "..\ConstantBuffers.h"


class BlurPack
{
public:
	BlurPack(Graphics& gfx, int radius = 5, float sigma = 1.0f);

public:
	void Bind(Graphics& gfx);

	void SetHorizontal(Graphics& gfx);

	void SetVertical(Graphics& gfx);

	// �����˹�ֲ���Ȩ��ֵ
	// radius��ģ�����ذ뾶
	void SetKernel(Graphics& gfx, int radius, float sigma);

private:
	// ��˹�ֲ���Ȩ������
	struct Kernel
	{
		int m_nTaps; // Ȩ������
		DirectX::XMFLOAT4 m_coefficients[15]; // Ȩ��ֵ
	};

	struct Control
	{
		BOOL m_horizontal;
	};

	Bind::PixelShader m_shader;
	Bind::PixelConstantBuffer<Kernel> m_pcb;
	Bind::PixelConstantBuffer<Control> m_ccb;
};