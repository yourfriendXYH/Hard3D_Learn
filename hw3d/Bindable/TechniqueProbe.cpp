#include "TechniqueProbe.h"
#include "Technique.h"
#include "Step.h"

void TechniqueProbe::SetTechnique(class Technique* pTechnique_in)
{
	m_pTechnique = pTechnique_in;
	++m_techIndex;
	OnSetTechnique();
}

void TechniqueProbe::SetStep(class Step* pStep_in)
{
	m_pStep = pStep_in;
	++m_stepIndex;
	OnSetStep();
}

bool TechniqueProbe::VisitBuffer(DynamicData::BufferEx& buffer)
{
	++m_bufIndex;
	return OnVisitBuffer(buffer);
}
