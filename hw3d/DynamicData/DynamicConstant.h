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
		static constexpr size_t m_hlslSize = 4u; // 着色器中的字节大小
		static constexpr const char* m_code = "BL"; // 类型签名
		static constexpr bool m_valid = true;
	};

	#define X(element) static_assert(Map<element>::m_valid, "Missing map implementation for" #element);
	LEAF_ELEMENT_TYPES
	#undef X

	// 启用从 SysType 到叶类型的反向查找
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

	/* LayoutElements 实例形成一棵树，描述数据缓冲区的布局，支持结构体和数组的嵌套聚合 */
	class LayoutElementEx
	{
	private:
		struct ExtraDataBase
		{
			virtual ~ExtraDataBase() = default;
		};
		friend class RawLayoutEx;
		friend struct ExtraData;
	public:
		std::string GetSignature() const noexcept;

		bool Exists() const noexcept;

		std::pair<size_t, const LayoutElementEx*> CalculateIndexingOffset(size_t offset, size_t index) const noexcept;

		LayoutElementEx& operator[](const std::string& key) noexcept;
		const LayoutElementEx& operator[](const std::string& key) const noexcept;

		LayoutElementEx& ToLayout() noexcept;
		const LayoutElementEx& ToLayout() const noexcept;

		size_t GetOffsetBegin() const noexcept;
		size_t GetOffsetEnd() const noexcept;

		size_t GetSizeInBytes() const noexcept;

		// 仅适用于结构体
		LayoutElementEx& Add(Type addedType, std::string name) noexcept;
		template<Type typeAdded>
		LayoutElementEx& Add(std::string key) noexcept
		{
			return Add(typeAdded, std::move(key));
		}

		// 仅适用于数组
		LayoutElementEx& Set(Type addedType, size_t size) noexcept;
		template<Type typeAdded>
		LayoutElementEx& Set(size_t size) noexcept
		{
			return Set(typeAdded, size);
		}

		// 返回叶类型的偏移量，用于在调试中使用类型检查进行读/写
		template<typename T>
		size_t Resolve() const noexcept
		{
			switch (m_type)
			{
			#define X(element) case element: assert(typeid(Map<element>::SysType) == typeid(T)); return *m_offset;
			LEAF_ELEMENT_TYPES
			#undef X
			default:
				assert(false);
				return 0u;
			}
			return 0u;
		}

	private:
		// 构造一个空的布局元素
		LayoutElementEx() noexcept = default;
		LayoutElementEx(Type typeIn) noexcept;

		/* 设置元素和子元素的所有偏移量，必要时添加填充，直接返回此元素后的偏移量 */
		size_t Finalize(size_t offsetIn) noexcept;

		// 聚合类型的 GetSignature 实现
		std::string GetSignatureForStruct() const noexcept;
		std::string GetSignatureForArray() const noexcept;

		// 聚合类型的 Finalize 实现
		size_t FinalizeForStruct(size_t offsetIn);
		size_t FinalizeForArray(size_t offsetIn);

		static LayoutElementEx& GetEmptyElement() noexcept
		{
			static LayoutElementEx empty{};
			return empty;
		}

		// 返回偏移量的值，该值上升到下一个 16 字节边界（如果尚未到达边界）
		static size_t AdvanceToBoundary(size_t offset) noexcept;
		// 如果内存块跨越边界则返回 true
		static bool CrossesBoundary(size_t offset, size_t size) noexcept;
		// 如果块跨越边界，则将偏移量推进到下一个边界
		static size_t AdvanceIfCrossesBoundary(size_t offset, size_t size) noexcept;
		// 检查字符串作为结构键的有效性
		static bool ValidateSymbolName(const std::string& name) noexcept;


	private:
		/* 每个元素都存储自己的偏移量。这使得查找其在字节缓冲区中的位置变得很快。涉及数组的情况需要特殊处理 */
		std::optional<size_t> m_offset;
		Type m_type = EEmpty;
		std::unique_ptr<ExtraDataBase> m_pExtraData;
	};

	/* 布局类充当外壳，用于保存 LayoutElement 树的根。客户端不直接创建 LayoutElement，而是
	创建原始布局，然后使用它来访问元素并从那里添加。构建完成后，原始布局将移至 Codex（通常
	通过 Buffer::Make），内部布局元素树将被“交付”（最终确定并移出）。Codex 返回烘焙布局，
	然后缓冲区可以使用它来初始化自身。烘焙布局还可用于直接初始化多个缓冲区。烘焙布局在概念
	上是不可变的。无法构造基本布局类。*/
	class LayoutEx
	{
		friend class LayoutCodex;
	public:
		size_t GetSizeInBytes() const noexcept;
		std::string GetSignature() const noexcept;
	protected:
		LayoutEx(std::shared_ptr<LayoutElementEx> pRoot) noexcept;
	protected:
		std::shared_ptr<LayoutElementEx> m_pRootLayout;
	};


	class RawLayoutEx : public LayoutEx
	{
		friend class LayoutCodex;
	public:
		RawLayoutEx() noexcept;
		// 输入根结构
		LayoutElementEx& operator[](const std::string& key) noexcept;
		// 向根结构体添加一个元素
		template<Type type>
		LayoutElementEx& Add(const std::string& key) noexcept
		{
			return m_pRootLayout->Add<type>(key);
		}
	private:
		// 用根部的一个空结构重置此对象
		void ClearRoot() noexcept;
		// 完成布局然后放弃（通过放弃根布局元素）
		std::shared_ptr<LayoutElementEx> DeliverRoot() noexcept;
	};

	// Cooked Layout 代表一个已完成并注册的 Layout shell 对象，布局树已修复
	class CookedLayoutEx : public LayoutEx
	{
		friend class BufferEx;
		friend class LayoutCodex;
	public:
		// 输入根结构（const 以禁用布局的变异）
		const LayoutElementEx& operator[](const std::string& key) const noexcept;
		// 获取布局树根
		std::shared_ptr<LayoutElementEx> ShareRoot() const noexcept;
	private:
		// Codex 使用此 ctor 返回已烘焙的布局
		CookedLayoutEx(std::shared_ptr<LayoutElementEx> pRoot) noexcept;
		// 用于窃取布局树
		std::shared_ptr<LayoutElementEx> RelinquishRoot() const noexcept;
	};

	class ConstElementRefEx
	{
		friend class BufferEx;
		friend class ElementRefEx;
	public:
		class Ptr
		{
			friend ConstElementRefEx;
		public:
			// 转换为获取支持的 SysType 的只读指针
			template<typename T>
			operator const T* () const noexcept
			{
				static_assert(ReverseMap<std::remove_const_t<T>>::m_valid, "Unsupported SysType used in pointer conversion");
				return &static_cast<const T&>(*m_ref);
			}
		private:
			Ptr(const ConstElementRefEx* ref) noexcept;
		private:
			const ConstElementRefEx* m_ref;
		};
	public:
		/* 检查索引元素是否实际存在，这是可能的，因为如果你使用不存在的键输入到
		结构体中，它仍将返回一个空的布局元素，这将启用此测试，但不会启用任何其他类型的访问 */
		bool Exists() const noexcept;
		// key into the current element as a struct
		ConstElementRefEx operator[](const std::string& key) const noexcept;
		// index into the current element as an array
		ConstElementRefEx operator[](size_t index) const noexcept;
		// emit a pointer proxy object
		Ptr operator&() const noexcept;
		// conversion for reading as a supported SysType
		template<typename T>
		operator const T& () const noexcept
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::m_valid, "Unsupported SysType used in conversion");
			return *reinterpret_cast<const T*>(m_pBytes + m_offset + m_pLayout->Resolve<T>());
		}
	private:
		// refs should only be constructable by other refs or by the buffer
		ConstElementRefEx(const LayoutElementEx* pLayout, const char* pBytes, size_t offset) noexcept;
	private:

		// this offset is the offset that is built up by indexing into arrays
		// accumulated for every array index in the path of access into the structure
		size_t m_offset;
		const LayoutElementEx* m_pLayout;
		const char* m_pBytes;
	};

	// version of ConstElementRef that also allows writing to the bytes of Buffer
	// see above in ConstElementRef for detailed description
	class ElementRefEx
	{
		friend class BufferEx;
	public:
		class Ptr
		{
			friend ElementRefEx;
		public:
			// conversion to read/write pointer to supported SysType
			template<typename T>
			operator T* () const noexcept
			{
				static_assert(ReverseMap<std::remove_const_t<T>>::m_valid, "Unsupported SysType used in pointer conversion");
				return &static_cast<T&>(*m_ref);
			}
		private:
			Ptr(ElementRefEx* ref) noexcept;
		private:
			ElementRefEx* m_ref;
		};

	public:
		operator ConstElementRefEx() const noexcept;
		bool Exists() const noexcept;
		ElementRefEx operator[](const std::string& key) const noexcept;
		ElementRefEx operator[](size_t index) const noexcept;

		// optionally set value if not an empty Ref
		template<typename S>
		bool SetIfExists(const S& val) noexcept
		{
			if (Exists())
			{
				*this = val;
				return true;
			}
			return false;
		}

		Ptr operator&() const noexcept;
		// conversion for reading/writing as a supported SysType
		template<typename T>
		operator T& () const noexcept
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::m_valid, "Unsupported SysType used in conversion");
			return *reinterpret_cast<T*>(m_pBytes + m_offset + m_pLayout->Resolve<T>());
		}
		// assignment for writing to as a supported SysType
		template<typename T>
		T& operator=(const T& rhs) const noexcept
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::m_valid, "Unsupported SysType used in assignment");
			return static_cast<T&>(*this) = rhs;
		}

	private:
		// refs should only be constructable by other refs or by the buffer
		ElementRefEx(const LayoutElementEx* pLayout, char* pBytes, size_t offset) noexcept;

	private:
		size_t m_offset;
		const LayoutElementEx* m_pLayout;
		char* m_pBytes;
	};

	// The buffer object is a combination of a raw byte buffer with a LayoutElement
	// tree structure which acts as an view/interpretation/overlay for those bytes
	// operator [] indexes into the root Struct, returning a Ref shell that can be
	// used to further index if struct/array, returning further Ref shells, or used
	// to access the data stored in the buffer if a Leaf element type
	class BufferEx
	{
	public:
		// various resources can be used to construct a Buffer
		BufferEx(RawLayoutEx&& lay) noexcept;
		BufferEx(const CookedLayoutEx& lay) noexcept;
		BufferEx(CookedLayoutEx&& lay) noexcept;
		BufferEx(const BufferEx& buffer) noexcept;
		// have to be careful with this one...
		// the buffer that has once been pilfered must not be used :x
		BufferEx(BufferEx&& buffer) noexcept;
		// how you begin indexing into buffer (root is always Struct)
		ElementRefEx operator[](const std::string& key) noexcept;
		// if Buffer is const, you only get to index into the buffer with a read-only proxy
		ConstElementRefEx operator[](const std::string& key) const noexcept;
		// get the raw bytes
		const char* GetData() const noexcept;
		// size of the raw byte buffer
		size_t GetSizeInBytes() const noexcept;
		const LayoutElementEx& GetRootLayoutElement() const noexcept;
		// copy bytes from another buffer (layouts must match)
		void CopyFrom(const BufferEx& other) noexcept;
		// return another sptr to the layout root
		std::shared_ptr<LayoutElementEx> ShareLayoutRoot() const noexcept;
	private:
		std::shared_ptr<LayoutElementEx> m_pLayoutRoot;
		std::vector<char> m_bytes;
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
