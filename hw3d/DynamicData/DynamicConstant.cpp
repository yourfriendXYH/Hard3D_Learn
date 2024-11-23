#include "DynamicConstant.h"
#include "LayoutCodex.h"
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
size_t elementType::Finalize(size_t offset) noexcept \
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
	struct ExtraData
	{
		struct Struct : public LayoutElementEx::ExtraDataBase
		{
			std::vector<std::pair<std::string, LayoutElementEx>> layoutElements;
		};
		struct Array : public LayoutElementEx::ExtraDataBase
		{
			std::optional<LayoutElementEx> layoutElement;
			size_t size;
		};
	};

	std::string LayoutElementEx::GetSignature() const noexcept
	{
		switch (m_type)
		{
		#define X(element) case element: return Map<element>::m_code;
		LEAF_ELEMENT_TYPES
		#undef X
		case EStruct:
			return GetSignatureForStruct();
		case EArray:
			return GetSignatureForArray();
		default:
			assert("Bad type in signature generation" && false);
			return "???";
		}
	}

	bool LayoutElementEx::Exists() const noexcept
	{
		return m_type != EEmpty;
	}

	std::pair<size_t, const LayoutElementEx*> LayoutElementEx::CalculateIndexingOffset(size_t offset, size_t index) const noexcept
	{
		assert("Indexing into non-array" && m_type == EArray);
		const auto& data = static_cast<ExtraData::Array&>(*m_pExtraData);
		assert(index < data.size);
		return { offset + data.layoutElement->GetSizeInBytes() * index,&*data.layoutElement };
	}

	LayoutElementEx& LayoutElementEx::operator[](const std::string& key) noexcept
	{
		assert("Keying into non-struct" && m_type == EStruct);
		for (auto& mem : static_cast<ExtraData::Struct&>(*m_pExtraData).layoutElements)
		{
			if (mem.first == key)
			{
				return mem.second;
			}
		}
		return GetEmptyElement();
	}

	const LayoutElementEx& LayoutElementEx::operator[](const std::string& key) const noexcept
	{
		return const_cast<LayoutElementEx&>(*this)[key];
	}

	LayoutElementEx& LayoutElementEx::ToLayout() noexcept
	{
		assert("Accessing T of non-array" && m_type == EArray);
		return *static_cast<ExtraData::Array&>(*m_pExtraData).layoutElement;
	}

	const LayoutElementEx& LayoutElementEx::ToLayout() const noexcept
	{
		return const_cast<LayoutElementEx&>(*this).ToLayout();
	}

	size_t LayoutElementEx::GetOffsetBegin() const noexcept
	{
		return *m_offset;
	}

	size_t LayoutElementEx::GetOffsetEnd() const noexcept
	{
		switch (m_type)
		{
		#define X(element) case element: return *m_offset + Map<element>::m_hlslSize;
		LEAF_ELEMENT_TYPES
		#undef X
		case EStruct:
		{
			const auto& data = static_cast<ExtraData::Struct&>(*m_pExtraData);
			return AdvanceToBoundary(data.layoutElements.back().second.GetOffsetEnd());
		}
		case EArray:
		{
			const auto& data = static_cast<ExtraData::Array&>(*m_pExtraData);
			return *m_offset + AdvanceToBoundary(data.layoutElement->GetSizeInBytes()) * data.size;
		}
		default:
			assert("Tried to get offset of empty or invalid element" && false);
			return 0u;
		}
	}

	size_t LayoutElementEx::GetSizeInBytes() const noexcept
	{
		return GetOffsetEnd() - GetOffsetBegin();
	}

	LayoutElementEx& LayoutElementEx::Add(Type addedType, std::string name) noexcept
	{
		assert("Add to non-struct in layout" && m_type == EStruct);
		assert("invalid symbol name in Struct" && ValidateSymbolName(name));
		auto& structData = static_cast<ExtraData::Struct&>(*m_pExtraData);
		for (auto& mem : structData.layoutElements)
		{
			if (mem.first == name)
			{
				assert("Adding duplicate name to struct" && false);
			}
		}
		structData.layoutElements.emplace_back(std::move(name), LayoutElementEx{ addedType });
		return *this;
	}

	LayoutElementEx& LayoutElementEx::Set(Type addedType, size_t size) noexcept
	{
		assert("Set on non-array in layout" && m_type == EArray);
		assert(size != 0u);
		auto& arrayData = static_cast<ExtraData::Array&>(*m_pExtraData);
		arrayData.layoutElement = { addedType };
		arrayData.size = size;
		return *this;
	}

	LayoutElementEx::LayoutElementEx(Type typeIn) noexcept
		:
		m_type(typeIn)
	{
		assert(typeIn != EEmpty);
		if (typeIn == EStruct)
		{
			m_pExtraData = std::unique_ptr<ExtraData::Struct>{ new ExtraData::Struct() };
		}
		else if (typeIn == EArray)
		{
			m_pExtraData = std::unique_ptr<ExtraData::Array>{ new ExtraData::Array() };
		}
	}

	size_t LayoutElementEx::Finalize(size_t offsetIn) noexcept
	{
		switch (m_type)
		{
		#define X(element) case element: m_offset = AdvanceIfCrossesBoundary( offsetIn,Map<element>::m_hlslSize ); return *m_offset + Map<element>::m_hlslSize;
		LEAF_ELEMENT_TYPES
		#undef X
		case EStruct:
			return FinalizeForStruct(offsetIn);
		case EArray:
			return FinalizeForArray(offsetIn);
		default:
			assert("Bad type in size computation" && false);
			return 0u;
		}
	}

	std::string LayoutElementEx::GetSignatureForStruct() const noexcept
	{
		using namespace std::string_literals;
		auto sig = "St{"s;
		for (const auto& el : static_cast<ExtraData::Struct&>(*m_pExtraData).layoutElements)
		{
			sig += el.first + ":"s + el.second.GetSignature() + ";"s;
		}
		sig += "}"s;
		return sig;
	}

	std::string LayoutElementEx::GetSignatureForArray() const noexcept
	{
		using namespace std::string_literals;
		const auto& data = static_cast<ExtraData::Array&>(*m_pExtraData);
		return "Ar:"s + std::to_string(data.size) + "{"s + data.layoutElement->GetSignature() + "}"s;
	}

	size_t LayoutElementEx::FinalizeForStruct(size_t offsetIn)
	{
		auto& data = static_cast<ExtraData::Struct&>(*m_pExtraData);
		assert(data.layoutElements.size() != 0u);
		m_offset = AdvanceToBoundary(offsetIn);
		auto offsetNext = *m_offset;
		for (auto& el : data.layoutElements)
		{
			offsetNext = el.second.Finalize(offsetNext);
		}
		return offsetNext;
	}

	size_t LayoutElementEx::FinalizeForArray(size_t offsetIn)
	{
		auto& data = static_cast<ExtraData::Array&>(*m_pExtraData);
		assert(data.size != 0u);
		m_offset = AdvanceToBoundary(offsetIn);
		data.layoutElement->Finalize(*m_offset);
		return GetOffsetEnd();
	}

	size_t LayoutElementEx::AdvanceToBoundary(size_t offset) noexcept
	{
		return offset + (16u - offset % 16u) % 16u;
	}

	bool LayoutElementEx::CrossesBoundary(size_t offset, size_t size) noexcept
	{
		const auto end = offset + size;
		const auto pageStart = offset / 16u;
		const auto pageEnd = end / 16u;
		return (pageStart != pageEnd && end % 16 != 0u) || size > 16u;
	}

	size_t LayoutElementEx::AdvanceIfCrossesBoundary(size_t offset, size_t size) noexcept
	{
		return CrossesBoundary(offset, size) ? AdvanceToBoundary(offset) : offset;
	}

	bool LayoutElementEx::ValidateSymbolName(const std::string& name) noexcept
	{
		// symbols can contain alphanumeric and underscore, must not start with digit
		return !name.empty() && !std::isdigit(name.front()) &&
			std::all_of(name.begin(), name.end(), [](char c) {
			return std::isalnum(c) || c == '_';
				}
		);
	}

	size_t LayoutEx::GetSizeInBytes() const noexcept
	{
		return m_pRootLayout->GetSizeInBytes();
	}

	std::string LayoutEx::GetSignature() const noexcept
	{
		return m_pRootLayout->GetSignature();
	}

	LayoutEx::LayoutEx(std::shared_ptr<LayoutElementEx> pRoot) noexcept
		:
		m_pRootLayout{ std::move(pRoot) }
	{
	}

	RawLayoutEx::RawLayoutEx() noexcept
		:
		LayoutEx{ std::shared_ptr<LayoutElementEx>{ new LayoutElementEx(EStruct) } }
	{
	}
	
	LayoutElementEx& RawLayoutEx::operator[](const std::string& key) noexcept
	{
		return (*m_pRootLayout)[key];
	}

	void RawLayoutEx::ClearRoot() noexcept
	{
		*this = RawLayoutEx();
	}

	std::shared_ptr<LayoutElementEx> RawLayoutEx::DeliverRoot() noexcept
	{
		auto temp = std::move(m_pRootLayout);
		temp->Finalize(0);
		*this = RawLayoutEx();
		return std::move(temp);
	}	
	
	const LayoutElementEx& CookedLayoutEx::operator[](const std::string& key) const noexcept
	{
		return (*m_pRootLayout)[key];
	}

	std::shared_ptr<LayoutElementEx> CookedLayoutEx::ShareRoot() const noexcept
	{
		return m_pRootLayout;
	}

	CookedLayoutEx::CookedLayoutEx(std::shared_ptr<LayoutElementEx> pRoot) noexcept
		:
		LayoutEx(std::move(pRoot))
	{
	}

	std::shared_ptr<LayoutElementEx> CookedLayoutEx::RelinquishRoot() const noexcept
	{
		return std::move(m_pRootLayout);
	}

	ConstElementRefEx::Ptr::Ptr(const ConstElementRefEx* ref) noexcept
		:
		m_ref(ref)
	{
	}

	bool ConstElementRefEx::Exists() const noexcept
	{
		return m_pLayout->Exists();
	}

	ConstElementRefEx ConstElementRefEx::operator[](const std::string& key) const noexcept
	{
		return { &(*m_pLayout)[key],m_pBytes,m_offset };
	}

	ConstElementRefEx ConstElementRefEx::operator[](size_t index) const noexcept
	{
		const auto indexingData = m_pLayout->CalculateIndexingOffset(m_offset, index);
		return { indexingData.second, m_pBytes, indexingData.first };
	}

	ConstElementRefEx::Ptr ConstElementRefEx::operator&() const noexcept
	{
		return Ptr{ this };
	}

	ConstElementRefEx::ConstElementRefEx(const LayoutElementEx* pLayout, const char* pBytes, size_t offset) noexcept
		:
		m_offset(offset),
		m_pLayout(pLayout),
		m_pBytes(pBytes)
	{
	}

	ElementRefEx::Ptr::Ptr(ElementRefEx* ref) noexcept
		:
		m_ref(ref)
	{
	}

	ElementRefEx::operator ConstElementRefEx() const noexcept
	{
		return { m_pLayout, m_pBytes, m_offset };
	}

	bool ElementRefEx::Exists() const noexcept
	{
		return m_pLayout->Exists();
	}

	ElementRefEx ElementRefEx::operator[](const std::string& key) const noexcept
	{
		return { &(*m_pLayout)[key], m_pBytes, m_offset };
	}

	ElementRefEx ElementRefEx::operator[](size_t index) const noexcept
	{
		const auto indexingData = m_pLayout->CalculateIndexingOffset(m_offset, index);
		return { indexingData.second, m_pBytes, indexingData.first };
	}

	ElementRefEx::Ptr ElementRefEx::operator&() const noexcept
	{
		return Ptr{ const_cast<ElementRefEx*>(this) };
	}

	ElementRefEx::ElementRefEx(const LayoutElementEx* pLayout, char* pBytes, size_t offset) noexcept
		:
		m_offset(offset),
		m_pLayout(pLayout),
		m_pBytes(pBytes)
	{
	}

	BufferEx::BufferEx(RawLayoutEx&& lay) noexcept
		:
		BufferEx(LayoutCodex::Resolve(std::move(lay)))
	{
	}

	BufferEx::BufferEx(const CookedLayoutEx& lay) noexcept
		:
		m_pLayoutRoot(lay.ShareRoot()),
		m_bytes(m_pLayoutRoot->GetOffsetEnd())
	{
	}

	BufferEx::BufferEx(CookedLayoutEx&& lay) noexcept
		:
		m_pLayoutRoot(lay.RelinquishRoot()),
		m_bytes(m_pLayoutRoot->GetOffsetEnd())
	{
	}

	BufferEx::BufferEx(const BufferEx& buffer) noexcept
		:
		m_pLayoutRoot(buffer.m_pLayoutRoot),
		m_bytes(buffer.m_bytes)
	{
	}

	BufferEx::BufferEx(BufferEx&& buffer) noexcept
		:
		m_pLayoutRoot(std::move(buffer.m_pLayoutRoot)),
		m_bytes(std::move(buffer.m_bytes))
	{
	}

	ElementRefEx BufferEx::operator[](const std::string& key) noexcept
	{
		return { &(*m_pLayoutRoot)[key], m_bytes.data(),0u };
	}

	ConstElementRefEx BufferEx::operator[](const std::string& key) const noexcept
	{
		return const_cast<BufferEx&>(*this)[key];
	}

	const char* BufferEx::GetData() const noexcept
	{
		return m_bytes.data();
	}

	size_t BufferEx::GetSizeInBytes() const noexcept
	{
		return m_bytes.size();
	}

	const LayoutElementEx& BufferEx::GetRootLayoutElement() const noexcept
	{
		return *m_pLayoutRoot;
	}

	void BufferEx::CopyFrom(const BufferEx& other) noexcept
	{
		assert(&GetRootLayoutElement() == &other.GetRootLayoutElement());
		std::copy(other.m_bytes.begin(), other.m_bytes.end(), m_bytes.begin());
	}

	std::shared_ptr<LayoutElementEx> BufferEx::ShareLayoutRoot() const noexcept
	{
		return m_pLayoutRoot;
	}

/////////////////////////////////////旧的动态常数缓存实现/////////////////////////////////////////////
	LayoutElement::~LayoutElement()
	{
	}

	LayoutElement& LayoutElement::operator[](const std::string& key) noexcept
	{
		assert(false);
		return *this;
	}

	const LayoutElement& LayoutElement::operator[](const std::string& key) const noexcept
	{
		assert(false);
		return *this;
	}

	LayoutElement& LayoutElement::LayoutEle() noexcept
	{
		assert(false);
		return *this;
	}

	const LayoutElement& LayoutElement::LayoutEle() const noexcept
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
		size_t GetOffsetEnd() const noexcept final
		{
			return 0u;
		}

		bool Exists() const noexcept final
		{
			return false;
		}

		std::string GetSignature() const noexcept override
		{
			assert(false);
			return "";
		}

	protected:
		size_t Finalize(size_t offset) noexcept final
		{
			return 0u;
		}
		size_t ComputeSize() const noexcept final
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

	LayoutElement& Struct::operator[](const std::string& key) noexcept
	{
		auto iter = m_map.find(key);
		if (iter == m_map.end())
		{
			return emptyLayoutElement;
		}
		return *iter->second;
	}

	const LayoutElement& Struct::operator[](const std::string& key) const noexcept
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

	size_t Struct::Finalize(size_t offset) noexcept
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

	LayoutElement& Array::LayoutEle() noexcept
	{
		return *m_upElement;
	}

	const LayoutElement& Array::LayoutEle() const noexcept
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

	size_t Array::Finalize(size_t offset) noexcept
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

	Layout::Layout() noexcept
	{
		struct Enabler : public Struct {};
		m_pLayout = std::make_shared<Struct>();
	}

	Layout::Layout(std::shared_ptr<LayoutElement> pLayout) noexcept
		:
		m_pLayout(std::move(pLayout))
	{
	}

	LayoutElement& RawLayout::operator[](const std::string& key)
	{
		return (*m_pLayout)[key];
	}

	std::shared_ptr<LayoutElement> RawLayout::DeliverLayout() noexcept
	{
		auto temp = std::move(m_pLayout);
		temp->Finalize(0);
		ClearLayout();
		return std::move(temp);
	}

	void RawLayout::ClearLayout() noexcept
	{
		*this = RawLayout();
	}

	size_t Layout::GetSizeInByte() const noexcept
	{
		return m_pLayout->GetSizeInBytes();
	}

	std::string Layout::GetSignature() const noexcept
	{
		return m_pLayout->GetSignature();
	}

	const DynamicData::LayoutElement& CookedLayout::operator[](const std::string& key)
	{
		return (*m_pLayout)[key];
	}

	CookedLayout::CookedLayout(std::shared_ptr<LayoutElement> pLayout) noexcept
		:
		Layout(std::move(pLayout))
	{
	}

	std::shared_ptr<LayoutElement> CookedLayout::ReliquishLayout() const noexcept
	{
		return std::move(m_pLayout);
	}

	std::shared_ptr<LayoutElement> CookedLayout::ShareRoot() const noexcept
	{
		return m_pLayout;
	}

	ConstElementRef::Ptr::Ptr(ConstElementRef& ref) noexcept
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

	ConstElementRef::ConstElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset) noexcept
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

	ElementRef::Ptr::Ptr(ElementRef& ref) noexcept
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

	ElementRef::ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset) noexcept
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

	Buffer::Buffer(RawLayout&& layout) noexcept
		:
		Buffer(LayoutCodex::Resolve(std::move(layout)))
	{

	}

	Buffer::Buffer(CookedLayout&& layout) noexcept
		:
		m_pLayout(layout.ReliquishLayout()),
		m_bytes(m_pLayout->GetOffsetEnd())
	{
	}

	Buffer::Buffer(const CookedLayout& layout) noexcept
		:
		m_pLayout(layout.ShareRoot()),
		m_bytes(m_pLayout->GetOffsetEnd()) // 根据布局初始化缓存大小
	{
	}

	Buffer::Buffer(const Buffer& buffer) noexcept
		:
		m_pLayout(buffer.m_pLayout),
		m_bytes(buffer.m_bytes)
	{
	}

	Buffer::Buffer(Buffer&& buffer) noexcept
		:
		m_pLayout(std::move(buffer.m_pLayout)),
		m_bytes(std::move(buffer.m_bytes))
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

	void Buffer::CopyFrom(const Buffer& buffer) noexcept
	{
		assert(&GetLayout() == &buffer.GetLayout());
		std::copy(buffer.m_bytes.begin(), buffer.m_bytes.end(), m_bytes.begin());
	}

	std::shared_ptr<LayoutElement> Buffer::ShareLayout() const noexcept
	{
		return m_pLayout;
	}
}