#include "Mesh.h"
#include "../imgui/imgui.h"
#include <unordered_map>
#include "../Surface.h"
#include <stdexcept>
#include "../Bindable/Blender.h"
#include "../Bindable/Rasterizer.h"
//#include "../Utils/CommonDirectXMath.h"

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

const DirectX::XMFLOAT4X4& Node::GetAppliedTransfrom() const noexcept
{
	return m_appliedTransform;
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
	void Show(Graphics& gfx, const char* windowName /*= nullptr*/, const Node& rootNode) noexcept
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
				const auto id = m_pSelectedNode->GetId();
				auto iter = m_transformsMap.find(id);
				if (iter == m_transformsMap.end())
				{
					const auto& applied = m_pSelectedNode->GetAppliedTransfrom();
					//const auto angles = ExtractEulerAngles(applied);
					//const auto translation = ExtractTranslation(applied);
					TransformParameters transParams;
					//transParams.roll = angles.z;
					//transParams.pitch = angles.x;
					//transParams.yaw = angles.y;
					//transParams.x = translation.x;
					//transParams.y = translation.y;
					//transParams.z = translation.z;

					std::tie(iter, std::ignore) = m_transformsMap.insert({ id, transParams });

				}
				auto& transform = iter->second;
				//auto& transform = m_transformsMap[m_pSelectedNode->GetId()];

				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.f, 180.f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.f, 180.f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.f, 180.f);
				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.f, 20.f);
				ImGui::SliderFloat("Y", &transform.y, -20.f, 20.f);
				ImGui::SliderFloat("Z", &transform.z, -20.f, 20.f);

				if (!m_pSelectedNode->ControlNode(gfx, m_skinMaterialConstant))
				{
					m_pSelectedNode->ControlNode(gfx, m_ringMaterialConstant);
				}
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

	Node::PSMaterialConstantFullmonte m_skinMaterialConstant;
	Node::PSMaterialConstantNotex m_ringMaterialConstant;

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

Model::Model(Graphics& gfx, const std::string& pathString, float scale)
	:
	m_pModelWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer importer;
	const auto pScene = importer.ReadFile(pathString,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals | // 获取法线数据?
		aiProcess_CalcTangentSpace); // 获取切线空间数据

	for (size_t i = 0u; i < pScene->mNumMeshes; ++i)
	{
		//auto test = pScene->mMeshes[i];
		this->m_meshPtrs.emplace_back(std::move(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials, pathString, scale)));
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

void Model::ShowWindow(Graphics& gfx, const char* windowName /*= nullptr*/) noexcept
{
	m_pModelWindow->Show(gfx, windowName, *m_pRootNode);
}

void Model::SetModelRootTransform(DirectX::FXMMATRIX transform)
{
	m_pRootNode->SetAppliedTransform(transform);
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale)
{
	using namespace std::string_literals;
	using namespace Bind;

	const auto rootPath = path.parent_path().string() + "\\";

	std::vector<std::shared_ptr<Bindable>> bindablePtrs;

	bool hasDiffuseMap = false; // 是否有漫反射纹理
	bool hasAlphaGloss = false;
	bool hasAlphaDiffuse = false; // 漫反射纹理是否有透明度
	bool hasNormalMap = false;
	bool hasSpecularMap = false;
	float shininess = 2.0f;
	DirectX::XMFLOAT4 specularColor = { 0.18f, 0.18f, 0.18f, 1.0f };
	DirectX::XMFLOAT4 diffuseColor = { 0.45f, 0.45f, 0.85f, 1.0f };

	// 获取网格模型的纹理
	if (mesh.mMaterialIndex >= 0)
	{
		auto& material = *pMaterials[mesh.mMaterialIndex];
		aiString textureFileName;

		// 获取漫反射纹理
		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			// 添加纹理资源（像素着色器）
			auto spTexture = Texture::Resolve(gfx, rootPath + textureFileName.C_Str());
			hasAlphaDiffuse = spTexture->HasAlpha(); // 是否有透明度，是通过Surface判断的
			bindablePtrs.push_back(std::move(spTexture));

			if (hasAlphaDiffuse)
			{
				if (1)
				{
				}
			}

			hasDiffuseMap = true;
		}
		else
		{
			material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(diffuseColor));
		}

		// 获取高光纹理
		if (material.GetTexture(aiTextureType_SPECULAR, 0, &textureFileName) == aiReturn_SUCCESS) // 如果有高光贴图
		{
			// 添加纹理资源（像素着色器
			auto spTextureBind = Texture::Resolve(gfx, rootPath + textureFileName.C_Str(), 1);
			hasAlphaGloss = spTextureBind->HasAlpha();
			bindablePtrs.push_back(spTextureBind);
			hasSpecularMap = true;
		}
		else
		{
			material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(specularColor));
		}

		if (!hasAlphaGloss)
		{
			material.Get(AI_MATKEY_SHININESS, shininess);
		}

		// 获取法线纹理
		if (material.GetTexture(aiTextureType_NORMALS, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			auto spTextureBind = Texture::Resolve(gfx, rootPath + textureFileName.C_Str(), 2u);
			hasAlphaGloss = spTextureBind->HasAlpha();
			bindablePtrs.push_back(spTextureBind);
			hasNormalMap = true;
		}

		if (hasDiffuseMap || hasSpecularMap || hasNormalMap)
		{
			// 纹理采样
			bindablePtrs.push_back(Sampler::Resolve(gfx));
		}
	}

	const auto meshTag = rootPath + "%" + mesh.mName.C_Str();

	if (hasDiffuseMap && hasSpecularMap && hasNormalMap)
	{
		// 动态顶点对象
		DynamicData::VerticesBuffer vBuf(
			DynamicData::VertexLayout{}
			.Append(DynamicData::VertexLayout::Position3D)
			.Append(DynamicData::VertexLayout::Normal)
			.Append(DynamicData::VertexLayout::Tangent)
			.Append(DynamicData::VertexLayout::Bitangent)
			.Append(DynamicData::VertexLayout::Texture2D)
		);

		// 获取顶点数据
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

		// 顶点缓存
		bindablePtrs.emplace_back(VertexBuffer::Resolve(gfx, meshTag, vBuf));
		// 索引缓存
		bindablePtrs.emplace_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		// 顶点着色器
		auto pVertexShader = VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
		auto pVertexShaderByteCode = pVertexShader->GetByteCode();
		bindablePtrs.emplace_back(std::move(pVertexShader));
		// 像素着色器
		bindablePtrs.emplace_back(PixelShader::Resolve(gfx, hasDiffuseMap ? "PhongPSSpecNormalMask.cso" : "PhongPSSpecNormalMap.cso")); // 有高光纹理的像素着色器

		// 输入布局
		bindablePtrs.emplace_back(InputLayout::Resolve(gfx, vBuf.GetVertexLayout(), pVertexShaderByteCode));

		Node::PSMaterialConstantFullmonte pmc;
		pmc.specularPower = shininess;
		pmc.hasGlossMap = hasAlphaGloss ? TRUE : FALSE;

		bindablePtrs.emplace_back(PixelConstantBuffer<Node::PSMaterialConstantFullmonte>::Resolve(gfx, pmc, 1u));
	}
	else if (hasDiffuseMap && hasNormalMap) // 没有高光纹理
	{
		DynamicData::VerticesBuffer vBuf{
			DynamicData::VertexLayout{}
			.Append(DynamicData::VertexLayout::Position3D)
			.Append(DynamicData::VertexLayout::Normal)
			.Append(DynamicData::VertexLayout::Tangent)
			.Append(DynamicData::VertexLayout::Bitangent)
			.Append(DynamicData::VertexLayout::Texture2D)
		};

		// 获取顶点数据
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

		// 顶点缓存
		bindablePtrs.emplace_back(VertexBuffer::Resolve(gfx, meshTag, vBuf));
		// 索引缓存
		bindablePtrs.emplace_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		// 顶点着色器
		auto pVertexShader = VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
		auto pVertexShaderByteCode = pVertexShader->GetByteCode();
		bindablePtrs.emplace_back(std::move(pVertexShader));
		// 像素着色器
		bindablePtrs.emplace_back(PixelShader::Resolve(gfx, "PhongPSNormalMap.cso")); // 有高光纹理的像素着色器

		// 输入布局
		bindablePtrs.emplace_back(InputLayout::Resolve(gfx, vBuf.GetVertexLayout(), pVertexShaderByteCode));

		struct PSMaterialConstantDiffNorm
		{
			float specularIntensity;
			float specularPower = 18;
			BOOL  normalMapEnabled = TRUE; // 是否使用法线纹理
			float padding[1];
		} pmc;
		pmc.specularPower = shininess;
		pmc.specularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;

		bindablePtrs.emplace_back(PixelConstantBuffer<PSMaterialConstantDiffNorm>::Resolve(gfx, pmc, 1u));
	}
	else if (hasDiffuseMap && !hasNormalMap && hasSpecularMap) // 没有法线纹理
	{
		DynamicData::VerticesBuffer verticesBuffer{ std::move(
			DynamicData::VertexLayout{}
			.Append(DynamicData::VertexLayout::Position3D)
			.Append(DynamicData::VertexLayout::Normal)
			.Append(DynamicData::VertexLayout::Texture2D)
		) };

		// 顶点数据
		for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
		{
			verticesBuffer.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTextureCoords[0][i])
			);
		}

		// 索引数据
		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; ++i)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.emplace_back(face.mIndices[0]);
			indices.emplace_back(face.mIndices[1]);
			indices.emplace_back(face.mIndices[2]);
		}

		// 顶点缓存
		bindablePtrs.emplace_back(VertexBuffer::Resolve(gfx, meshTag, verticesBuffer));
		// 索引缓存
		bindablePtrs.emplace_back(IndexBuffer::Resolve(gfx, meshTag, indices));
		// 顶点着色器
		auto pVertexShader = VertexShader::Resolve(gfx, "PhongVS.cso");
		auto pVertexShaderByteCode = pVertexShader->GetByteCode();
		bindablePtrs.emplace_back(std::move(pVertexShader));
		// 像素着色器
		bindablePtrs.emplace_back(PixelShader::Resolve(gfx, "PhongPSSpec.cso"));
		// 输入布局
		bindablePtrs.emplace_back(InputLayout::Resolve(gfx, verticesBuffer.GetVertexLayout(), pVertexShaderByteCode));

		struct PSMaterialConstantDiffuseSpec
		{
			float specularPowerConst;
			BOOL hasGloss;
			float specularMapWeight;
			float padding;
		} pmc;

		pmc.specularPowerConst = shininess;
		pmc.hasGloss = hasAlphaGloss ? TRUE : FALSE;
		pmc.specularMapWeight = 1.0f;

		bindablePtrs.emplace_back(PixelConstantBuffer<PSMaterialConstantDiffuseSpec>::Resolve(gfx, pmc, 1u));
	}
	else if (hasDiffuseMap)
	{
		DynamicData::VerticesBuffer vBuf{
			DynamicData::VertexLayout{}
			.Append(DynamicData::VertexLayout::Position3D)
			.Append(DynamicData::VertexLayout::Normal)
			.Append(DynamicData::VertexLayout::Texture2D)
		};

		// 获取顶点数据
		for (unsigned int i = 0u; i < mesh.mNumVertices; ++i)
		{
			auto test = mesh.mVertices[i];
			vBuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
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

		// 顶点缓存
		bindablePtrs.emplace_back(VertexBuffer::Resolve(gfx, meshTag, vBuf));
		// 索引缓存
		bindablePtrs.emplace_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		// 顶点着色器
		auto pVertexShader = VertexShader::Resolve(gfx, "PhongVS.cso");
		auto pVertexShaderByteCode = pVertexShader->GetByteCode();
		bindablePtrs.emplace_back(std::move(pVertexShader));
		// 像素着色器
		bindablePtrs.emplace_back(PixelShader::Resolve(gfx, "PhongPS.cso")); // 有高光纹理的像素着色器

		// 输入布局
		bindablePtrs.emplace_back(InputLayout::Resolve(gfx, vBuf.GetVertexLayout(), pVertexShaderByteCode));

		struct PSMaterialConstantDiffuse
		{
			float specularIntensity;
			float specularPower = 18;
			float padding[2];
		} pmc;
		pmc.specularPower = shininess;
		pmc.specularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;

		bindablePtrs.emplace_back(PixelConstantBuffer<PSMaterialConstantDiffuse>::Resolve(gfx, pmc, 1u));
	}
	else if (!hasDiffuseMap && !hasNormalMap && !hasSpecularMap)
	{
		DynamicData::VerticesBuffer vBuf{
			DynamicData::VertexLayout{}
			.Append(DynamicData::VertexLayout::Position3D)
			.Append(DynamicData::VertexLayout::Normal)
		};

		// 获取顶点数据
		for (unsigned int i = 0u; i < mesh.mNumVertices; ++i)
		{
			auto test = mesh.mVertices[i];
			vBuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i])
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

		// 顶点缓存
		bindablePtrs.emplace_back(VertexBuffer::Resolve(gfx, meshTag, vBuf));
		// 索引缓存
		bindablePtrs.emplace_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		// 顶点着色器
		auto pVertexShader = VertexShader::Resolve(gfx, "PhongVSNotex.cso");
		auto pVertexShaderByteCode = pVertexShader->GetByteCode();
		bindablePtrs.emplace_back(std::move(pVertexShader));
		// 像素着色器
		bindablePtrs.emplace_back(PixelShader::Resolve(gfx, "PhongPSNotex.cso")); // 有高光纹理的像素着色器

		// 输入布局
		bindablePtrs.emplace_back(InputLayout::Resolve(gfx, vBuf.GetVertexLayout(), pVertexShaderByteCode));

		Node::PSMaterialConstantNotex pmc;
		pmc.specularPower = shininess;
		pmc.specularColor = specularColor;
		pmc.materialColor = diffuseColor;

		bindablePtrs.emplace_back(PixelConstantBuffer<Node::PSMaterialConstantNotex>::Resolve(gfx, pmc, 1u));
	}
	else
	{
		//throw std::runtime_error("没有正常的纹理组合");
		if (1)
		{
		}
	}

	// 所有的材质都需要混合模式
	// bindablePtrs.emplace_back(Blender::Resolve(gfx, hasAlphaDiffuse)); // 材质是否有透明度

	bindablePtrs.emplace_back(Blender::Resolve(gfx, false));

	bindablePtrs.emplace_back(Rasterizer::Resolve(gfx, hasAlphaDiffuse));

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
