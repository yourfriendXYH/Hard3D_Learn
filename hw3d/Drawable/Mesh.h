#pragma once
#include "..\Topology.h"
#include "..\IndexBuffer.h"
#include "..\TransformCBuf.h"

#include "../assimp/include/assimp/Importer.hpp"
#include "../assimp/include/assimp/scene.h"
#include "../assimp/include/assimp/postprocess.h"
#include <optional>

#include "..\DynamicData\DynamicVertex.h"
#include "..\VertexBuffer.h"
#include "..\VertexShader.h"
#include "..\PixelShader.h"
#include "..\InputLayout.h"
#include "..\Texture.h"
#include "..\Sampler.h"
#include "../imgui/imgui.h"
#include <type_traits>

// 模型异常类
class ModelException
{
public:

private:
	std::string m_node;
};

// 网格类
class Mesh : public Drawable
{
public:
	Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bindable>> bindPtrs);

	// 传入累计的矩阵
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;

	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
	mutable DirectX::XMFLOAT4X4 m_transform; // 模型的变换矩阵
};

// 模型节点
class Node
{
	friend class Model;
public:
	struct PSMaterialConstantFullmonte
	{
		BOOL normalMapEnabled = TRUE; // 是否使用法线纹理
		BOOL specularMapEnabled = TRUE;
		BOOL hasGlossMap = FALSE;
		float specularPower = 3.1f;
		DirectX::XMFLOAT3 specularColor = { 0.75f, 0.75f, 0.75f };
		float specularMapWeight = 0.671f;
	};

	struct PSMaterialConstantNotex
	{
		DirectX::XMFLOAT4 materialColor = { 0.447970f,0.327254f,0.176283f,1.0f };
		float specularIntensity = 0.65f;
		float specularPower = 120.0f;
		float padding[2];
	};

public:
	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept;

	// 递归绘制Mesh
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const;

	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;

	int GetId() const noexcept;

	void ShowTree(Node*& pSelectedNode) const noexcept;

	// 控制节点的材质效果
	template<class T>
	bool ControlNode(Graphics& gfx, T& constant)
	{
		if (m_meshPtrs.empty())
		{
			return false;
		}

		if constexpr (std::is_same<T, PSMaterialConstantFullmonte>::value)
		{
			if (auto pcb = m_meshPtrs.front()->QueryBindable<Bind::PixelConstantBuffer<T>>())
			{
				ImGui::Text("Material");

				bool normalMapEnabled = (bool)constant.normalMapEnabled;
				ImGui::Checkbox("Norm Map", &normalMapEnabled);
				constant.normalMapEnabled = normalMapEnabled ? TRUE : FALSE;

				bool specularMapEnabled = (bool)constant.specularMapEnabled;
				ImGui::Checkbox("Spec Map", &specularMapEnabled);
				constant.specularMapEnabled = specularMapEnabled ? TRUE : FALSE;

				bool hasGlossMap = (bool)constant.hasGlossMap;
				ImGui::Checkbox("Gloss Alpha", &hasGlossMap);
				constant.hasGlossMap = hasGlossMap ? TRUE : FALSE;

				ImGui::SliderFloat("Spec Weight", &constant.specularMapWeight, 0.0f, 2.0f);

				ImGui::SliderFloat("Spec Pow", &constant.specularPower, 0.0f, 1000.0f, "%f");

				ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&constant.specularColor));

				pcb->Update(gfx, constant);
				return true;
			}
		}
		else if constexpr (std::is_same<T, PSMaterialConstantNotex>::value)
		{
			if (auto pcb = m_meshPtrs.front()->QueryBindable<Bind::PixelConstantBuffer<T>>())
			{
				ImGui::Text("Material");

				ImGui::SliderFloat("Spec Inten.", &constant.specularIntensity, 0.0f, 1.0f);

				ImGui::SliderFloat("Spec Pow", &constant.specularPower, 0.0f, 1000.0f, "%f");

				ImGui::ColorPicker3("Diff Color", reinterpret_cast<float*>(&constant.materialColor));

				pcb->Update(gfx, constant);
				return true;
			}
		}
		return false;
	}

private:
	// 给Model使用
	void AddChild(std::unique_ptr<Node> pChild);

private:
	int m_id;
	std::string m_name;
	std::vector<std::unique_ptr<Node>> m_childPtrs;	// 子节点
	std::vector<Mesh*> m_meshPtrs;
	DirectX::XMFLOAT4X4 m_transform;
	DirectX::XMFLOAT4X4 m_appliedTransform;
};

// 加载模型并绘制
class Model
{
public:
	Model(Graphics& gfx, const std::string fileName, std::string texBasePath = "");
	~Model();

public:
	void Draw(Graphics& gfx) const;

	void ShowWindow(Graphics& gfx, const char* windowName = nullptr) noexcept;
	void SetModelRootTransform(DirectX::FXMMATRIX transform);

private:
	// 创建网格模型
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node);

private:
	std::unique_ptr<Node> m_pRootNode; // 模型根节点
	std::vector<std::unique_ptr<Mesh>> m_meshPtrs;
	std::unique_ptr<class ModelWindow> m_pModelWindow;
	std::string m_textureBasePath = "";
};