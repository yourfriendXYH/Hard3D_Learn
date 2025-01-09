#pragma once
#include <limits>

namespace DynamicData
{
	class BufferEx;
}

class TechniqueProbe
{
public:
	void SetTechnique(class Technique* pTechnique_in);

	void SetStep(class Step* pStep_in);

	bool VisitBuffer(DynamicData::BufferEx& buffer);

protected:
	virtual void OnSetTechnique() {};
	virtual void OnSetStep() {};

	virtual bool OnVisitBuffer(DynamicData::BufferEx& buffer)
	{
		return false;
	}

protected:
	class Technique* m_pTechnique = nullptr;
	class Step* m_pStep = nullptr;

	size_t m_techIndex = std::numeric_limits<size_t>::max();
	size_t m_stepIndex = std::numeric_limits<size_t>::max();
	size_t m_bufIndex = std::numeric_limits<size_t>::max();
};