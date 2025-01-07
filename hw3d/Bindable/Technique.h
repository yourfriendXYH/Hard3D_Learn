#pragma once
#include <vector>
#include "Step.h"

class Technique
{
public:
	Technique() = default;
	Technique(std::string name, bool startActive = true) noexcept;

	// Ã·Ωª
	void Submit(class FrameCommander& frame, const class Drawable& drawable) const noexcept;

	void AddStep(Step step) noexcept
	{
		m_steps.emplace_back(std::move(step));
	}

	bool IsActive() const noexcept;

	void SetActiveState(bool active_in) noexcept;

	void InitializeParentReferences(const class Drawable& parent) noexcept;

	void Accept(class TechniqueProbe& probe);

	const std::string& GetName() const noexcept;

private:
	bool m_active = true;
	std::vector<Step> m_steps;
	std::string m_name = "Nameless Tech";
};