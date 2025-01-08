#pragma once
#include <filesystem>
#include "../Graphics.h"
#include "../assimp/include/assimp/scene.h"
#include "../DynamicData/DynamicVertex.h"
#include "../Bindable/Technique.h"
#include "../VertexBuffer.h"
#include "../IndexBuffer.h"


class Material
{
public:
	Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noexcept;

	// 提取顶点数据
	DynamicData::VerticesBuffer ExtractVertices(const aiMesh& mesh) const noexcept;

	// 提取顶点索引
	std::vector<unsigned short> ExtractIndices(const aiMesh& mesh) const noexcept;

	std::shared_ptr<Bind::VertexBuffer> MakeVertexBindable(Graphics& gfx, const aiMesh& mesh, float scale = 1.0f) const noexcept;

	std::shared_ptr<Bind::IndexBuffer> MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noexcept;

	std::vector<Technique> GetTechniques() const noexcept;

private:
	std::string MakeMeshTag(const aiMesh& mesh) const noexcept;

private:

	DynamicData::VertexLayout m_vtxLayout; // 动态顶点布局
	std::vector<Technique> m_techniques;
	std::string m_modelPath;
	std::string m_name;
};