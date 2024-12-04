#include "Material.h"

Material::Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noexcept
	:
	m_modelPath(path.string())
{
	const auto rootPath = path.parent_path().string() + "\\";
	{
		aiString tempName;
		material.Get(AI_MATKEY_NAME, tempName);
		m_name = tempName.C_Str();
	}

}

DynamicData::VerticesBuffer Material::ExtractVertices(const aiMesh& mesh) const noexcept
{
	return { m_vtxLayout, mesh };
}

std::vector<unsigned short> Material::ExtractIndices(const aiMesh& mesh) const noexcept
{
	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0u; i < mesh.mNumFaces; ++i)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.emplace_back(face.mIndices[0]);
		indices.emplace_back(face.mIndices[1]);
		indices.emplace_back(face.mIndices[2]);
	}
	return indices;
}

std::shared_ptr<Bind::VertexBuffer> Material::MakeVertexBindable(Graphics& gfx, const aiMesh& mesh) const noexcept
{
	return Bind::VertexBuffer::Resolve(gfx, MakeMeshTag(mesh), ExtractVertices(mesh));
}

std::shared_ptr<Bind::IndexBuffer> Material::MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noexcept
{
	return Bind::IndexBuffer::Resolve(gfx, MakeMeshTag(mesh), ExtractIndices(mesh));
}

std::vector<Technique> Material::GetTechniques() const noexcept
{
	return m_techniques;
}

std::string Material::MakeMeshTag(const aiMesh& mesh) const noexcept
{
	return m_modelPath + "%" + mesh.mName.C_Str();
}
