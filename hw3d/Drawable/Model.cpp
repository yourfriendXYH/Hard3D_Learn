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
	// ��ȡģ���ļ�������
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
		// ģ���ļ���ȡʧ��
	}

	for (size_t i = 0u; i < pScene->mNumMeshes; ++i)
	{
		m_meshPtrs.emplace_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials, pathString, scale));
	}

	// ����ģ�ͽڵ�����
	int nextId = 0;
	m_pRoot = ParseNode(nextId, *pScene->mRootNode);
}

Model::~Model() noexcept
{

}

void Model::Submit(FrameCommander& frame) const noexcept
{
	m_pWindow->ApplyParameters();
	// �Ӹ��ڵ㿪ʼ�ύ�������� Drawable
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
	// ��ȡģ�ͽڵ��Transform����
	const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)));

	// ��ȡ�ýڵ��ģ��
	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0u; i < node.mNumMeshes; ++i)
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.emplace_back(m_meshPtrs.at(meshIdx).get());
	}

	// ������ǰ�ڵ�
	auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	// �����ӽڵ㣬�ݹ��������ȡ
	for (size_t i = 0u; i < node.mNumChildren; ++i)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
	}

	return pNode;
}
