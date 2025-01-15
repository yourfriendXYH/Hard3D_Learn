#pragma once
#include "..\Graphics.h"
#include "..\PixelShader.h"
#include "..\ConstantBuffers.h"


class BlurPack
{
public:
	BlurPack(Graphics& gfx, int radius = 7, float sigma = 2.6f, const char* shader = "Blur_PS.cso");

public:
	void Bind(Graphics& gfx);

	void SetHorizontal(Graphics& gfx);

	void SetVertical(Graphics& gfx);

	// �����˹�ֲ���Ȩ��ֵ
	// radius��ģ�����ذ뾶
	void SetKernelGauss(Graphics& gfx, int radius, float sigma) noexcept;

	// ƽ��Ȩ��
	void SetKernelBox(Graphics& gfx, int radius) noexcept;

	void RenderWidget(Graphics& gfx);

private:

	// ģ���㷨����
	enum class KernelType
	{
		Gauss, // ��˹ģ��
		Box,
	};

	static constexpr int m_maxRadius = 15; // ����ģ���뾶
	KernelType m_kernelType = KernelType::Gauss; // ģ������

	// ��˹�ֲ���Ȩ������
	struct Kernel
	{
		int m_nTaps; // Ȩ������
		float m_padding[3];
		DirectX::XMFLOAT4 m_coefficients[m_maxRadius * 2 + 1]; // Ȩ��ֵ
	};

	struct Control
	{
		BOOL m_horizontal;
		float m_padding[3];
	};

	Bind::PixelShader m_shader;
	Bind::PixelConstantBuffer<Kernel> m_pcb;
	Bind::PixelConstantBuffer<Control> m_ccb;

	int m_radius; // ģ���뾶
	float m_sigma;
};