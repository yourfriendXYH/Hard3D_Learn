#pragma once
#include <assert.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <DirectXMath.h>

#define DCB_RESOLVE_BASE(elementType) \
virtual size_t Resolve ## elementType() const noexcept;

// 布局元素宏
#define DCB_LEAF_ELEMENT_IMPL(elementType, sysType, hlslSize) \
class elementType : public LayoutElement \
{ \
public: \
	using SystemType = sysType; \
	size_t Resolve ## elementType() const noexcept override final; \
	size_t GetOffsetEnd() const noexcept override final; \
	std::string GetSignature() const noexcept override; \
protected: \
	size_t Finalize(size_t offset) override final; \
	size_t ComputeSize() const noexcept override final; \
};

#define DCB_LEAF_ELEMENT(elementType, sysType) DCB_LEAF_ELEMENT_IMPL(elementType, sysType, sizeof(sysType))

// 类型转换运算符 和 赋值运算符 的重写(__VA_ARGS__多参数)
#define DCB_REF_CONVERSION(elementType,...) \
operator __VA_ARGS__ elementType::SystemType& () noexcept;

#define DCB_REF_ASSIGN(elementType) \
elementType::SystemType& operator=(const elementType::SystemType& rhs) noexcept;

#define DCB_REF_NONCONST(elementType) DCB_REF_CONVERSION(elementType) DCB_REF_ASSIGN(elementType)
#define DCB_REF_CONST(elementType) DCB_REF_CONVERSION(elementType, const)

#define DCB_PTR_CONVERSION(elementType,...) \
operator __VA_ARGS__ elementType::SystemType*() noexcept;

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
		virtual ~LayoutElement();

		// 获取此元素的字符串签名（递归）
		virtual std::string GetSignature() const noexcept = 0;

		virtual bool Exists() const noexcept
		{
			return true;
		}

		// [] 仅适用于结构体；通过名称访问成员
		virtual LayoutElement& operator[](const std::string& key);

		virtual const LayoutElement& operator[](const std::string& key) const;

		// LayoutEle() 仅适用于数组；获取数组类型布局对象
		virtual LayoutElement& LayoutEle();

		virtual const LayoutElement& LayoutEle() const;

		// 基于偏移量的函数 仅在完成后才起作用！
		size_t GetOffsetBegin() const noexcept;

		virtual size_t GetOffsetEnd() const noexcept = 0;

		// 获取从偏移量得出的字节大小
		size_t GetSizeInBytes() const noexcept;

		// 仅适用于 Structs；添加 LayoutElement
		template<typename T>
		LayoutElement& Add(const std::string& key) noexcept;

		// 仅适用于数组；设置元素的类型和大小
		template<typename T>
		LayoutElement& Set(size_t size_in) noexcept;

		// 返回偏移量的值，该值上升到下一个 16 字节边界（如果尚未到达边界）
		static size_t GetNextBoundaryOffset(size_t offset);

		DCB_RESOLVE_BASE(Matrix);
		DCB_RESOLVE_BASE(Float4);
		DCB_RESOLVE_BASE(Float3);
		DCB_RESOLVE_BASE(Float2);
		DCB_RESOLVE_BASE(Float);
		DCB_RESOLVE_BASE(Bool);

	protected:
		// 设置元素和子元素的所有偏移量，返回此元素之后的偏移量
		virtual size_t Finalize(size_t offset) = 0;
		// 计算此元素的大小（以字节为单位），考虑数组和结构上的填充
		virtual size_t ComputeSize() const noexcept = 0;

	protected:
		size_t m_offset = 0u;
	};

	DCB_LEAF_ELEMENT(Matrix, DirectX::XMFLOAT4X4);
	DCB_LEAF_ELEMENT(Float4, DirectX::XMFLOAT4);
	DCB_LEAF_ELEMENT(Float3, DirectX::XMFLOAT3);
	DCB_LEAF_ELEMENT(Float2, DirectX::XMFLOAT2);
	DCB_LEAF_ELEMENT(Float, float);
	DCB_LEAF_ELEMENT_IMPL(Bool, bool, 4u);

	// 结构体布局
	class Struct : public LayoutElement
	{
	public:
		//using LayoutElement::LayoutElement; // 使用基类的构造函数
		LayoutElement& operator[](const std::string& key) override final;

		const LayoutElement& operator[](const std::string& key) const override final;

		size_t GetOffsetEnd() const noexcept override final;

		std::string GetSignature() const noexcept override;

		// 添加元素布局
		void Add(const std::string& name, std::unique_ptr<LayoutElement> pElement) noexcept;

	protected:
		size_t Finalize(size_t offset) override final;

		size_t ComputeSize() const noexcept override final;

	private:
		static size_t CalculatePaddingBeforeElement(size_t offset, size_t size) noexcept;

	private:
		std::unordered_map<std::string, LayoutElement*> m_map; // 成员名对应的布局
		std::vector<std::unique_ptr<LayoutElement>> m_elements; // 成员布局
	};

	class Array : public LayoutElement
	{
	public:
		size_t GetOffsetEnd() const noexcept override final;

		void Set(std::unique_ptr<LayoutElement> pElement, size_t size_in) noexcept;

		// 拿到数组的元素布局
		LayoutElement& LayoutEle() override final;

		const LayoutElement& LayoutEle() const override final;

		std::string GetSignature() const noexcept override;

		bool IndexInBounds(size_t index) const noexcept;

	protected:
		// 数组末端的offset
		size_t Finalize(size_t offset) override final;

		size_t ComputeSize() const noexcept override final;

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
		Layout();

		Layout(std::shared_ptr<LayoutElement> pLayout);

		LayoutElement& operator[](const std::string& key);

		size_t GetSizeInByte() const noexcept;

		template<typename T>
		LayoutElement& Add(const std::string& key) noexcept
		{
			return m_pLayout->Add<T>(key);
		}

		std::shared_ptr<LayoutElement> Finalize();

		std::string GetSignature() const noexcept;

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
			Ptr(ConstElementRef& ref);

			DCB_PTR_CONVERSION(Matrix, const);
			DCB_PTR_CONVERSION(Float4, const);
			DCB_PTR_CONVERSION(Float3, const);
			DCB_PTR_CONVERSION(Float2, const);
			DCB_PTR_CONVERSION(Float, const);
			DCB_PTR_CONVERSION(Bool, const);

		private:
			ConstElementRef& m_ref;
		};
	public:
		ConstElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset);

		bool Exists() const noexcept;

		ConstElementRef operator[](const std::string& key) noexcept;

		ConstElementRef operator[](size_t index) noexcept;

		Ptr operator&() noexcept;

		DCB_REF_CONST(Matrix);
		DCB_REF_CONST(Float4);
		DCB_REF_CONST(Float3);
		DCB_REF_CONST(Float2);
		DCB_REF_CONST(Float);
		DCB_REF_CONST(Bool);

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
			Ptr(ElementRef& ref);

			DCB_PTR_CONVERSION(Matrix);
			DCB_PTR_CONVERSION(Float4);
			DCB_PTR_CONVERSION(Float3);
			DCB_PTR_CONVERSION(Float2);
			DCB_PTR_CONVERSION(Float);
			DCB_PTR_CONVERSION(Bool);

		private:
			ElementRef& m_ref;
		};

	public:
		ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset);

		bool Exists() const noexcept;

		operator ConstElementRef() const noexcept;

		ElementRef operator[](const std::string& key) noexcept;

		ElementRef operator[](size_t index) noexcept;

		Ptr operator&() noexcept;

		DCB_REF_NONCONST(Matrix);
		DCB_REF_NONCONST(Float4);
		DCB_REF_NONCONST(Float3);
		DCB_REF_NONCONST(Float2);
		DCB_REF_NONCONST(Float);
		DCB_REF_NONCONST(Bool);

	private:
		const class LayoutElement* m_pLayout;
		char* m_pBytes;
		size_t m_offset;
	};

	class Buffer
	{
	public:
		Buffer(Layout& layout);

		ElementRef operator[](const std::string& key);

		ConstElementRef operator[](const std::string& key) const noexcept;

		const char* GetData() const noexcept;

		size_t GetSizeInByte() const noexcept;

		const LayoutElement& GetLayout() const noexcept;

		std::shared_ptr<LayoutElement> CloneLayout() const;

		std::string GetSignature() const noexcept;

	private:
		std::shared_ptr<Struct> m_pLayout; // 常数缓存的布局
		std::vector<char> m_bytes; // 常数缓存的数据
	};
}
