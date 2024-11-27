#include "../Drawable.h"
#include "Job.h"
#include "Step.h"

Job::Job(const Step* pStep, const Drawable* pDrawable)
	:
	m_pDrawable(pDrawable),
	m_pStep(pStep)
{
}

void Job::Execute(Graphics& gfx) const noexcept
{
	m_pDrawable->Bind(gfx);
	m_pStep->Bind(gfx);
	gfx.DrawIndexed(m_pDrawable->GetIndexCount());
}
