#pragma once

namespace DynamicData
{
	class BufferEx;
}

class TechniqueProbe
{
public:
	void SetTechnique(class Technique* pTechnique_in);

	void SetStep(class Step* pStep_in);

	virtual bool VisitBuffer(DynamicData::BufferEx& buffer) = 0;

protected:
	virtual void OnSetTechnique() {};
	virtual void OnSetStep() {};

protected:
	class Technique* m_pTechnique = nullptr;
	class Step* m_pStep = nullptr;
};