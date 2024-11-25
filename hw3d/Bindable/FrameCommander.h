#pragma once
#include <array>
#include "Pass.h"

class FrameCommander
{
public:
	void Accept(Job job, size_t target) noexcept;

	void Execute(Graphics& gfx) const noexcept;

	// ������ж���
	void Reset() noexcept;

private:
	std::array<Pass, 3> m_passes;
};