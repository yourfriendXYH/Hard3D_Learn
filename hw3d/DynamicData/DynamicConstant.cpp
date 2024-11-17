#include "DynamicConstant.h"
#include <string>
#include <cctype>
#include <algorithm>

#define DCB_RESOLVE_BASE(elementType) \
size_t LayoutElement::Resolve ## elementType() const noexcept \
{ \
	assert(false); \
	return 0; \
}

#define DCB_LEAF_ELEMENT_IMPL(elementType, sysType, hlslSize) \
size_t elementType::Resolve ## elementType() const noexcept \
{ \
	return GetOffsetBegin(); \
} \
size_t elementType::GetOffsetEnd() const noexcept \
{ \
	return GetOffsetBegin() + ComputeSize(); \
} \
std::string elementType::GetSignature() const noexcept \
{ \
	return #elementType; \
} \
size_t elementType::Finalize(size_t offset) \
{ \
	m_offset = offset; \
	return offset + ComputeSize(); \
} \
size_t elementType::ComputeSize() const noexcept \
{ \
	return hlslSize; \
}

#define DCB_LEAF_ELEMENT(elementType, sysType) DCB_LEAF_ELEMENT_IMPL(elementType, sysType, sizeof(sysType))

#define DCB_REF_CONVERSION(refType,elementType,...) \
refType::operator __VA_ARGS__ elementType::SystemType& () noexcept \
{ \
	return *reinterpret_cast<elementType::SystemType*>(m_pBytes + m_pLayout->Resolve ## elementType()); \
}

#define DCB_REF_ASSIGN(refType,elementType) \
elementType::SystemType& refType::operator=(const elementType::SystemType& rhs) noexcept \
{ \
	return static_cast<elementType::SystemType&>(*this) = rhs; \
}

#define DCB_REF_NONCONST(refType,elementType) DCB_REF_CONVERSION(refType,elementType) DCB_REF_ASSIGN(refType,elementType)
#define DCB_REF_CONST(refType,elementType) DCB_REF_CONVERSION(refType,elementType, const)

#define DCB_PTR_CONVERSION(refType,elementType,...) \
refType::operator __VA_ARGS__ elementType::SystemType*() noexcept \
{ \
	return &static_cast<__VA_ARGS__ elementType::SystemType&>(m_ref); \
}

namespace DynamicData
{
	LayoutElement::~LayoutElement()
	{
	}

	LayoutElement& LayoutElement::operator[](const std::string& key)
	{
		assert(false);
		return *this;
	}

	const LayoutElement& LayoutElement::operator[](const std::string& key) const
	{
		assert(false);
		return *this;
	}

	LayoutElement& LayoutElement::LayoutEle()
	{
		assert(false);
		return *this;
	}

	const LayoutElement& LayoutElement::LayoutEle() const
	{
		assert(false);
		return *this;
	}

	size_t LayoutElement::GetOffsetBegin() const noexcept
	{
		return m_offset;
	}

	size_t LayoutElement::GetSizeInBytes() const noexcept
	{
		return GetOffsetEnd() - GetOffsetBegin();
	}

	size_t LayoutElement::GetNextBoundaryOffset(size_t offset)
	{
		return offset + (16u - offset % 16u) % 16u;
	}

	DCB_RESOLVE_BASE(Matrix);
	DCB_RESOLVE_BASE(Float4);
	DCB_RESOLVE_BASE(Float3);
	DCB_RESOLVE_BASE(Float2);
	DCB_RESOLVE_BASE(Float);
	DCB_RESOLVE_BASE(Bool);

	// 无效的布局元素
	class Empty : public LayoutElement
	{
	public:
		size_t GetOffsetEnd() const noexcept override final
		{
			return 0u;
		}

		bool Exists() const noexcept override final
		{
			return false;
		}

		std::string GetSignature() const noexcept override
		{
			assert(false);
			return "";
		}

	protected:
		size_t Finalize(size_t offset) override final
		{
			return 0u;
		}
		size_t ComputeSize() const noexcept override final
		{
			return 0u;
		}

	private:
	} emptyLayoutElement;

	DCB_LEAF_ELEMENT(Matrix, DirectX::XMFLOAT4X4);
	DCB_LEAF_ELEMENT(Float4, DirectX::XMFLOAT4);
	DCB_LEAF_ELEMENT(Float3, DirectX::XMFLOAT3);
	DCB_LEAF_ELEMENT(Float2, DirectX::XMFLOAT2);
	DCB_LEAF_ELEMENT(Float, float);
	DCB_LEAF_ELEMENT_IMPL(Bool, bool, 4u);

	LayoutElement& Struct::operator[](const std::string& key)
	{
		auto iter = m_map.find(key);
		if (iter == m_map.end())
		{
			return emptyLayoutElement;
		}
		return *iter->second;
	}

	const LayoutElement& Struct::operator[](const std::string& key) const
	{
		return *m_map.at(key);
	}

	size_t Struct::GetOffsetEnd() const noexcept
	{
		// 上升到下一个边界（因为结构的大小是 16 的倍数）
		return LayoutElement::GetNextBoundaryOffset(m_elements.back()->GetOffsetEnd());
	}

	std::string Struct::GetSignature() const noexcept
	{
		using namespace std::string_literals;
		auto sig = "Struct{"s;
		for (const auto& pElement : m_elements)
		{
			auto iter = std::find_if(
				m_map.begin(), m_map.end(),
				[&pElement](const std::pair<std::string, LayoutElement*>& v)
				{
					return &*pElement == v.second;
				}
			);
			sig += iter->first + ":"s + pElement->GetSignature() + ";"s;
		}
		sig += "}"s;
		return sig;
	}

	bool ValidateSymbolName(const std::string& name) noexcept
	{
		return !name.empty() && !std::isdigit(name.front()) &&
			std::all_of(name.begin(), name.end(), [](char c)
				{
					return std::isalnum(c) || c == '_';
				}
			);
	}

	void Struct::Add(const std::string& name, std::unique_ptr<LayoutElement> pElement) noexcept
	{
		// 
		assert(ValidateSymbolName(name) && "invalid name in Struct");

		m_elements.emplace_back(std::move(pElement));
		if (!m_map.emplace(name, m_elements.back().get()).second)
		{
			assert(false);
		}
	}

	size_t Struct::Finalize(size_t offset)
	{
		m_offset = offset;
		size_t offsetNext = offset;
		for (auto& pEle : m_elements)
		{
			offsetNext = (*pEle).Finalize(offsetNext); // 给结构体成员赋offset
		}
		return GetOffsetEnd();
	}

	size_t Struct::ComputeSize() const noexcept
	{
		// 通过对 大小 + 填充 求和来计算所有元素的偏移量
		size_t offsetNext = 0u;
		for (auto& ele : m_elements)
		{
			const auto elementSize = ele->ComputeSize(); // 结构体中单个元素的大小
			offsetNext += CalculatePaddingBeforeElement(offsetNext, elementSize) + elementSize;

		}
		// 结构大小必须是 16 字节的倍数
		return GetNextBoundaryOffset(offsetNext);
	}

	size_t Struct::CalculatePaddingBeforeElement(size_t offset, size_t size) noexcept
	{
		// 如果跨越 16 字节边界，则前进到下一个边界
		if (offset / 16u != (offset + size - 1u) / 16u)
		{
			return GetNextBoundaryOffset(offset) - offset;
		}
		return 0u;
		return offset; // 返回offset似乎是错的
	}

	size_t Array::GetOffsetEnd() const noexcept
	{
		assert(nullptr != m_upElement);
		// 数组未在 hlsl 中打包
		return GetOffsetBegin() + LayoutElement::GetNextBoundaryOffset(m_upElement->GetSizeInBytes()) * m_size;
	}

	void Array::Set(std::unique_ptr<LayoutElement> pElement, size_t size_in) noexcept
	{
		assert(nullptr == m_upElement);
		m_upElement = std::move(pElement);
		m_size = size_in;
	}

	LayoutElement& Array::LayoutEle()
	{
		return *m_upElement;
	}

	const LayoutElement& Array::LayoutEle() const
	{
		return const_cast<Array*>(this)->LayoutEle();
	}

	// 获取签名
	std::string Array::GetSignature() const noexcept
	{
		using namespace std::string_literals;
		return "Array"s + std::to_string(m_size) + "{"s + LayoutEle().GetSignature() + "}"s;
	}

	bool Array::IndexInBounds(size_t index) const noexcept
	{
		return index < m_size;
	}

	size_t Array::Finalize(size_t offset)
	{
		assert(m_size != 0u && nullptr != m_upElement);
		m_offset = offset;
		m_upElement->Finalize(offset);
		return GetOffsetEnd();
	}

	size_t Array::ComputeSize() const noexcept
	{
		// 数组未在 hlsl 中打包
		return LayoutElement::GetNextBoundaryOffset(m_upElement->ComputeSize()) * m_size;
	}

	Layout::Layout()
		:
		m_pLayout(std::make_shared<Struct>())
	{
	}

	Layout::Layout(std::shared_ptr<LayoutElement> pLayout)
		:
		m_pLayout(std::move(pLayout))
	{
	}

	LayoutElement& Layout::operator[](const std::string& key)
	{
		return (*m_pLayout)[key];
	}

	size_t Layout::GetSizeInByte() const noexcept
	{
		return m_pLayout->GetSizeInBytes();
	}

	std::shared_ptr<LayoutElement> Layout::Finalize()
	{
		m_pLayout->Finalize(0); // 初始布局的偏移值
		m_finalized = true;
		return m_pLayout;
	}

	std::string Layout::GetSignature() const noexcept
	{
		return m_pLayout->GetSignature();
	}

	ConstElementRef::Ptr::Ptr(ConstElementRef& ref)
		:
		m_ref(ref)
	{
	}

	DCB_PTR_CONVERSION(ConstElementRef::Ptr, Matrix, const);
	DCB_PTR_CONVERSION(ConstElementRef::Ptr, Float4, const);
	DCB_PTR_CONVERSION(ConstElementRef::Ptr, Float3, const);
	DCB_PTR_CONVERSION(ConstElementRef::Ptr, Float2, const);
	DCB_PTR_CONVERSION(ConstElementRef::Ptr, Float, const);
	DCB_PTR_CONVERSION(ConstElementRef::Ptr, Bool, const);

	ConstElementRef::ConstElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset)
		:
		m_offset(offset),
		m_pLayout(pLayout),
		m_pBytes(pBytes)
	{
	}

	bool ConstElementRef::Exists() const noexcept
	{
		return m_pLayout->Exists();
	}

	ConstElementRef ConstElementRef::operator[](const std::string& key) noexcept
	{
		return { &(*m_pLayout)[key], m_pBytes, m_offset };
	}

	ConstElementRef ConstElementRef::operator[](size_t index) noexcept
	{
		// 数组未在 hlsl 中打包
		const auto& t = m_pLayout->LayoutEle();
		// 上一次调用没有失败，断言意味着布局是数组
		assert(static_cast<const Array&>(*m_pLayout).IndexInBounds(index));
		const auto elementSize = LayoutElement::GetNextBoundaryOffset(t.GetSizeInBytes());
		return { &t, m_pBytes, m_offset + elementSize * index };
	}

	ConstElementRef::Ptr ConstElementRef::operator&() noexcept
	{
		return { *this };
	}

	DCB_REF_CONST(ConstElementRef, Matrix);
	DCB_REF_CONST(ConstElementRef, Float4);
	DCB_REF_CONST(ConstElementRef, Float3);
	DCB_REF_CONST(ConstElementRef, Float2);
	DCB_REF_CONST(ConstElementRef, Float);
	DCB_REF_CONST(ConstElementRef, Bool);

	ElementRef::Ptr::Ptr(ElementRef& ref)
		:
		m_ref(ref)
	{
	}

	DCB_PTR_CONVERSION(ElementRef::Ptr, Matrix);
	DCB_PTR_CONVERSION(ElementRef::Ptr, Float4);
	DCB_PTR_CONVERSION(ElementRef::Ptr, Float3);
	DCB_PTR_CONVERSION(ElementRef::Ptr, Float2);
	DCB_PTR_CONVERSION(ElementRef::Ptr, Float);
	DCB_PTR_CONVERSION(ElementRef::Ptr, Bool);

	ElementRef::ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset)
		:
		m_pLayout(pLayout),
		m_pBytes(pBytes),
		m_offset(offset)
	{
	}

	bool ElementRef::Exists() const noexcept
	{
		return m_pLayout->Exists();
	}

	ElementRef::operator ConstElementRef() const noexcept
	{
		return { m_pLayout, m_pBytes, m_offset };
	}

	ElementRef ElementRef::operator[](const std::string& key) noexcept
	{
		return { &(*m_pLayout)[key], m_pBytes, m_offset };
	}

	ElementRef ElementRef::operator[](size_t index) noexcept
	{
		// 数组未在 hlsl 中打包
		const auto& t = m_pLayout->LayoutEle();
		// 上一次调用没有失败，断言意味着布局是数组
		assert(static_cast<const Array&>(*m_pLayout).IndexInBounds(index));
		const auto elementSize = LayoutElement::GetNextBoundaryOffset(t.GetSizeInBytes());
		return { &t, m_pBytes, m_offset + elementSize * index };
	}

	ElementRef::Ptr ElementRef::operator&() noexcept
	{
		return { *this };
	}

	DCB_REF_NONCONST(ElementRef, Matrix);
	DCB_REF_NONCONST(ElementRef, Float4);
	DCB_REF_NONCONST(ElementRef, Float3);
	DCB_REF_NONCONST(ElementRef, Float2);
	DCB_REF_NONCONST(ElementRef, Float);
	DCB_REF_NONCONST(ElementRef, Bool);



	Buffer::Buffer(Layout& layout)
		:
		m_pLayout(static_pointer_cast<Struct>(layout.Finalize())),
		m_bytes(m_pLayout->GetOffsetEnd()) // 根据布局初始化缓存大小
	{
	}

	ElementRef Buffer::operator[](const std::string& key)
	{
		return { &(*m_pLayout)[key], m_bytes.data(), 0u };
	}

	ConstElementRef Buffer::operator[](const std::string& key) const noexcept
	{
		return const_cast<Buffer&>(*this)[key];
	}

	const char* Buffer::GetData() const noexcept
	{
		return m_bytes.data();
	}

	size_t Buffer::GetSizeInByte() const noexcept
	{
		return m_bytes.size();
	}

	const LayoutElement& Buffer::GetLayout() const noexcept
	{
		return *m_pLayout;
	}

	std::shared_ptr<LayoutElement> Buffer::CloneLayout() const
	{
		return m_pLayout;
	}

	std::string Buffer::GetSignature() const noexcept
	{
		return m_pLayout->GetSignature();
	}

}