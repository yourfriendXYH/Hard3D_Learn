#include "../Drawable.h"
#include "Job.h"
#include "Step.h"

Job::Job(const class Step* pStep, const class Drawable* pDrawable)
	:
	m_pDrawable(pDrawable),
	m_pStep(pStep)
{
}

void Job::Execute(class Graphics& gfx) const noexcept
{
	// m_pDrawable->Draw(gfx);
	m_pStep->Bind(gfx);
	// gfx.DrawIndexed(m_pDrawable->GetIndexCount());
}
