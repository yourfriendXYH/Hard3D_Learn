#include "Drawable.h"
#include "Bindable.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Topology.h"
#include "Bindable\FrameCommander.h"
#include "Bindable\TechniqueProbe.h"
#include "Drawable\Material.h"
#include "assimp/include/assimp/scene.h"

using namespace Bind;

Drawable::Drawable(Graphics& gfx, const Material& material, const aiMesh& mesh) noexcept
{
	m_pVertexBuffer = material.MakeVertexBindable(gfx, mesh);
	m_pIndexBuffer = material.MakeIndexBindable(gfx, mesh);
	m_pTopology = Topology::Resolve(gfx);

	for (auto& technique : material.GetTechniques())
	{
		AddTechnique(std::move(technique));
	}
}

Drawable::~Drawable()
{

}

void Drawable::AddTechnique(Technique technique_in) noexcept
{
	technique_in.InitializeParentReferences(*this);
	m_techniques.emplace_back(std::move(technique_in));
}

void Drawable::Submit(FrameCommander& frame) const noexcept
{
	for (const auto& technique : m_techniques)
	{
		technique.Submit(frame, *this);
	}
}

void Drawable::Bind(Graphics& gfx) const noexcept
{
	m_pIndexBuffer->Bind(gfx);
	m_pVertexBuffer->Bind(gfx);
	m_pTopology->Bind(gfx);
}

UINT Drawable::GetIndexCount() const noexcept
{
	return m_pIndexBuffer->GetCount();
}

void Drawable::Accept(TechniqueProbe& probe)
{
	for (auto& tech : m_techniques)
	{
		tech.Accept(probe);
	}
}
