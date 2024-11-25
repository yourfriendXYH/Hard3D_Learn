#pragma once
#include <vector>
#include "Step.h"

class Technique
{
public:
protected:
private:
	bool m_active = true;
	std::vector<Step> m_steps;
};