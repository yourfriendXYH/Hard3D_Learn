#include "Technique.h"
#include "TechniqueProbe.h"

Technique::Technique(std::string name) noexcept
	:
	m_name(name)
{

}

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

bool Technique::IsActive() const noexcept
{
	return m_active;
}

void Technique::SetActiveState(bool active_in) noexcept
{
	m_active = active_in;
}

void Technique::InitializeParentReferences(const class Drawable& parent) noexcept
{
	for (auto& step : m_steps)
	{
		step.InitializeParentReferences(parent);
	}
}

void Technique::Accept(TechniqueProbe& probe)
{
	probe.SetTechnique(this);
	for (auto& step : m_steps)
	{
		step.Accept(probe);
	}
}

const std::string& Technique::GetName() const noexcept
{
	return m_name;
}
