#pragma once

#include <unordered_map>
#include <memory>
#include "DynamicConstant.h"

// 常数缓存布局的字典(与BindableCodex类似)
// 避免创建相同的布局
class LayoutCodex
{
public:
	static bool Has(const std::string& tag)
	{
		return Get_().Has_(tag);
	}

	static DynamicData::Layout Load(const std::string& tag)
	{
		return Get_().Load_(tag);
	}

	static void Store(const std::string& tag, DynamicData::Layout& layout)
	{
		Get_().Store_(tag, layout);
	}

private:
	static LayoutCodex& Get_()
	{
		static LayoutCodex codex;
		return codex;
	}

	bool Has_(const std::string& tag) const
	{
		return m_map.find(tag) != m_map.end();
	}

	DynamicData::Layout Load_(const std::string& tag) const
	{
		return { m_map.find(tag)->second };
	}

	void Store_(const std::string& tag, DynamicData::Layout& layout)
	{
		auto r = m_map.insert({ tag, layout.Finalize() });
		assert(r.second);
	}

private:
	std::unordered_map<std::string, std::shared_ptr<DynamicData::LayoutElement>> m_map;
};