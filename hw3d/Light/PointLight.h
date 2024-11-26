#pragma once

#include "../Graphics.h"
#include "../ConstantBuffers.h"
#include "../Model/SolidSphere.h"

using namespace Bind;

class PointLight
{
public:
	PointLight(Graphics& gfx, float radius = 0.5f);

public:
	void SpawnControlWindow();

	// void Draw(Graphics& gfx) noexcept;

	void Submit(class FrameCommander& frame) const noexcept;

	void Bind(Graphics& gfx, DirectX::XMMATRIX cameraMatrix) noexcept;

private:
	void Reset() noexcept;

private:
	struct PointLightCBuf
	{
		alignas(16) DirectX::XMFLOAT3 m_position; // 点光源位置
		alignas(16) DirectX::XMFLOAT3 m_ambient;	// 环境光颜色
		alignas(16) DirectX::XMFLOAT3 m_diffuseColor;	// 漫反射颜色
		float m_diffuseIntensity;	// 漫反射强度
		float m_attQuad;
		float m_attLin;
		float m_attConst;
	};

private:
	PointLightCBuf m_cbufData; // 点光源常量缓存
	mutable SolidSphere m_mesh;
	mutable PixelConstantBuffer<PointLightCBuf> m_cBuf;
};