#include "Step.h"
#include "FrameCommander.h"
#include "Job.h"
#include "TechniqueProbe.h"

void Step::AddBindable(std::shared_ptr<Bindable> pBindable_in) noexcept
{
	m_bindables.emplace_back(std::move(pBindable_in));
}

void Step::Submit(FrameCommander& frame, const Drawable& drawable) const
{
	// 接收提交
	frame.Accept(Job(this, &drawable), m_targetPass);
}

void Step::Bind(Graphics& gfx) const
{
	for (const auto& pBindable : m_bindables)
	{
		pBindable->Bind(gfx);
	}
}

void Step::InitializeParentReferences(const class Drawable& parent) noexcept
{
	for (const auto& pBindable : m_bindables)
	{
		pBindable->InitializeParentReference(parent);
	}
}

void Step::Accept(TechniqueProbe& probe)
{
	probe.SetStep(this);
	for (auto& pBindable : m_bindables)
	{
		pBindable->Accept(probe);
	}
}
