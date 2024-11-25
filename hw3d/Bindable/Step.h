#pragma once
#include <memory>
#include <vector>
#include "../Bindable.h"

class Step
{
public:
	Step(size_t targetPass_in)
		:
		m_targetPass(targetPass_in)
	{
	}

	void AddBindable(std::shared_ptr<Bindable> pBindable_in) noexcept;

	void Submit(class FrameCommander& frame, const class Drawable& drawable) const;

	void Bind(Graphics& gfx) const;

	void InitializeParentReferences(const class Drawable& parent) noexcept;

private:
	size_t m_targetPass;
	std::vector<std::shared_ptr<Bindable>> m_bindables;
};
