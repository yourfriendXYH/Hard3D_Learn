#include "Pass.h"

void Pass::Accept(Job job) noexcept
{
	m_jobs.emplace_back(job);
}

void Pass::Execute(Graphics& gfx) const noexcept
{
	for (const auto& job : m_jobs)
	{
		job.Execute(gfx);
	}
}

void Pass::Reset() noexcept
{
	m_jobs.clear();
}
