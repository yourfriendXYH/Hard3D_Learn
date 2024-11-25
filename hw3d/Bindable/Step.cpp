#include "Step.h"

void Step::AddBindable(std::shared_ptr<Bindable> pBindable_in) noexcept
{
	m_bindables.emplace_back(std::move(pBindable_in));
}

void Step::Submit(class FrameCommander& frame, const class Drawable& drawable) const
{
	// 接收提交
	frame;
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
		pBindable;
	}
}
