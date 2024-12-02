#include "Material.h"

DynamicData::VerticesBuffer Material::ExtractVertices(const aiMesh& mesh) const noexcept
{
	DynamicData::VerticesBuffer buf(m_vtxLayout);
	return buf;
}

std::vector<Technique> Material::GetTechniques() const noexcept
{
	return m_techniques;
}
