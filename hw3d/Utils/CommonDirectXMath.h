#pragma once
#include <DirectXMath.h>

// ��ȡŷ����
DirectX::XMFLOAT3 ExtractEulerAngles(const DirectX::XMFLOAT4X4& matrix);

// ��ȡƽ������
DirectX::XMFLOAT3 ExtractTranslation(const DirectX::XMFLOAT4X4& matrix);