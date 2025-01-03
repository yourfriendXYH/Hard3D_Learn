#pragma once
#include "../Topology.h"
#include "../IndexBuffer.h"
#include "../TransformCBuf.h"

#include "../assimp/include/assimp/Importer.hpp"
#include "../assimp/include/assimp/scene.h"
#include "../assimp/include/assimp/postprocess.h"
#include <optional>

#include "../DynamicData/DynamicVertex.h"
#include "../VertexBuffer.h"
#include "../VertexShader.h"
#include "../PixelShader.h"
#include "../InputLayout.h"
#include "../Texture.h"
#include "../Sampler.h"
#include "../imgui/imgui.h"
#include "../DynamicData/DynamicConstant.h"
#include <type_traits>
#include <filesystem>
#include "Node.h"

// ģ���쳣��
class ModelException
{
public:

private:
	std::string m_node;
};

// ������
class Mesh : public Drawable
{
public:
	Mesh(Graphics& gfx, const class Material& mat, const aiMesh& mesh) noexcept;

	//// �����ۼƵľ���
	//void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;

	using Drawable::Drawable;

	DirectX::XMMATRIX GetTransformXM() const noexcept override;

	void Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept;

private:
	mutable DirectX::XMFLOAT4X4 m_transform; // ģ�͵ı任����
};

//// ģ�ͽڵ�
//class Node
//{
//	friend class Model;
//public:
//	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept;
//
//	// �ݹ����Mesh
//	//void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const;
//
//	void Submit(class FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept;
//
//	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
//
//	const DirectX::XMFLOAT4X4& GetAppliedTransfrom() const noexcept;
//
//	int GetId() const noexcept;
//
//	void ShowTree(Node*& pSelectedNode) const noexcept;
//
//	//const DynamicData::BufferEx* GetMaterialConstant() const noexcept;
//
//	//void SetMaterialConstant(const DynamicData::BufferEx& buf) noexcept;
//
//private:
//	// ��Modelʹ��
//	void AddChild(std::unique_ptr<Node> pChild);
//
//private:
//	int m_id;
//	std::string m_name;
//	std::vector<std::unique_ptr<Node>> m_childPtrs;	// �ӽڵ�
//	std::vector<Mesh*> m_meshPtrs;
//	DirectX::XMFLOAT4X4 m_transform;
//	DirectX::XMFLOAT4X4 m_appliedTransform;
//};

// ����ģ�Ͳ�����
class Model
{
public:
	Model(Graphics& gfx, const std::string& pathString, float scale = 1.0f);
	~Model();

public:
	//void Draw(Graphics& gfx) const;

	void Submit(FrameCommander& frame) const noexcept;

	void ShowWindow(Graphics& gfx, const char* windowName = nullptr) noexcept;
	void SetModelRootTransform(DirectX::FXMMATRIX transform);

private:
	// ��������ģ��
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale = 1.0f);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node);

private:
	std::unique_ptr<Node> m_pRootNode; // ģ�͸��ڵ�
	std::vector<std::unique_ptr<Mesh>> m_meshPtrs;
	std::unique_ptr<class ModelWindow> m_pModelWindow;
};