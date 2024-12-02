#pragma once
#include <filesystem>
#include "../Graphics.h"
#include "../assimp/include/assimp/scene.h"
#include "../DynamicData/DynamicVertex.h"
#include "../Bindable/Technique.h"


class Material
{
public:
	Material(Graphics& gfx, const aiMaterial* pMaterial, const std::filesystem::path& path) noexcept {};

	DynamicData::VerticesBuffer ExtractVertices(const aiMesh& mesh) const noexcept;

	std::vector<Technique> GetTechniques() const noexcept;

private:

	DynamicData::VertexLayout m_vtxLayout;
	std::vector<Technique> m_techniques;
};