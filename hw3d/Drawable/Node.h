#pragma once
#include "../Graphics.h"
#include <memory>

class Model;
class Mesh;
class FrameCommander;

// 模型节点
class Node
{
	friend Model;
public:
	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept;

	// 提交Bindable给渲染队列（第二个参数是累计的变换）
	void Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept;

	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;

	int GetId() const noexcept;

	void ShowTree(Node*& pSelectedNode) const noexcept;

private:
	void AddChild(std::unique_ptr<Node> pChild) noexcept;

private:
	std::string m_name; // 节点名
	int m_id; // 节点ID
	std::vector<std::unique_ptr<Node>> m_childPtrs;
	std::vector<Mesh*> m_meshPtrs;

	DirectX::XMFLOAT4X4 m_transform; // 世界坐标系的变换
	DirectX::XMFLOAT4X4 m_appliedTransform; // 自身坐标系的变换？
};