#pragma once
#include <memory>
#include <vector>
#include <filesystem>
#include "../Graphics.h"

class Node;
class Mesh;
class FrameCommander;

struct aiMesh;
struct aiMaterial;
struct aiNode;

class Model
{
public:
	// pathString: 模型文件的路径
	Model(Graphics& gfx, const std::string& pathString, float scale = 1.0f);
	~Model() noexcept;

	// 提交要绘制的管线数据
	void Submit(FrameCommander& frame) const noexcept;
	// 显示UI窗口
	void ShowWindow(Graphics& gfx, const char* windowName = nullptr) noexcept;
	// 设置模型根节点的transform
	void SetRootTransform(DirectX::FXMMATRIX transform);

private:
	// 解析网格模型
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale);
	// 解析模型节点
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node) noexcept;

private:
	std::unique_ptr<Node> m_pRoot; // 模型的根节点
	std::vector<std::unique_ptr<Mesh>> m_meshPtrs; // 根节点挂载的模型drawable
	std::unique_ptr<class ModelWindow> m_pWindow; // 调整模型的UI窗口
};