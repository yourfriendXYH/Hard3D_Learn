#pragma once
#include "..\Graphics.h"
#include "..\PixelShader.h"
#include "..\ConstantBuffers.h"


class BlurPack
{
public:
	BlurPack(Graphics& gfx, int radius = 7, float sigma = 2.6f);

public:
	void Bind(Graphics& gfx);

	void SetHorizontal(Graphics& gfx);

	void SetVertical(Graphics& gfx);

	// �����˹�ֲ���Ȩ��ֵ
	// radius��ģ�����ذ뾶
	void SetKernel(Graphics& gfx, int radius, float sigma);

	void ShowWindow(Graphics& gfx);

private:
	// ��˹�ֲ���Ȩ������
	struct Kernel
	{
		int m_nTaps; // Ȩ������
		float m_padding[3];
		DirectX::XMFLOAT4 m_coefficients[31]; // Ȩ��ֵ
	};

	struct Control
	{
		BOOL m_horizontal;
		float m_padding[3];
	};

	Bind::PixelShader m_shader;
	Bind::PixelConstantBuffer<Kernel> m_pcb;
	Bind::PixelConstantBuffer<Control> m_ccb;

	int m_radius;
	float m_sigma;
};