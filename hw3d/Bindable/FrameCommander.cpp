#include "FrameCommander.h"

void FrameCommander::Accept(Job job, size_t target) noexcept
{
	m_passes[target].Accept(job);
}

void FrameCommander::Execute(Graphics& gfx) const noexcept
{

}

void FrameCommander::Reset() noexcept
{
	for (auto& pass : m_passes)
	{
		pass.Reset();
	}
}
