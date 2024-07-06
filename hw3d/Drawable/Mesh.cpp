#include "Mesh.h"
#include "../imgui/imgui.h"
#include <unordered_map>
#include "../Surface.h"
#include <stdexcept>

Mesh::Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bindable>> bindPtrs)
{
	using namespace Bind;
	// ͼԪ����
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	for (auto& pBind : bindPtrs)
	{
		AddBind(std::move(pBind));
	}

	AddBind(std::make_shared<TransformCBuf>(gfx, *this));
}

void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
	DirectX::XMStoreFloat4x4(&m_transform, accumulatedTransform);
	Drawable::Draw(gfx);
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&m_transform);
}

Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept
	:
	m_meshPtrs(std::move(meshPtrs)),
	m_name(name),
	m_id(id)
{
	DirectX::XMStoreFloat4x4(&this->m_transform, transform);
	DirectX::XMStoreFloat4x4(&this->m_appliedTransform, DirectX::XMMatrixIdentity());
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const
{
	const auto built =
		DirectX::XMLoadFloat4x4(&this->m_appliedTransform) *
		DirectX::XMLoadFloat4x4(&this->m_transform) *
		accumulatedTransform;
	for (const auto pMesh : this->m_meshPtrs)
	{
		pMesh->Draw(gfx, built);
	}
	for (const auto& node : this->m_childPtrs)
	{
		node->Draw(gfx, built);
	}
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	DirectX::XMStoreFloat4x4(&m_appliedTransform, transform);
}

int Node::GetId() const noexcept
{
	return m_id;
}

void Node::ShowTree(Node*& pSelectedNode) const noexcept
{
	const int selectedId = pSelectedNode == nullptr ? -1 : pSelectedNode->GetId();

	const auto nodeFlag = ImGuiTreeNodeFlags_OpenOnArrow
		| ((GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0) // ��ǰ�Ľڵ���ѡ�еģ������
		| ((m_childPtrs.empty()) ? ImGuiTreeNodeFlags_Leaf : 0); // �ӽڵ�Ϊ�գ���TreeNodeEx����false

	const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)GetId(), nodeFlag, m_name.c_str());

	if (ImGui::IsItemClicked())
	{
		pSelectedNode = const_cast<Node*>(this); // ����ָ�븳ֵ
	}

	if (expanded)
	{
		for (const auto& pChildNode : m_childPtrs)
		{
			pChildNode->ShowTree(pSelectedNode);
		}
		ImGui::TreePop();
	}
}

void Node::AddChild(std::unique_ptr<Node> pChild)
{
	assert(nullptr != pChild);
	this->m_childPtrs.emplace_back(std::move(pChild));
}

class ModelWindow
{
public:
	void Show(const char* windowName /*= nullptr*/, const Node& rootNode) noexcept
	{
		windowName = nullptr != windowName ? windowName : "Model";
		// �ڵ�����׷��
		int nodeIndexTracker = 0;
		if (ImGui::Begin(windowName))
		{
			ImGui::Columns(2, nullptr, true);
			rootNode.ShowTree(m_pSelectedNode);

			ImGui::NextColumn();
			if (nullptr != m_pSelectedNode)
			{
				auto& transform = m_transformsMap[m_pSelectedNode->GetId()];

				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.f, 180.f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.f, 180.f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.f, 180.f);
				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.f, 20.f);
				ImGui::SliderFloat("Y", &transform.y, -20.f, 20.f);
				ImGui::SliderFloat("Z", &transform.z, -20.f, 20.f);
			}
		}
		ImGui::End();
	}

	DirectX::XMMATRIX GetTransform() const noexcept
	{
		assert(m_pSelectedNode != nullptr);
		const auto& transform = m_transformsMap.at(m_pSelectedNode->GetId());
		return DirectX::XMMatrixRotationRollPitchYaw(transform.pitch, transform.yaw, transform.roll) *
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);

		return DirectX::XMMatrixIdentity();
	}

	// UI�б�ѡ��Ľڵ�
	Node* GetSelectedNode() const noexcept
	{
		return m_pSelectedNode;
	}

private:

	Node* m_pSelectedNode = nullptr;

	struct TransformParameters
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	// ÿ��ģ�ͽڵ����������ϵ�任
	std::unordered_map<int, TransformParameters> m_transformsMap;
};

Model::Model(Graphics& gfx, const std::string fileName, std::string texBasePath)
	:
	m_pModelWindow(std::make_unique<ModelWindow>()),
	m_textureBasePath(texBasePath)
{
	Assimp::Importer importer;
	const auto pScene = importer.ReadFile(fileName,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals | // ��ȡ��������?
		aiProcess_CalcTangentSpace); // ��ȡ���߿ռ�����

	for (size_t i = 0u; i < pScene->mNumMeshes; ++i)
	{
		//auto test = pScene->mMeshes[i];
		this->m_meshPtrs.emplace_back(std::move(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials)));
	}
	int nextId = 0;
	this->m_pRootNode = ParseNode(nextId, *pScene->mRootNode);
}

Model::~Model()
{

}

void Model::Draw(Graphics& gfx) const
{
	// ���±�ѡ��ڵ������任
	auto pNode = m_pModelWindow->GetSelectedNode();
	if (nullptr != pNode)
	{
		pNode->SetAppliedTransform(m_pModelWindow->GetTransform()); // ��UI�����úõı任ֵ�����ڵ���
	}

	// �Ӹ��ڵ�ݹ����
	m_pRootNode->Draw(gfx, DirectX::XMMatrixIdentity());
}

void Model::ShowWindow(const char* windowName /*= nullptr*/) noexcept
{
	m_pModelWindow->Show(windowName, *m_pRootNode);
}

void Model::SetModelRootTransform(DirectX::FXMMATRIX transform)
{
	m_pRootNode->SetAppliedTransform(transform);
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials)
{
	using namespace std::string_literals;
	using namespace Bind;

	std::vector<std::shared_ptr<Bindable>> bindablePtrs;
	const auto basePath = "Models\\gobber\\"s;

	bool hasDiffuseMap = false; // �Ƿ�������������
	bool hasNormalMap = false;
	bool hasSpecularMap = false;
	float shininess = 35.f;

	// ��ȡ����ģ�͵�����
	if (mesh.mMaterialIndex >= 0)
	{
		auto& material = *pMaterials[mesh.mMaterialIndex];
		aiString textureFileName;

		// ��ȡ����������
		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			// ���������Դ��������ɫ����
			bindablePtrs.push_back(Texture::Resolve(gfx, basePath + textureFileName.C_Str()));
			hasDiffuseMap = true;
		}

		// ��ȡ�߹�����
		if (material.GetTexture(aiTextureType_SPECULAR, 0, &textureFileName) == aiReturn_SUCCESS) // ����и߹���ͼ
		{
			// ���������Դ��������ɫ����
			bindablePtrs.push_back(Texture::Resolve(gfx, basePath + textureFileName.C_Str(), 1));
			hasSpecularMap = true;
		}
		else
		{
			material.Get(AI_MATKEY_SHININESS, shininess);
		}

		// ��ȡ��������
		if (material.GetTexture(aiTextureType_NORMALS, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			bindablePtrs.push_back(Texture::Resolve(gfx, basePath + textureFileName.C_Str(), 2u));
			hasNormalMap = true;
		}

		if (hasDiffuseMap || hasSpecularMap || hasNormalMap)
		{
			// �������
			bindablePtrs.push_back(Sampler::Resolve(gfx));
		}
	}

	const auto meshTag = basePath + "%" + mesh.mName.C_Str();
	const float scale = 6.0f;

	if (hasDiffuseMap && hasSpecularMap && hasNormalMap)
	{
		// ��̬�������
		DynamicData::VerticesBuffer vBuf(
			DynamicData::VertexLayout{}
			.Append(DynamicData::VertexLayout::Position3D)
			.Append(DynamicData::VertexLayout::Normal)
			.Append(DynamicData::VertexLayout::Tangent)
			.Append(DynamicData::VertexLayout::Bitangent)
			.Append(DynamicData::VertexLayout::Texture2D)
		);

		// ��ȡ��������
		for (unsigned int i = 0u; i < mesh.mNumVertices; ++i)
		{
			auto test = mesh.mVertices[i];
			vBuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

		// ��ȡ��������
		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		// ���㻺��
		bindablePtrs.emplace_back(VertexBuffer::Resolve(gfx, meshTag, vBuf));
		// ��������
		bindablePtrs.emplace_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		// ������ɫ��
		auto pVertexShader = VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
		auto pVertexShaderByteCode = pVertexShader->GetByteCode();
		bindablePtrs.emplace_back(std::move(pVertexShader));
		// ������ɫ��
		bindablePtrs.emplace_back(PixelShader::Resolve(gfx, "PhongPSSpecNormalMap.cso")); // �и߹������������ɫ��

		// ���벼��
		bindablePtrs.emplace_back(InputLayout::Resolve(gfx, vBuf.GetVertexLayout(), pVertexShaderByteCode));

		struct PSMaterialConstantFullmonte
		{
			BOOL  normalMapEnabled = TRUE; // �Ƿ�ʹ�÷�������
			float padding[3];
		} pmc;

		bindablePtrs.emplace_back(PixelConstantBuffer<PSMaterialConstantFullmonte>::Resolve(gfx, pmc, 1u));
	}
	else if (hasDiffuseMap && hasNormalMap) // û�и߹�����
	{
		DynamicData::VerticesBuffer vBuf{
			DynamicData::VertexLayout{}
			.Append(DynamicData::VertexLayout::Position3D)
			.Append(DynamicData::VertexLayout::Normal)
			.Append(DynamicData::VertexLayout::Tangent)
			.Append(DynamicData::VertexLayout::Bitangent)
			.Append(DynamicData::VertexLayout::Texture2D)
		};

		// ��ȡ��������
		for (unsigned int i = 0u; i < mesh.mNumVertices; ++i)
		{
			auto test = mesh.mVertices[i];
			vBuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

		// ��ȡ��������
		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		// ���㻺��
		bindablePtrs.emplace_back(VertexBuffer::Resolve(gfx, meshTag, vBuf));
		// ��������
		bindablePtrs.emplace_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		// ������ɫ��
		auto pVertexShader = VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
		auto pVertexShaderByteCode = pVertexShader->GetByteCode();
		bindablePtrs.emplace_back(std::move(pVertexShader));
		// ������ɫ��
		bindablePtrs.emplace_back(PixelShader::Resolve(gfx, "PhongPSNormalMap.cso")); // �и߹������������ɫ��

		// ���벼��
		bindablePtrs.emplace_back(InputLayout::Resolve(gfx, vBuf.GetVertexLayout(), pVertexShaderByteCode));

		struct PSMaterialConstantDiffNorm
		{
			float specularIntensity = 0.18f;
			float specularPower = 18;
			BOOL  normalMapEnabled = TRUE; // �Ƿ�ʹ�÷�������
			float padding[1];
		} pmc;
		pmc.specularPower = shininess;

		bindablePtrs.emplace_back(PixelConstantBuffer<PSMaterialConstantDiffNorm>::Resolve(gfx, pmc, 1u));
	}
	else if (hasDiffuseMap)
	{
		DynamicData::VerticesBuffer vBuf{
			DynamicData::VertexLayout{}
			.Append(DynamicData::VertexLayout::Position3D)
			.Append(DynamicData::VertexLayout::Normal)
			.Append(DynamicData::VertexLayout::Texture2D)
		};

		// ��ȡ��������
		for (unsigned int i = 0u; i < mesh.mNumVertices; ++i)
		{
			auto test = mesh.mVertices[i];
			vBuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

		// ��ȡ��������
		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		// ���㻺��
		bindablePtrs.emplace_back(VertexBuffer::Resolve(gfx, meshTag, vBuf));
		// ��������
		bindablePtrs.emplace_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		// ������ɫ��
		auto pVertexShader = VertexShader::Resolve(gfx, "PhongVS.cso");
		auto pVertexShaderByteCode = pVertexShader->GetByteCode();
		bindablePtrs.emplace_back(std::move(pVertexShader));
		// ������ɫ��
		bindablePtrs.emplace_back(PixelShader::Resolve(gfx, "PhongPS.cso")); // �и߹������������ɫ��

		// ���벼��
		bindablePtrs.emplace_back(InputLayout::Resolve(gfx, vBuf.GetVertexLayout(), pVertexShaderByteCode));

		struct PSMaterialConstantDiffuse
		{
			float specularIntensity = 0.18f;
			float specularPower = 18;
			float padding[1];
		} pmc;
		pmc.specularPower = shininess;

		bindablePtrs.emplace_back(PixelConstantBuffer<PSMaterialConstantDiffuse>::Resolve(gfx, pmc, 1u));
	}
	else if (!hasDiffuseMap && !hasNormalMap && !hasSpecularMap)
	{
		DynamicData::VerticesBuffer vBuf{
			DynamicData::VertexLayout{}
			.Append(DynamicData::VertexLayout::Position3D)
			.Append(DynamicData::VertexLayout::Normal)
		};

		// ��ȡ��������
		for (unsigned int i = 0u; i < mesh.mNumVertices; ++i)
		{
			auto test = mesh.mVertices[i];
			vBuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i])
			);
		}

		// ��ȡ��������
		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		// ���㻺��
		bindablePtrs.emplace_back(VertexBuffer::Resolve(gfx, meshTag, vBuf));
		// ��������
		bindablePtrs.emplace_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		// ������ɫ��
		auto pVertexShader = VertexShader::Resolve(gfx, "PhongVSNotex.cso");
		auto pVertexShaderByteCode = pVertexShader->GetByteCode();
		bindablePtrs.emplace_back(std::move(pVertexShader));
		// ������ɫ��
		bindablePtrs.emplace_back(PixelShader::Resolve(gfx, "PhongPSNotex.cso")); // �и߹������������ɫ��

		// ���벼��
		bindablePtrs.emplace_back(InputLayout::Resolve(gfx, vBuf.GetVertexLayout(), pVertexShaderByteCode));

		struct PSMaterialConstantNotex
		{
			DirectX::XMFLOAT4 materialColor = { 0.65f,0.65f,0.85f,1.0f };
			float specularIntensity = 0.18f;
			float specularPower = 18;
			float padding[2];
		} pmc;
		pmc.specularPower = shininess;

		bindablePtrs.emplace_back(PixelConstantBuffer<PSMaterialConstantNotex>::Resolve(gfx, pmc, 1u));
	}
	else
	{
		//throw std::runtime_error("û���������������");
		if (1)
		{
		}
	}

	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node)
{
	// ��ȡNode�ı任
	const auto nodeTransform = DirectX::XMMatrixTranspose(
		DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation))
	);

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(m_meshPtrs.at(meshIdx).get());
	}

	// ����ڵ�����ӵ�е�����ģ�� �� �任����
	auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(curMeshPtrs), nodeTransform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
	}

	return pNode;
}
