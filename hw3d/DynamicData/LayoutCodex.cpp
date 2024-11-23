#include "LayoutCodex.h"

namespace DynamicData
{

	DynamicData::CookedLayout LayoutCodex::Resolve(RawLayout&& layout) noexcept
	{
		auto sig = layout.GetSignature();
		auto& map = Get_().m_mapOld;
		const auto iter = map.find(sig);
		if (iter != map.end())
		{
			return { iter->second };
		}
		auto result = map.insert({ std::move(sig), layout.DeliverLayout() });
		return { result.first->second };
	}

	DynamicData::CookedLayoutEx LayoutCodex::Resolve(RawLayoutEx&& layout) noexcept
	{
		auto sig = layout.GetSignature();
		auto& map = Get_().m_map;
		const auto iter = map.find(sig);
		if (iter != map.end())
		{
			return { iter->second };
		}
		auto result = map.insert({ std::move(sig), layout.DeliverRoot() });
		return { result.first->second };
	}

	DynamicData::LayoutCodex& LayoutCodex::Get_() noexcept
	{
		static LayoutCodex codex;
		return codex;
	}
}