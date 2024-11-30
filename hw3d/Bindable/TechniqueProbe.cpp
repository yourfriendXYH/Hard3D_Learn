#include "TechniqueProbe.h"
#include "Technique.h"
#include "Step.h"

void TechniqueProbe::SetTechnique(class Technique* pTechnique_in)
{
	m_pTechnique = pTechnique_in;
	OnSetTechnique();
}

void TechniqueProbe::SetStep(class Step* pStep_in)
{
	m_pStep = pStep_in;
	OnSetStep();
}
