#pragma once

#include <unordered_map>
#include <memory>
#include "DynamicConstant.h"

// 常数缓存布局的字典(与BindableCodex类似)
// 避免创建相同的布局

namespace DynamicData
{
	class LayoutCodex
	{
	public:
		static CookedLayout Resolve(RawLayout&& layout) noexcept;

	private:
		static LayoutCodex& Get_() noexcept;

	private:
		std::unordered_map<std::string, std::shared_ptr<DynamicData::LayoutElement>> m_map;
	};
}
