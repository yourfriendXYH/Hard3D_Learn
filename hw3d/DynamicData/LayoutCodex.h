#pragma once

#include <unordered_map>
#include <memory>
#include "DynamicConstant.h"

// �������沼�ֵ��ֵ�(��BindableCodex����)
// ���ⴴ����ͬ�Ĳ���

namespace DynamicData
{
	class LayoutCodex
	{
	public:
		static Layout Resolve(Layout& layout) noexcept;

	private:
		static LayoutCodex& Get_() noexcept;

	private:
		std::unordered_map<std::string, std::shared_ptr<DynamicData::LayoutElement>> m_map;
	};
}
