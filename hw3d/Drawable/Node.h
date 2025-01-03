#pragma once
#include "../Graphics.h"
#include <memory>

class Model;
class Mesh;
class FrameCommander;

// ģ�ͽڵ�
class Node
{
	friend Model;
public:
	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept;

	// �ύBindable����Ⱦ���У��ڶ����������ۼƵı任��
	void Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept;

	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;

	int GetId() const noexcept;

	void ShowTree(Node*& pSelectedNode) const noexcept;

private:
	void AddChild(std::unique_ptr<Node> pChild) noexcept;

private:
	std::string m_name; // �ڵ���
	int m_id; // �ڵ�ID
	std::vector<std::unique_ptr<Node>> m_childPtrs;
	std::vector<Mesh*> m_meshPtrs;

	DirectX::XMFLOAT4X4 m_transform; // ��������ϵ�ı任
	DirectX::XMFLOAT4X4 m_appliedTransform; // ��������ϵ�ı任��
};