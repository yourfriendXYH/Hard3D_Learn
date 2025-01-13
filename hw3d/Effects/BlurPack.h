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

	// 计算高斯分布的权重值
	// radius：模糊像素半径
	void SetKernel(Graphics& gfx, int radius, float sigma);

private:
	// 高斯分布的权重数据
	struct Kernel
	{
		int m_nTaps; // 权重数量
		DirectX::XMFLOAT4 m_coefficients[15]; // 权重值
	};

	struct Control
	{
		BOOL m_horizontal;
	};

	Bind::PixelShader m_shader;
	Bind::PixelConstantBuffer<Kernel> m_pcb;
	Bind::PixelConstantBuffer<Control> m_ccb;
};