#include "Node.h"
#include "Mesh.h"


Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept
	:
	m_id(id),
	m_name(name),
	m_meshPtrs(std::move(meshPtrs))
{
	DirectX::XMStoreFloat4x4(&m_transform, transform);
	DirectX::XMStoreFloat4x4(&m_appliedTransform, DirectX::XMMatrixIdentity());
}

void Node::Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
	// 该模型节点的最终矩阵变换
	const auto built =
		DirectX::XMLoadFloat4x4(&m_appliedTransform) *
		DirectX::XMLoadFloat4x4(&m_transform) *
		accumulatedTransform;

	// 提交自身Mesh的Bindable到渲染队列
	for (const auto pMesh : m_meshPtrs)
	{
		pMesh->Submit(frame, built);
	}

	// 提交子节点的Bindable到渲染队列
	for (const auto& pChildNode : m_childPtrs)
	{
		pChildNode->Submit(frame, built);
	}
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	DirectX::XMStoreFloat4x4(&m_appliedTransform, transform);
}

const DirectX::XMFLOAT4X4& Node::GetAppliedTransform() const noexcept
{
	return m_appliedTransform;
}

int Node::GetId() const noexcept
{
	return m_id;
}

void Node::ShowTree(Node*& pSelectedNode) const noexcept
{
	// 当前选中模型节点的ID
	const auto selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();

	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| (GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0
		| (m_childPtrs.size() == 0u) ? ImGuiTreeNodeFlags_Leaf : 0;

	//生成当前节点对应的ImGUI
	const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)GetId(), node_flags, m_name.c_str());

	if (ImGui::IsItemClicked()) // 如果自身节点被选中
	{
		pSelectedNode = const_cast<Node*>(this);
	}

	// 显示子节点的ImGUI
	if (expanded)
	{
		for (const auto& pChildNode : m_childPtrs)
		{
			pChildNode->ShowTree(pSelectedNode);
		}
		ImGui::TreePop();
	}
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept
{
	assert(pChild);
	m_childPtrs.emplace_back(std::move(pChild));
}

