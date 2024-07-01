#include "Drawable.h"
#include "Bindable.h"
#include "IndexBuffer.h"
//#include "Draw"

using namespace Bind;

void Drawable::Draw(Graphics& gfx) const noexcept
{
	for (auto& bind : m_binds)
	{
		bind->Bind(gfx);
	}

	gfx.DrawIndexed(m_pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::shared_ptr<Bindable> bind) noexcept
{
	//assert("Must use AddIndexBuffer to " && typeid(*bind) != typeid(IndexBuffer));
	if (typeid(*bind) == typeid(IndexBuffer)) // 如果是索引缓存类型
	{
		assert(m_pIndexBuffer == nullptr);
		m_pIndexBuffer = &static_cast<IndexBuffer&>(*bind);
	}
	m_binds.emplace_back(std::move(bind));
}
