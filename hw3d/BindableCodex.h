#pragma once

#include <memory>
#include <unordered_map>
#include <type_traits>
#include "Bindable.h"

namespace Bind
{
	// ������Դ�ֵ䣨����Ⱦ�����ϵ���Դ�����������еĻ�ֱ�ӻ�ȡ���ɣ������ظ�������
	class BindCodex
	{
	public:
		template<class T, typename...Params>
		static std::shared_ptr<T> Resolve(Graphics& gfx, Params&&...params) noexcept
		{
			static_assert(std::is_base_of<Bindable, T>::value, "���Ͳ��̳���Bindable");
			return Get().Resolve_<T>(gfx, std::forward<Params>(params)...);
		}

	private:

		template<class T, typename...Params>
		std::shared_ptr<T> Resolve_(Graphics& gfx, Params&&...params) noexcept
		{
			const auto key = T::GenerateUID(std::forward<Params>(params)...);
			const auto iter = m_binds.find(key);
			if (iter == m_binds.end())
			{
				auto bind = std::make_shared<T>(gfx, std::forward<Params>(params)...);
				m_binds[key] = bind;
				return bind;
			}
			else
			{
				return static_pointer_cast<T>(iter->second);
			}
		}

		static BindCodex& Get()
		{
			static BindCodex codex;
			return codex;
		}

	private:
		std::unordered_map<std::string, std::shared_ptr<Bindable>> m_binds;
	};

}