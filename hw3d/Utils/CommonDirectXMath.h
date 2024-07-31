#pragma once
#include <DirectXMath.h>

// 提取欧拉角
DirectX::XMFLOAT3 ExtractEulerAngles(const DirectX::XMFLOAT4X4& matrix);

// 提取平移向量
DirectX::XMFLOAT3 ExtractTranslation(const DirectX::XMFLOAT4X4& matrix);