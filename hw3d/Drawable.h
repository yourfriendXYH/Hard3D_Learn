#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include <memory>

// 绘制？？
class Bindable;
namespace Bind
{
	class IndexBuffer;
}
class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noexcept;
	void AddBind(std::shared_ptr<Bindable> bind) noexcept;

protected:
	// 获取指定的Bindable
	template<class T>
	T* QueryBindable() noexcept
	{
		for (auto& bind : m_binds)
		{
			if (auto pBind = dynamic_cast<T*>(bind.get()))
			{
				return pBind;
			}
		}
		return nullptr;
	}

private:
	const class Bind::IndexBuffer* m_pIndexBuffer = nullptr;
	std::vector<std::shared_ptr<Bindable>> m_binds;
};
