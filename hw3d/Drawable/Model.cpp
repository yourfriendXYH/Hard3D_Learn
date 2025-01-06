#include "../assimp/include/assimp/Importer.hpp"
#include "../assimp/include/assimp/scene.h"
#include "../assimp/include/assimp/postprocess.h"
#include "Model.h"
#include "Node.h"
#include "Mesh.h"
#include "ModelWindow.h"


Model::Model(Graphics& gfx, const std::string& pathString, float scale)
	:
	m_pWindow(std::make_unique<ModelWindow>())
{
	// 读取模型文件的数据
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(pathString.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	if (nullptr == pScene)
	{
		// 模型文件读取失败
	}

	for (size_t i = 0u; i < pScene->mNumMeshes; ++i)
	{
		m_meshPtrs.emplace_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials, pathString, scale));
	}

	// 解析模型节点数据
	int nextId = 0;
	m_pRoot = ParseNode(nextId, *pScene->mRootNode);
}

Model::~Model() noexcept
{

}

void Model::Submit(FrameCommander& frame) const noexcept
{
	m_pWindow->ApplyParameters();
	// 从根节点开始提交绘制数据 Drawable
	m_pRoot->Submit(frame, DirectX::XMMatrixIdentity());
}

void Model::ShowWindow(Graphics& gfx, const char* windowName /*= nullptr*/) noexcept
{
	m_pWindow->Show(gfx, windowName, *m_pRoot);
}

void Model::SetRootTransform(DirectX::FXMMATRIX transform)
{
	m_pRoot->SetAppliedTransform(transform);
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale)
{
	return {};
}

std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node) noexcept
{
	// 获取模型节点的Transform（）
	const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)));

	// 获取该节点的模型
	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0u; i < node.mNumMeshes; ++i)
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.emplace_back(m_meshPtrs.at(meshIdx).get());
	}

	// 创建当前节点
	auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	// 遍历子节点，递归解析并获取
	for (size_t i = 0u; i < node.mNumChildren; ++i)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
	}

	return pNode;
}
