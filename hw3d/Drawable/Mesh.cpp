#include "Mesh.h"
#include "../imgui/imgui.h"
#include <unordered_map>
#include "../Surface.h"

Mesh::Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bindable>> bindPtrs)
{
	using namespace Bind;
	// 图元拓扑
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
		| ((GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0) // 当前的节点是选中的，则高亮
		| ((m_childPtrs.empty()) ? ImGuiTreeNodeFlags_Leaf : 0); // 子节点为空，则TreeNodeEx返回false

	const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)GetId(), nodeFlag, m_name.c_str());

	if (ImGui::IsItemClicked())
	{
		pSelectedNode = const_cast<Node*>(this); // 引用指针赋值
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
		// 节点索引追踪
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

	// UI中被选择的节点
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
	// 每个模型节点的自身坐标系变换
	std::unordered_map<int, TransformParameters> m_transformsMap;
};

Model::Model(Graphics& gfx, const std::string fileName)
	:
	m_pModelWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer importer;
	const auto pScene = importer.ReadFile(fileName,
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

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
	// 更新被选择节点的自身变换
	auto pNode = m_pModelWindow->GetSelectedNode();
	if (nullptr != pNode)
	{
		pNode->SetAppliedTransform(m_pModelWindow->GetTransform()); // 将UI上设置好的变换值给到节点上
	}

	// 从根节点递归绘制
	m_pRootNode->Draw(gfx, DirectX::XMMatrixIdentity());
}

void Model::ShowWindow(const char* windowName /*= nullptr*/) noexcept
{
	m_pModelWindow->Show(windowName, *m_pRootNode);
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial*const* pMaterials)
{
	using namespace Bind;

	DynamicData::VerticesBuffer vBuf(
		DynamicData::VertexLayout{}
		.Append(DynamicData::VertexLayout::Position3D)
		.Append(DynamicData::VertexLayout::Normal)
		.Append(DynamicData::VertexLayout::Texture2D)
	);
	// 获取当前网格的材质
	//auto& material = *pMaterials[mesh.mMaterialIndex];
	//for (int i = 0; i < material.mNumProperties; ++i)
	//{
	//	auto& prop = *material.mProperties[i];
	//}


	// 获取顶点数据
	for (unsigned int i = 0u; i < mesh.mNumVertices; ++i)
	{
		auto test = mesh.mVertices[i];
		vBuf.EmplaceBack(
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
		);
	}

	// 获取索引数据
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

	std::vector<std::shared_ptr<Bindable>> bindablePtrs;
	using namespace std::string_literals;
	const auto basePath = "Models\\nano_textured\\"s;
	bool hasSpecularMap = false; // 是否有镜面高光贴图
	float shininess = 35.0f; // 没有高光贴图时的镜面强度
	if (mesh.mMaterialIndex >= 0)
	{
		auto& material = *pMaterials[mesh.mMaterialIndex];
		aiString textureFileName;
		material.GetTexture(aiTextureType_DIFFUSE, 0, &textureFileName);
		// 添加纹理资源（像素着色器）
		bindablePtrs.push_back(Texture::Resolve(gfx, basePath + textureFileName.C_Str()));

		auto result = material.GetTexture(aiTextureType_SPECULAR, 0, &textureFileName);
		if (result == aiReturn_SUCCESS) // 如果有高光贴图
		{
			// 添加纹理资源（像素着色器）
			bindablePtrs.push_back(Texture::Resolve(gfx, basePath + textureFileName.C_Str(), 1));
			hasSpecularMap = true;
		}
		else
		{
			material.Get(AI_MATKEY_SHININESS, shininess);
		}

		// 纹理采样
		bindablePtrs.push_back(Sampler::Resolve(gfx));
	}

	// 顶点缓存
	const auto meshTag = basePath + "%" + mesh.mName.C_Str();
	bindablePtrs.emplace_back(VertexBuffer::Resolve(gfx, meshTag, vBuf));
	// 索引缓存
	bindablePtrs.emplace_back(IndexBuffer::Resolve(gfx, meshTag, indices));
	// 顶点着色器
	auto pVertexShader = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pVertexShaderByteCode = pVertexShader->GetByteCode();
	bindablePtrs.emplace_back(std::move(pVertexShader));

	// 输入布局
	bindablePtrs.emplace_back(InputLayout::Resolve(gfx, vBuf.GetVertexLayout(), pVertexShaderByteCode));

	if (hasSpecularMap) // 有高光贴图，则用高光贴图的数据算镜面高光
	{
		bindablePtrs.emplace_back(PixelShader::Resolve(gfx, "PhongPSSpecMap.cso"));
	}
	else // 否则用常量
	{
		bindablePtrs.emplace_back(PixelShader::Resolve(gfx, "PhongPS.cso"));
		// 像素着色器常量缓存
		struct PSMaterialConstant
		{
			float specularIntensity = 0.8f;
			float specularPower = 40.0f;
			float padding[2];
		} psMaterialConst;
		psMaterialConst.specularPower = shininess;
		bindablePtrs.emplace_back(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, psMaterialConst, 1u));
	}

	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node)
{
	// 获取Node的变换
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

	// 传入节点名、拥有的网格模型 和 变换矩阵
	auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(curMeshPtrs), nodeTransform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
	}

	return pNode;
}
