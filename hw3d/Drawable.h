#pragma once
#include <DirectXMath.h>
#include <memory>
#include "Graphics.h"
#include "Bindable/Technique.h"


// ���ƣ���
class Bindable;
namespace Bind
{
	class IndexBuffer;
	class VertexBuffer;
	class Topology;
}
class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable();

	void AddTechnique(Technique technique_in) noexcept;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;

	void Submit(class FrameCommander& frame) const noexcept;

	void Bind(Graphics& gfx) const noexcept;

	UINT GetIndexCount() const noexcept;

	void Accept(class TechniqueProbe& probe);


	// ��ȡָ����Bindable
	//template<class T>
	//T* QueryBindable() noexcept
	//{
	//	for (auto& bind : m_binds)
	//	{
	//		if (auto pBind = dynamic_cast<T*>(bind.get()))
	//		{
	//			return pBind;
	//		}
	//	}
	//	return nullptr;
	//}

protected:
	std::shared_ptr<Bind::IndexBuffer> m_pIndexBuffer = nullptr;
	std::shared_ptr<Bind::VertexBuffer> m_pVertexBuffer = nullptr;
	std::shared_ptr<Bind::Topology> m_pTopology = nullptr;
	std::vector<Technique> m_techniques;
};
