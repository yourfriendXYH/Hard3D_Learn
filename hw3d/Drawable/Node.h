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

	// 设置和获取模型自身坐标系的transform
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;

	int GetId() const noexcept;

	// 当前模型的UI节点
	void ShowTree(Node*& pSelectedNode) const noexcept;

	// 是否有模型子节点
	bool HasChild() const noexcept;

	// 显示控制管线数据的UI???
	void Accetp(class ModelProbe& probe);

	// 获取模型节点名
	const std::string& GetName() const;

private:
	void AddChild(std::unique_ptr<Node> pChild) noexcept;

private:
	std::string m_name; // 节点名
	int m_id; // 节点ID
	std::vector<std::unique_ptr<Node>> m_childPtrs; // 子节点
	std::vector<Mesh*> m_meshPtrs; // 当前节点挂载的模型

	DirectX::XMFLOAT4X4 m_transform; // 世界坐标系的变换
	DirectX::XMFLOAT4X4 m_appliedTransform; // 自身坐标系的变换？
};