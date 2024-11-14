#pragma once
#include <assert.h>
#include <unordered_map>
#include <string>
#include <memory>

#define RESOLVE_BASE(elementType) \
virtual size_t Resolve ## elementType() const noexcept \
{ \
	assert(false); \
	return 0; \
}

// 布局元素宏
#define LEAF_ELEMENT_IMPL(elementType, sysType, hlslSize) \
class elementType : public LayoutElement \
{ \
public: \
	using SystemType = sysType; \
	size_t Resolve ## elementType() const noexcept override final \
	{ \
		return GetOffsetBegin(); \
	} \
	size_t GetOffsetEnd() const noexcept override final \
	{ \
		return GetOffsetBegin() + ComputeSize(); \
	} \
protected: \
	size_t Finalize(size_t offset) override final \
	{ \
		m_offset = offset; \
		return offset + ComputeSize(); \
	} \
	size_t ComputeSize() const noexcept override final \
	{ \
		return hlslSize; \
	} \
};

#define LEAF_ELEMENT(elementType, sysType) LEAF_ELEMENT_IMPL(elementType, sysType, sizeof(sysType))

// 类型转换运算符 和 赋值运算符 的重写(__VA_ARGS__多参数)
#define REF_CONVERSION(elementType,...) \
operator __VA_ARGS__ elementType::SystemType& () noexcept \
{ \
	return *reinterpret_cast<elementType::SystemType*>(m_pBytes + m_pLayout->Resolve ## elementType()); \
} 

#define REF_ASSIGN(elementType) \
elementType::SystemType& operator=(const elementType::SystemType& rhs) noexcept \
{ \
	return static_cast<elementType::SystemType&>(*this) = rhs; \
}

#define REF_NONCONST(elementType) REF_CONVERSION(elementType) REF_ASSIGN(elementType)
#define REF_CONST(elementType) REF_CONVERSION(elementType, const)

#define PTR_CONVERSION(elementType,...) \
operator __VA_ARGS__ elementType::SystemType*() noexcept \
{ \
	return &static_cast<__VA_ARGS__ elementType::SystemType&>(m_ref); \
}

namespace DynamicData
{
	class Struct;
	class Array;
	class Layout;
	// 单个元素布局
	class LayoutElement
	{
		friend class Struct;
		friend class Array;
		friend class Layout;
	public:
		virtual ~LayoutElement()
		{
		}

		// [] 仅适用于结构体；通过名称访问成员
		virtual LayoutElement& operator[](const std::string& key)
		{
			assert(false);
			return *this;
		}
		virtual const LayoutElement& operator[](const std::string& key) const
		{
			assert(false);
			return *this;
		}

		// LayoutEle() 仅适用于数组；获取数组类型布局对象
		virtual LayoutElement& LayoutEle()
		{
			assert(false);
			return *this;
		}
		virtual const LayoutElement& LayoutEle() const
		{
			assert(false);
			return *this;
		}

		// 基于偏移量的函数 仅在完成后才起作用！
		size_t GetOffsetBegin() const noexcept
		{
			return m_offset;
		}
		virtual size_t GetOffsetEnd() const noexcept = 0;

		// 获取从偏移量得出的字节大小
		size_t GetSizeInBytes() const noexcept
		{
			return GetOffsetEnd() - GetOffsetBegin();
		}

		// 仅适用于 Structs；添加 LayoutElement
		template<typename T>
		LayoutElement& Add(const std::string& key) noexcept;

		// 仅适用于数组；设置元素的类型和大小
		template<typename T>
		LayoutElement& Set(size_t size_in) noexcept;

		// 返回偏移量的值，该值上升到下一个 16 字节边界（如果尚未到达边界）
		static size_t GetNextBoundaryOffset(size_t offset)
		{
			return offset + (16u - offset % 16u) % 16u;
		}

		RESOLVE_BASE(Matrix);
		RESOLVE_BASE(Float4);
		RESOLVE_BASE(Float3);
		RESOLVE_BASE(Float2);
		RESOLVE_BASE(Float);
		RESOLVE_BASE(Bool);

	protected:
		// 设置元素和子元素的所有偏移量，返回此元素之后的偏移量
		virtual size_t Finalize(size_t offset) = 0;
		// 计算此元素的大小（以字节为单位），考虑数组和结构上的填充
		virtual size_t ComputeSize() const noexcept = 0;

	protected:
		size_t m_offset = 0u;
	};

	LEAF_ELEMENT(Matrix, DirectX::XMFLOAT4X4);
	LEAF_ELEMENT(Float4, DirectX::XMFLOAT4);
	LEAF_ELEMENT(Float3, DirectX::XMFLOAT3);
	LEAF_ELEMENT(Float2, DirectX::XMFLOAT2);
	LEAF_ELEMENT(Float, float);
	LEAF_ELEMENT_IMPL(Bool, bool, 4u);

	// 结构体布局
	class Struct : public LayoutElement
	{
	public:
		//using LayoutElement::LayoutElement; // 使用基类的构造函数
		LayoutElement& operator[](const std::string& key) override final
		{
			return *m_map.at(key);
		}

		const LayoutElement& operator[](const std::string& key) const override final
		{
			return *m_map.at(key);
		}

		size_t GetOffsetEnd() const noexcept override final
		{
			// 上升到下一个边界（因为结构的大小是 16 的倍数）
			return LayoutElement::GetNextBoundaryOffset(m_elements.back()->GetOffsetEnd());
		}

		// 添加元素布局
		void Add(const std::string& name, std::unique_ptr<LayoutElement> pElement) noexcept
		{
			m_elements.emplace_back(std::move(pElement));
			if (!m_map.emplace(name, m_elements.back().get()).second)
			{
				assert(false);
			}
		}

	protected:
		size_t Finalize(size_t offset) override final
		{
			m_offset = offset;
			size_t offsetNext = offset;
			for (auto& pEle : m_elements)
			{
				offsetNext = (*pEle).Finalize(offsetNext); // 给结构体成员赋offset
			}
			return GetOffsetEnd();
		}

		size_t ComputeSize() const noexcept override final
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

	private:
		static size_t CalculatePaddingBeforeElement(size_t offset, size_t size) noexcept
		{
			// 如果跨越 16 字节边界，则前进到下一个边界
			if (offset / 16u != (offset + size - 1u) / 16u)
			{
				return GetNextBoundaryOffset(offset) - offset;
			}
			return 0u;
			return offset; // 返回offset似乎是错的
		}

	private:
		std::unordered_map<std::string, LayoutElement*> m_map; // 成员名对应的布局
		std::vector<std::unique_ptr<LayoutElement>> m_elements; // 成员布局
	};

	class Array : public LayoutElement
	{
	public:
		size_t GetOffsetEnd() const noexcept override final
		{
			assert(nullptr != m_upElement);
			// 数组未在 hlsl 中打包
			return GetOffsetBegin() + LayoutElement::GetNextBoundaryOffset(m_upElement->GetSizeInBytes()) * m_size;
		}

		void Set(std::unique_ptr<LayoutElement> pElement, size_t size_in) noexcept
		{
			assert(nullptr == m_upElement);
			m_upElement = std::move(pElement);
			m_size = size_in;
		}

		// 拿到数组的元素布局
		LayoutElement& LayoutEle() override final
		{
			return *m_upElement;
		}
		const LayoutElement& LayoutEle() const override final
		{
			return *m_upElement;
		}
	protected:
		// 数组末端的offset
		size_t Finalize(size_t offset) override final
		{
			assert(m_size != 0u && nullptr != m_upElement);
			m_offset = offset;
			m_upElement->Finalize(offset);
			return GetOffsetEnd();
		}

		size_t ComputeSize() const noexcept override final
		{
			// 数组未在 hlsl 中打包
			return LayoutElement::GetNextBoundaryOffset(m_upElement->ComputeSize()) * m_size;
		}

	private:
		size_t m_size = 0u; // 数组大小
		std::unique_ptr<LayoutElement> m_upElement; // 单个元素的布局
	};

	template<typename T>
	LayoutElement& LayoutElement::Add(const std::string& key) noexcept
	{
		auto pStruct = dynamic_cast<Struct*>(this);
		assert(nullptr != pStruct);
		pStruct->Add(key, std::make_unique<T>());
		return *this;
	}

	template<typename T>
	LayoutElement& LayoutElement::Set(size_t size_in) noexcept
	{
		auto pArray = dynamic_cast<Array*>(this);
		assert(nullptr != pArray);
		pArray->Set(std::make_unique<T>(), size_in);
		return *this;
	}

	// 缓存布局的封装类
	class Layout
	{
	public:
		Layout()
			:
			m_pLayout(std::make_shared<Struct>())
		{
		}

		Layout(std::shared_ptr<LayoutElement> pLayout)
			:
			m_pLayout(std::move(pLayout))
		{
		}

		LayoutElement& operator[](const std::string& key)
		{
			return (*m_pLayout)[key];
		}

		size_t GetSizeInByte() const noexcept
		{
			return m_pLayout->GetSizeInBytes();
		}

		template<typename T>
		LayoutElement& Add(const std::string& key) noexcept
		{
			return m_pLayout->Add<T>(key);
		}

		std::shared_ptr<LayoutElement> Finalize()
		{
			m_pLayout->Finalize(0); // 初始布局的偏移值
			m_finalized = true;
			return m_pLayout;
		}

	private:
		bool m_finalized = false;
		std::shared_ptr<LayoutElement> m_pLayout;
	};

	class ConstElementRef
	{
	public:
		class Ptr
		{
		public:
			Ptr(ConstElementRef& ref)
				:
				m_ref(ref)
			{
			}

			PTR_CONVERSION(Matrix, const);
			PTR_CONVERSION(Float4, const);
			PTR_CONVERSION(Float3, const);
			PTR_CONVERSION(Float2, const);
			PTR_CONVERSION(Float, const);
			PTR_CONVERSION(Bool, const);

		private:
			ConstElementRef& m_ref;
		};
	public:
		ConstElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset)
			:
			m_offset(offset),
			m_pLayout(pLayout),
			m_pBytes(pBytes)
		{
		}

		ConstElementRef operator[](const std::string& key) noexcept
		{
			return { &(*m_pLayout)[key], m_pBytes, m_offset };
		}

		ConstElementRef operator[](size_t index) noexcept
		{
			// 数组未在 hlsl 中打包
			const auto& t = m_pLayout->LayoutEle();
			const auto elementSize = LayoutElement::GetNextBoundaryOffset(t.GetSizeInBytes());
			return { &t, m_pBytes, m_offset + elementSize * index };
		}

		Ptr operator&() noexcept
		{
			return { *this };
		}

		REF_CONST(Matrix);
		REF_CONST(Float4);
		REF_CONST(Float3);
		REF_CONST(Float2);
		REF_CONST(Float);
		REF_CONST(Bool);

	private:
		size_t m_offset;
		const class LayoutElement* m_pLayout;
		char* m_pBytes;
	};

	class ElementRef
	{
	public:
		class Ptr
		{
		public:
			Ptr(ElementRef& ref)
				:
				m_ref(ref)
			{
			}

			PTR_CONVERSION(Matrix);
			PTR_CONVERSION(Float4);
			PTR_CONVERSION(Float3);
			PTR_CONVERSION(Float2);
			PTR_CONVERSION(Float);
			PTR_CONVERSION(Bool);

		private:
			ElementRef& m_ref;
		};

	public:
		ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset)
			:
			m_pLayout(pLayout),
			m_pBytes(pBytes),
			m_offset(offset)
		{
		}

		operator ConstElementRef() const noexcept
		{
			return { m_pLayout, m_pBytes, m_offset };
		}

		ElementRef operator[](const std::string& key) noexcept
		{
			return { &(*m_pLayout)[key], m_pBytes, m_offset };
		}

		ElementRef operator[](size_t index) noexcept
		{
			// 数组未在 hlsl 中打包
			const auto& t = m_pLayout->LayoutEle();
			const auto elementSize = LayoutElement::GetNextBoundaryOffset(t.GetSizeInBytes());
			return { &t, m_pBytes, m_offset + elementSize * index };
		}

		Ptr operator&() noexcept
		{
			return { *this };
		}

		REF_NONCONST(Matrix);
		REF_NONCONST(Float4);
		REF_NONCONST(Float3);
		REF_NONCONST(Float2);
		REF_NONCONST(Float);
		REF_NONCONST(Bool);

	private:
		const class LayoutElement* m_pLayout;
		char* m_pBytes;
		size_t m_offset;
	};

	class Buffer
	{
	public:
		Buffer(Layout& layout)
			:
			m_pLayout(static_pointer_cast<Struct>(layout.Finalize())),
			m_bytes(m_pLayout->GetOffsetEnd()) // 根据布局初始化缓存大小
		{
		}
		ElementRef operator[](const std::string& key)
		{
			return { &(*m_pLayout)[key], m_bytes.data(), 0u };
		}

		ConstElementRef operator[](const std::string& key) const noexcept
		{
			return const_cast<Buffer&>(*this)[key];
		}

		const char* GetData() const noexcept
		{
			return m_bytes.data();
		}

		size_t GetSizeInByte() const noexcept
		{
			return m_bytes.size();
		}

		const LayoutElement& GetLayout() const noexcept
		{
			return *m_pLayout;
		}

		std::shared_ptr<LayoutElement> CloneLayout() const
		{
			return m_pLayout;
		}

	private:
		std::shared_ptr<Struct> m_pLayout; // 常数缓存的布局
		std::vector<char> m_bytes; // 常数缓存的数据
	};
}
