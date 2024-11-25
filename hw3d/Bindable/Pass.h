#pragma once
#include <vector>
#include "../Graphics.h"
#include "Job.h"


class Pass
{
public:
	void Accept(Job job) noexcept;

	void Execute(Graphics& gfx) const noexcept;

	void Reset() noexcept;

private:
	std::vector<Job> m_jobs;
};