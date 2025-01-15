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

	// 计算高斯分布的权重值
	// radius：模糊像素半径
	void SetKernelGauss(Graphics& gfx, int radius, float sigma) noexcept;

	// 平均权重
	void SetKernelBox(Graphics& gfx, int radius) noexcept;

	void RenderWidget(Graphics& gfx);

private:

	// 模糊算法类型
	enum class KernelType
	{
		Gauss, // 高斯模糊
		Box,
	};

	static constexpr int m_maxRadius = 15; // 最大的模糊半径
	KernelType m_kernelType = KernelType::Gauss; // 模糊类型

	// 高斯分布的权重数据
	struct Kernel
	{
		int m_nTaps; // 权重数量
		float m_padding[3];
		DirectX::XMFLOAT4 m_coefficients[m_maxRadius * 2 + 1]; // 权重值
	};

	struct Control
	{
		BOOL m_horizontal;
		float m_padding[3];
	};

	Bind::PixelShader m_shader;
	Bind::PixelConstantBuffer<Kernel> m_pcb;
	Bind::PixelConstantBuffer<Control> m_ccb;

	int m_radius; // 模糊半径
	float m_sigma;
};