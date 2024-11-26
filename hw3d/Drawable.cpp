#include "Drawable.h"
#include "Bindable.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Topology.h"
#include "Bindable\FrameCommander.h"

using namespace Bind;

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
