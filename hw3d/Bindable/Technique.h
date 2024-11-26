#pragma once
#include <vector>
#include "Step.h"

class Technique
{
public:
	// Ã·Ωª
	void Submit(class FrameCommander& frame, const class Drawable& drawable) const noexcept;

	void AddStep(Step step) noexcept
	{
		m_steps.emplace_back(std::move(step));
	}

	void Activate() noexcept
	{
		m_active = true;
	}

	void Deactivate() noexcept
	{
		m_active = false;
	}

	void InitializeParentReferences(const class Drawable& parent) noexcept;

private:
	bool m_active = true;
	std::vector<Step> m_steps;
};