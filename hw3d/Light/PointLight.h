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
		alignas(16) DirectX::XMFLOAT3 m_position; // ���Դλ��
		alignas(16) DirectX::XMFLOAT3 m_ambient;	// ��������ɫ
		alignas(16) DirectX::XMFLOAT3 m_diffuseColor;	// ��������ɫ
		float m_diffuseIntensity;	// ������ǿ��
		float m_attQuad;
		float m_attLin;
		float m_attConst;
	};

private:
	PointLightCBuf m_cbufData; // ���Դ��������
	mutable SolidSphere m_mesh;
	mutable PixelConstantBuffer<PointLightCBuf> m_cBuf;
};