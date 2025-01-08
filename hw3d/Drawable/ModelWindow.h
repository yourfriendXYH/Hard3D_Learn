#pragma once
#include <optional>
#include <unordered_map>
#include "..\DynamicData\DynamicConstant.h"
#include "..\Graphics.h"

class Node;

class ModelWindow
{
public:

	void Show(Graphics& gfx, const char* windowName, const Node& root) noexcept;

	void ApplyParameters() noexcept;

private:

	DirectX::XMMATRIX GetTransform() const noexcept;

	const DynamicData::BufferEx& GetMaterial() const noexcept;

	bool TransformDirty() const noexcept;

	void ResetTransformDirty() noexcept;

	bool MaterialDirty() const noexcept;

	void ResetMaterialDirty() noexcept;

	bool IsDirty() const noexcept;

private:
	struct TransformParameters
	{
		float m_roll = 0.0f;
		float m_pitch = 0.0f;
		float m_yaw = 0.0f;
		float m_x = 0.0f;
		float m_y = 0.0f;
		float m_z = 0.0f;
	};

	//struct NodeData
	//{
	//	TransformParameters m_transformParams;
	//	bool m_transformParamsDirty;
	//	std::optional<DynamicData::BufferEx> m_materialCBuf;
	//	bool m_materialCBufDirty;
	//};

	Node* m_pSelectedNode;
	std::unordered_map<int, TransformParameters> m_NodesTransform;
};