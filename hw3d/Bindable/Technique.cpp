#include "Technique.h"

void Technique::Submit(class FrameCommander& frame, const Drawable& drawable) const noexcept
{
	if (m_active)
	{
		for (const auto& step : m_steps)
		{
			step.Submit(frame, drawable);
		}
	}
}

void Technique::InitializeParentReferences(const class Drawable& parent) noexcept
{
	for (auto& step : m_steps)
	{
		step.InitializeParentReferences(parent);
	}
}
