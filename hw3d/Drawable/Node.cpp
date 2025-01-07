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
	// ��ģ�ͽڵ�����վ���任
	const auto built =
		DirectX::XMLoadFloat4x4(&m_appliedTransform) *
		DirectX::XMLoadFloat4x4(&m_transform) *
		accumulatedTransform;

	// �ύ����Mesh��Bindable����Ⱦ����
	for (const auto pMesh : m_meshPtrs)
	{
		pMesh->Submit(frame, built);
	}

	// �ύ�ӽڵ��Bindable����Ⱦ����
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
	// ��ǰѡ��ģ�ͽڵ��ID
	const auto selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();

	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| (GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0
		| (m_childPtrs.size() == 0u) ? ImGuiTreeNodeFlags_Leaf : 0;

	//���ɵ�ǰ�ڵ��Ӧ��ImGUI
	const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)GetId(), node_flags, m_name.c_str());

	if (ImGui::IsItemClicked()) // �������ڵ㱻ѡ��
	{
		pSelectedNode = const_cast<Node*>(this);
	}

	// ��ʾ�ӽڵ��ImGUI
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

