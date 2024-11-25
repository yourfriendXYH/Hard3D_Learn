#include "Technique.h"

void Technique::Submit(class FrameCommander& frame, class Drawable& drawable) const noexcept
{
	if (m_active)
	{
		for (const auto& step : m_steps)
		{
			step;
		}
	}
}

void Technique::InitializeParentReferences(const class Drawable& parent) noexcept
{
	for (const auto& step : m_steps)
	{
		step;
	}
}
