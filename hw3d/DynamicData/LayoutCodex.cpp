#include "LayoutCodex.h"

namespace DynamicData
{

	DynamicData::Layout LayoutCodex::Resolve(Layout& layout) noexcept
	{
		layout.Finalize();
		auto sig = layout.GetSignature();
		auto& map = Get_().m_map;
		const auto iter = map.find(sig);
		if (iter != map.end())
		{
			return { iter->second };
		}
		auto result = map.insert({ std::move(sig), layout.ShareRoot() });
		return { result.first->second };
	}

	DynamicData::LayoutCodex& LayoutCodex::Get_() noexcept
	{
		static LayoutCodex codex;
		return codex;
	}
}