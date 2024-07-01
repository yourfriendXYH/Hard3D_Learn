#pragma once

//#include "Drawable.h"
//#include "Bindable.h"
//#include "IndexBuffer.h"
//
//// 相同的对象，用同一套资源显示
//
//template<class T>
//class DrawableBase : public Drawable
//{
//public:
//	static bool IsStaticInitialized() noexcept
//	{
//		return !m_staticBinds.empty();
//	}
//
//	static void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept
//	{
//		assert(typeid(*bind) != typeid(IndexBuffer));
//		m_staticBinds.emplace_back(std::move(bind));
//	}
//
//	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept
//	{
//		assert(m_pIndexBuffer == nullptr);
//		m_pIndexBuffer = indexBuffer.get();
//		m_staticBinds.emplace_back(std::move(indexBuffer));
//	}
//
//	void SetStaticIndexBuffer()
//	{
//		for (const auto& staticBind : m_staticBinds)
//		{
//			if (const auto pIndexBuffer = dynamic_cast<IndexBuffer*>(staticBind.get()))
//			{
//				m_pIndexBuffer = pIndexBuffer;
//				return;
//			}
//		}
//	}
//
//private:
//	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept
//	{
//		return m_staticBinds;
//	}
//
//private:
//
//	static std::vector<std::unique_ptr<Bindable>> m_staticBinds;
//};
//
//template<class T>
//std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::m_staticBinds;