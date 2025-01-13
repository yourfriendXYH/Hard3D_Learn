#pragma once
#include "..\Graphics.h"
#include "..\PixelShader.h"
#include "..\ConstantBuffers.h"


class BlurPack
{
public:
	BlurPack(Graphics& gfx, int radius = 5, float simgma = 1.0f);

public:
	void Bind(Graphics& gfx);

	void SetHorizontal(Graphics& gfx);

	void SetVertical(Graphics& gfx);

	void SetKernel(Graphics& gfx);

private:
	struct Kernel
	{
		int m_nTaps;
		DirectX::XMFLOAT4 m_coefficients[15]; // ÏµÊý
	};

	struct Control
	{
		BOOL m_horizontal;
	};

	Bind::PixelShader m_shader;
	Bind::PixelConstantBuffer<Kernel> m_pcb;
	Bind::PixelConstantBuffer<Control> m_ccb;
};