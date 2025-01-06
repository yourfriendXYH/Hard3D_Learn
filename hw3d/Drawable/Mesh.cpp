#include "Mesh.h"
#include "../imgui/imgui.h"
#include <unordered_map>
#include "../Surface.h"
#include <stdexcept>
#include "../Bindable/Blender.h"
#include "../Bindable/Rasterizer.h"
#include "../Bindable/ConstantBuffersEx.h"
#include "../DynamicData/DynamicConstant.h"
#include "../DynamicData/LayoutCodex.h"
#include "../Bindable/Stencil.h"
#include "../Bindable/FrameCommander.h"
#include "Material.h"

Mesh::Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh) noexcept
	:
	Drawable(gfx, mat, mesh)
{

}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&m_transform);
}

void Mesh::Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
	DirectX::XMStoreFloat4x4(&m_transform, accumulatedTransform);
	Drawable::Submit(frame);
}

//class ModelWindow
//{
//public:
//	void Show(Graphics& gfx, const char* windowName /*= nullptr*/, const Node& rootNode) noexcept
//	{
//	}
//
//	DirectX::XMMATRIX GetTransform() const noexcept
//	{
//		assert(m_pSelectedNode != nullptr);
//		const auto& transform = m_transformsMap.at(m_pSelectedNode->GetId()).m_transformParams;
//		return DirectX::XMMatrixRotationRollPitchYaw(transform.pitch, transform.yaw, transform.roll) *
//			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
//
//		return DirectX::XMMatrixIdentity();
//	}
//
//	// 获取当前选择模型节点的像素常数缓存
//	const DynamicData::BufferEx* GetMaterial() const noexcept
//	{
//		assert(m_pSelectedNode != nullptr);
//		// 材质的常数缓存
//		const auto& matCBuf = m_transformsMap.at(m_pSelectedNode->GetId()).m_materialCBuf;
//		return matCBuf != std::nullopt ? &*matCBuf : nullptr;
//	}
//
//	// UI中被选择的节点
//	Node* GetSelectedNode() const noexcept
//	{
//		return m_pSelectedNode;
//	}
//
//private:
//	Node* m_pSelectedNode = nullptr;
//
//	struct TransformParameters
//	{
//		float roll = 0.0f;
//		float pitch = 0.0f;
//		float yaw = 0.0f;
//		float x = 0.0f;
//		float y = 0.0f;
//		float z = 0.0f;
//	};
//
//	struct NodeData
//	{
//		TransformParameters m_transformParams;
//		std::optional<DynamicData::BufferEx> m_materialCBuf;
//	};
//
//	// 每个模型节点的自身坐标系变换
//	std::unordered_map<int, NodeData> m_transformsMap;
//};
