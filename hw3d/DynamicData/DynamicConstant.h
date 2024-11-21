#pragma once
#include <assert.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <DirectXMath.h>
#include <optional>

// 旧代码
/* 宏简化了向 LayoutElement 添加基本虚拟解析函数的过
程，解析函数是验证多态类型访问的系统，这里的基础都提
供了默认的断言失败行为，例如 Float1 将重载 ResolveFloat1() 并提供不会失败的实现*/
#define DCB_RESOLVE_BASE(elementType) \
virtual size_t Resolve ## elementType() const noexcept;

/*这允许对 Float1、Bool 等叶布局类型进行元类模板化。
使用宏，以便可以自动生成重载 ResolveXXX 本身的名称。
请参阅 LayoutElement 基类以了解函数含义*/
// 布局元素宏
#define DCB_LEAF_ELEMENT_IMPL(elementType, sysType, hlslSize) \
class elementType : public LayoutElement \
{ \
	friend LayoutElement; \
public: \
	using SystemType = sysType; \
	size_t Resolve ## elementType() const noexcept final; \
	size_t GetOffsetEnd() const noexcept final; \
	std::string GetSignature() const noexcept override; \
protected: \
	size_t Finalize(size_t offset) noexcept final; \
	size_t ComputeSize() const noexcept final; \
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

// 重构代码
#define LEAF_ELEMENT_TYPES \
	X(EFloat) \
	X(EFloat2) \
	X(EFloat3) \
	X(EFloat4) \
	X(EMatrix) \
	X(EBool)

namespace DynamicData
{
	/// <summary>
	/// 动态常数缓存重构代码
	/// </summary>
	enum Type
	{
		// 用宏定义枚举
		#define X(element) element,
		LEAF_ELEMENT_TYPES
		#undef X
		EStruct,
		EArray,
		EEmpty,
	};

	// 模板特化类型
	template<Type type>
	struct Map
	{
		static constexpr bool m_valid = false;
	};
	
	template<> struct Map<EFloat>
	{
		using SysType = float;
		static constexpr size_t m_hlslSize = sizeof(SysType); // 着色器中的字节大小
		static constexpr const char* m_code = "F1"; // 类型签名
		static constexpr bool m_valid = true;
	};

	template<> struct Map<EFloat2>
	{
		using SysType = DirectX::XMFLOAT2;
		static constexpr size_t m_hlslSize = sizeof(SysType); // 着色器中的字节大小
		static constexpr const char* m_code = "F2"; // 类型签名
		static constexpr bool m_valid = true;
	};

	template<> struct Map<EFloat3>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr size_t m_hlslSize = sizeof(SysType); // 着色器中的字节大小
		static constexpr const char* m_code = "F3"; // 类型签名
		static constexpr bool m_valid = true;
	};

	template<> struct Map<EFloat4>
	{
		using SysType = DirectX::XMFLOAT4;
		static constexpr size_t m_hlslSize = sizeof(SysType); // 着色器中的字节大小
		static constexpr const char* m_code = "F4"; // 类型签名
		static constexpr bool m_valid = true;
	};

	template<> struct Map<EMatrix>
	{
		using SysType = DirectX::XMFLOAT4X4;
		static constexpr size_t m_hlslSize = sizeof(SysType); // 着色器中的字节大小
		static constexpr const char* m_code = "M4"; // 类型签名
		static constexpr bool m_valid = true;
	};

	template<> struct Map<EBool>
	{
		using SysType = bool;
		static constexpr size_t m_hlslSize = sizeof(SysType); // 着色器中的字节大小
		static constexpr const char* m_code = "BL"; // 类型签名
		static constexpr bool m_valid = true;
	};

	#define X(element) static_assert(Map<element>::m_valid, "Missing map implementation for" #element);
	LEAF_ELEMENT_TYPES
	#undef X

	template<typename T>
	struct ReverseMap
	{
		static constexpr bool m_valid = false;
	};

	#define X(element) \
	template<> struct ReverseMap<Map<element>::SysType> \
	{ \
		static constexpr Type m_type = element; \
		static constexpr bool m_valid = true; \
	};
	LEAF_ELEMENT_TYPES
	#undef X

	class LayoutElementEx
	{
	private:
		struct ExtraDataBase
		{
			virtual ~ExtraDataBase() = default;
		};
	public:
	protected:
	private:
		std::optional<size_t> m_offset;
		Type m_type = EEmpty;
		std::unique_ptr<ExtraDataBase> m_pExtraData;
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// 动态常数缓存旧代码
	/// </summary>
	class Struct;
	class Array;
	class Layout;
	// 单个元素布局
	class LayoutElement
	{
		friend class Struct;
		friend class Array;
		friend class RawLayout;
	public:
		virtual ~LayoutElement();

		// 获取此元素的字符串签名（递归）
		virtual std::string GetSignature() const noexcept = 0;

		virtual bool Exists() const noexcept
		{
			return true;
		}

		// [] 仅适用于结构体；通过名称访问成员
		virtual LayoutElement& operator[](const std::string& key) noexcept;

		virtual const LayoutElement& operator[](const std::string& key) const noexcept;

		// LayoutEle() 仅适用于数组；获取数组类型布局对象
		virtual LayoutElement& LayoutEle() noexcept;

		virtual const LayoutElement& LayoutEle() const noexcept;

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
		virtual size_t Finalize(size_t offset) noexcept = 0;
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
		friend LayoutElement;
	public:
		//using LayoutElement::LayoutElement; // 使用基类的构造函数
		LayoutElement& operator[](const std::string& key) noexcept override final;

		const LayoutElement& operator[](const std::string& key) const noexcept override final;

		size_t GetOffsetEnd() const noexcept override final;

		std::string GetSignature() const noexcept override;

		// 添加元素布局
		void Add(const std::string& name, std::unique_ptr<LayoutElement> pElement) noexcept;

	protected:
		// Struct() = default;

		size_t Finalize(size_t offset) noexcept override final;

		size_t ComputeSize() const noexcept override final;

	private:
		static size_t CalculatePaddingBeforeElement(size_t offset, size_t size) noexcept;

	private:
		std::unordered_map<std::string, LayoutElement*> m_map; // 成员名对应的布局
		std::vector<std::unique_ptr<LayoutElement>> m_elements; // 成员布局
	};

	class Array : public LayoutElement
	{
		friend LayoutElement;
	public:
		size_t GetOffsetEnd() const noexcept override final;

		void Set(std::unique_ptr<LayoutElement> pElement, size_t size_in) noexcept;

		// 拿到数组的元素布局
		LayoutElement& LayoutEle() noexcept override final;

		const LayoutElement& LayoutEle() const noexcept override final;

		std::string GetSignature() const noexcept override;

		bool IndexInBounds(size_t index) const noexcept;

	protected:
		// Array() = default;
		// 数组末端的offset
		size_t Finalize(size_t offset) noexcept override final;

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
		struct Enabler : public T {};
		pStruct->Add(key, std::make_unique<T>());
		return *this;
	}

	template<typename T>
	LayoutElement& LayoutElement::Set(size_t size_in) noexcept
	{
		auto pArray = dynamic_cast<Array*>(this);
		assert(nullptr != pArray);
		struct Enabler : public T {};
		pArray->Set(std::make_unique<Enabler>(), size_in);
		return *this;
	}

	// 缓存布局的封装类
	class Layout
	{
		friend class LayoutCodex;
		friend class Buffer;
	public:
		size_t GetSizeInByte() const noexcept;

		std::string GetSignature() const noexcept;
	protected:
		Layout() noexcept;
		Layout(std::shared_ptr<LayoutElement> pLayout) noexcept;

	protected:
		std::shared_ptr<LayoutElement> m_pLayout;
	};

	/* 原始布局 表示尚未最终确定的布局，可以通过添加布局节点来编辑注册结构 */
	class RawLayout : public Layout
	{
		friend class LayoutCodex;
	public:
		RawLayout() = default;

		LayoutElement& operator[](const std::string& key);

		template<typename T>
		LayoutElement& Add(const std::string& key) noexcept
		{
			return m_pLayout->Add<T>(key);
		}

	private:
		// 交付布局
		std::shared_ptr<LayoutElement> DeliverLayout() noexcept;
		void ClearLayout() noexcept;
	};

	class CookedLayout : public Layout
	{
		friend class LayoutCodex;
		friend class Buffer;
	public:
		const LayoutElement& operator[](const std::string& key);
		// 将对共享指针的引用添加到布局树根
		std::shared_ptr<LayoutElement> ShareRoot() const noexcept;
	private:
		// this ctor used by Codex to return cooked layouts
		CookedLayout(std::shared_ptr<LayoutElement> pLayout) noexcept;
		// 用于窃取布局树
		std::shared_ptr<LayoutElement> ReliquishLayout() const noexcept;
	};

	class ConstElementRef
	{
		friend class ElementRef;
		friend class Buffer;
	public:
		class Ptr
		{
			friend ConstElementRef;
		public:
			DCB_PTR_CONVERSION(Matrix, const);
			DCB_PTR_CONVERSION(Float4, const);
			DCB_PTR_CONVERSION(Float3, const);
			DCB_PTR_CONVERSION(Float2, const);
			DCB_PTR_CONVERSION(Float, const);
			DCB_PTR_CONVERSION(Bool, const);

		private:
			Ptr(ConstElementRef& ref) noexcept;
		private:
			ConstElementRef& m_ref;
		};
	public:
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
		ConstElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset) noexcept;

	private:
		size_t m_offset;
		const class LayoutElement* m_pLayout;
		char* m_pBytes;
	};

	class ElementRef
	{
		friend class Buffer;
	public:
		class Ptr
		{
			friend ElementRef;
		public:
			DCB_PTR_CONVERSION(Matrix);
			DCB_PTR_CONVERSION(Float4);
			DCB_PTR_CONVERSION(Float3);
			DCB_PTR_CONVERSION(Float2);
			DCB_PTR_CONVERSION(Float);
			DCB_PTR_CONVERSION(Bool);
		private:
			Ptr(ElementRef& ref) noexcept;

		private:
			ElementRef& m_ref;
		};

	public:
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
		ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset) noexcept;

	private:
		const class LayoutElement* m_pLayout;
		char* m_pBytes;
		size_t m_offset;
	};

	class Buffer
	{
	public:
		Buffer(RawLayout&& layout) noexcept;
		Buffer(const CookedLayout& layout) noexcept;
		Buffer(CookedLayout&& layout) noexcept;
		// 拷贝构造
		Buffer(const Buffer& buffer) noexcept;
		Buffer(Buffer&& buffer) noexcept;

		ElementRef operator[](const std::string& key);

		ConstElementRef operator[](const std::string& key) const noexcept;

		const char* GetData() const noexcept;

		size_t GetSizeInByte() const noexcept;

		const LayoutElement& GetLayout() const noexcept;

		// 拷贝函数
		void CopyFrom(const Buffer& buffer) noexcept;

		std::shared_ptr<LayoutElement> ShareLayout() const noexcept;

	private:
		std::shared_ptr<LayoutElement> m_pLayout; // 常数缓存的布局
		std::vector<char> m_bytes; // 常数缓存的数据
	};
}
