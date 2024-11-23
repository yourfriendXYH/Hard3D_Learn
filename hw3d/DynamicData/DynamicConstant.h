#pragma once
#include <assert.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <DirectXMath.h>
#include <optional>

// �ɴ���
/* ������� LayoutElement ��ӻ���������������Ĺ�
�̣�������������֤��̬���ͷ��ʵ�ϵͳ������Ļ�������
����Ĭ�ϵĶ���ʧ����Ϊ������ Float1 ������ ResolveFloat1() ���ṩ����ʧ�ܵ�ʵ��*/
#define DCB_RESOLVE_BASE(elementType) \
virtual size_t Resolve ## elementType() const noexcept;

/*������� Float1��Bool ��Ҷ�������ͽ���Ԫ��ģ�廯��
ʹ�ú꣬�Ա�����Զ��������� ResolveXXX ��������ơ�
����� LayoutElement �������˽⺯������*/
// ����Ԫ�غ�
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

// ����ת������� �� ��ֵ����� ����д(__VA_ARGS__�����)
#define DCB_REF_CONVERSION(elementType,...) \
operator __VA_ARGS__ elementType::SystemType& () noexcept;

#define DCB_REF_ASSIGN(elementType) \
elementType::SystemType& operator=(const elementType::SystemType& rhs) noexcept;

#define DCB_REF_NONCONST(elementType) DCB_REF_CONVERSION(elementType) DCB_REF_ASSIGN(elementType)
#define DCB_REF_CONST(elementType) DCB_REF_CONVERSION(elementType, const)

#define DCB_PTR_CONVERSION(elementType,...) \
operator __VA_ARGS__ elementType::SystemType*() noexcept;

// �ع�����
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
	/// ��̬���������ع�����
	/// </summary>
	enum Type
	{
		// �ú궨��ö��
		#define X(element) element,
		LEAF_ELEMENT_TYPES
		#undef X
		EStruct,
		EArray,
		EEmpty,
	};

	// ģ���ػ�����
	template<Type type>
	struct Map
	{
		static constexpr bool m_valid = false;
	};
	
	template<> struct Map<EFloat>
	{
		using SysType = float;
		static constexpr size_t m_hlslSize = sizeof(SysType); // ��ɫ���е��ֽڴ�С
		static constexpr const char* m_code = "F1"; // ����ǩ��
		static constexpr bool m_valid = true;
	};

	template<> struct Map<EFloat2>
	{
		using SysType = DirectX::XMFLOAT2;
		static constexpr size_t m_hlslSize = sizeof(SysType); // ��ɫ���е��ֽڴ�С
		static constexpr const char* m_code = "F2"; // ����ǩ��
		static constexpr bool m_valid = true;
	};

	template<> struct Map<EFloat3>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr size_t m_hlslSize = sizeof(SysType); // ��ɫ���е��ֽڴ�С
		static constexpr const char* m_code = "F3"; // ����ǩ��
		static constexpr bool m_valid = true;
	};

	template<> struct Map<EFloat4>
	{
		using SysType = DirectX::XMFLOAT4;
		static constexpr size_t m_hlslSize = sizeof(SysType); // ��ɫ���е��ֽڴ�С
		static constexpr const char* m_code = "F4"; // ����ǩ��
		static constexpr bool m_valid = true;
	};

	template<> struct Map<EMatrix>
	{
		using SysType = DirectX::XMFLOAT4X4;
		static constexpr size_t m_hlslSize = sizeof(SysType); // ��ɫ���е��ֽڴ�С
		static constexpr const char* m_code = "M4"; // ����ǩ��
		static constexpr bool m_valid = true;
	};

	template<> struct Map<EBool>
	{
		using SysType = bool;
		static constexpr size_t m_hlslSize = 4u; // ��ɫ���е��ֽڴ�С
		static constexpr const char* m_code = "BL"; // ����ǩ��
		static constexpr bool m_valid = true;
	};

	#define X(element) static_assert(Map<element>::m_valid, "Missing map implementation for" #element);
	LEAF_ELEMENT_TYPES
	#undef X

	// ���ô� SysType ��Ҷ���͵ķ������
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

	/* LayoutElements ʵ���γ�һ�������������ݻ������Ĳ��֣�֧�ֽṹ��������Ƕ�׾ۺ� */
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

		// �������ڽṹ��
		LayoutElementEx& Add(Type addedType, std::string name) noexcept;
		template<Type typeAdded>
		LayoutElementEx& Add(std::string key) noexcept
		{
			return Add(typeAdded, std::move(key));
		}

		// ������������
		LayoutElementEx& Set(Type addedType, size_t size) noexcept;
		template<Type typeAdded>
		LayoutElementEx& Set(size_t size) noexcept
		{
			return Set(typeAdded, size);
		}

		// ����Ҷ���͵�ƫ�����������ڵ�����ʹ�����ͼ����ж�/д
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
		// ����һ���յĲ���Ԫ��
		LayoutElementEx() noexcept = default;
		LayoutElementEx(Type typeIn) noexcept;

		/* ����Ԫ�غ���Ԫ�ص�����ƫ��������Ҫʱ�����䣬ֱ�ӷ��ش�Ԫ�غ��ƫ���� */
		size_t Finalize(size_t offsetIn) noexcept;

		// �ۺ����͵� GetSignature ʵ��
		std::string GetSignatureForStruct() const noexcept;
		std::string GetSignatureForArray() const noexcept;

		// �ۺ����͵� Finalize ʵ��
		size_t FinalizeForStruct(size_t offsetIn);
		size_t FinalizeForArray(size_t offsetIn);

		static LayoutElementEx& GetEmptyElement() noexcept
		{
			static LayoutElementEx empty{};
			return empty;
		}

		// ����ƫ������ֵ����ֵ��������һ�� 16 �ֽڱ߽磨�����δ����߽磩
		static size_t AdvanceToBoundary(size_t offset) noexcept;
		// ����ڴ���Խ�߽��򷵻� true
		static bool CrossesBoundary(size_t offset, size_t size) noexcept;
		// ������Խ�߽磬��ƫ�����ƽ�����һ���߽�
		static size_t AdvanceIfCrossesBoundary(size_t offset, size_t size) noexcept;
		// ����ַ�����Ϊ�ṹ������Ч��
		static bool ValidateSymbolName(const std::string& name) noexcept;


	private:
		/* ÿ��Ԫ�ض��洢�Լ���ƫ��������ʹ�ò��������ֽڻ������е�λ�ñ�úܿ졣�漰����������Ҫ���⴦�� */
		std::optional<size_t> m_offset;
		Type m_type = EEmpty;
		std::unique_ptr<ExtraDataBase> m_pExtraData;
	};

	/* ������䵱��ǣ����ڱ��� LayoutElement ���ĸ����ͻ��˲�ֱ�Ӵ��� LayoutElement������
	����ԭʼ���֣�Ȼ��ʹ����������Ԫ�ز���������ӡ�������ɺ�ԭʼ���ֽ����� Codex��ͨ��
	ͨ�� Buffer::Make�����ڲ�����Ԫ����������������������ȷ�����Ƴ�����Codex ���غ決���֣�
	Ȼ�󻺳�������ʹ��������ʼ�������決���ֻ�������ֱ�ӳ�ʼ��������������決�����ڸ���
	���ǲ��ɱ�ġ��޷�������������ࡣ*/
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
		// ������ṹ
		LayoutElementEx& operator[](const std::string& key) noexcept;
		// ����ṹ�����һ��Ԫ��
		template<Type type>
		LayoutElementEx& Add(const std::string& key) noexcept
		{
			return m_pRootLayout->Add<type>(key);
		}
	private:
		// �ø�����һ���սṹ���ô˶���
		void ClearRoot() noexcept;
		// ��ɲ���Ȼ�������ͨ������������Ԫ�أ�
		std::shared_ptr<LayoutElementEx> DeliverRoot() noexcept;
	};

	// Cooked Layout ����һ������ɲ�ע��� Layout shell ���󣬲��������޸�
	class CookedLayoutEx : public LayoutEx
	{
		friend class BufferEx;
		friend class LayoutCodex;
	public:
		// ������ṹ��const �Խ��ò��ֵı��죩
		const LayoutElementEx& operator[](const std::string& key) const noexcept;
		// ��ȡ��������
		std::shared_ptr<LayoutElementEx> ShareRoot() const noexcept;
	private:
		// Codex ʹ�ô� ctor �����Ѻ決�Ĳ���
		CookedLayoutEx(std::shared_ptr<LayoutElementEx> pRoot) noexcept;
		// ������ȡ������
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
			// ת��Ϊ��ȡ֧�ֵ� SysType ��ֻ��ָ��
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
		/* �������Ԫ���Ƿ�ʵ�ʴ��ڣ����ǿ��ܵģ���Ϊ�����ʹ�ò����ڵļ����뵽
		�ṹ���У����Խ�����һ���յĲ���Ԫ�أ��⽫���ô˲��ԣ������������κ��������͵ķ��� */
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
	/// ��̬��������ɴ���
	/// </summary>
	class Struct;
	class Array;
	class Layout;
	// ����Ԫ�ز���
	class LayoutElement
	{
		friend class Struct;
		friend class Array;
		friend class RawLayout;
	public:
		virtual ~LayoutElement();

		// ��ȡ��Ԫ�ص��ַ���ǩ�����ݹ飩
		virtual std::string GetSignature() const noexcept = 0;

		virtual bool Exists() const noexcept
		{
			return true;
		}

		// [] �������ڽṹ�壻ͨ�����Ʒ��ʳ�Ա
		virtual LayoutElement& operator[](const std::string& key) noexcept;

		virtual const LayoutElement& operator[](const std::string& key) const noexcept;

		// LayoutEle() �����������飻��ȡ�������Ͳ��ֶ���
		virtual LayoutElement& LayoutEle() noexcept;

		virtual const LayoutElement& LayoutEle() const noexcept;

		// ����ƫ�����ĺ��� ������ɺ�������ã�
		size_t GetOffsetBegin() const noexcept;

		virtual size_t GetOffsetEnd() const noexcept = 0;

		// ��ȡ��ƫ�����ó����ֽڴ�С
		size_t GetSizeInBytes() const noexcept;

		// �������� Structs����� LayoutElement
		template<typename T>
		LayoutElement& Add(const std::string& key) noexcept;

		// �����������飻����Ԫ�ص����ͺʹ�С
		template<typename T>
		LayoutElement& Set(size_t size_in) noexcept;

		// ����ƫ������ֵ����ֵ��������һ�� 16 �ֽڱ߽磨�����δ����߽磩
		static size_t GetNextBoundaryOffset(size_t offset);

		DCB_RESOLVE_BASE(Matrix);
		DCB_RESOLVE_BASE(Float4);
		DCB_RESOLVE_BASE(Float3);
		DCB_RESOLVE_BASE(Float2);
		DCB_RESOLVE_BASE(Float);
		DCB_RESOLVE_BASE(Bool);

	protected:
		// ����Ԫ�غ���Ԫ�ص�����ƫ���������ش�Ԫ��֮���ƫ����
		virtual size_t Finalize(size_t offset) noexcept = 0;
		// �����Ԫ�صĴ�С�����ֽ�Ϊ��λ������������ͽṹ�ϵ����
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

	// �ṹ�岼��
	class Struct : public LayoutElement
	{
		friend LayoutElement;
	public:
		//using LayoutElement::LayoutElement; // ʹ�û���Ĺ��캯��
		LayoutElement& operator[](const std::string& key) noexcept override final;

		const LayoutElement& operator[](const std::string& key) const noexcept override final;

		size_t GetOffsetEnd() const noexcept override final;

		std::string GetSignature() const noexcept override;

		// ���Ԫ�ز���
		void Add(const std::string& name, std::unique_ptr<LayoutElement> pElement) noexcept;

	protected:
		// Struct() = default;

		size_t Finalize(size_t offset) noexcept override final;

		size_t ComputeSize() const noexcept override final;

	private:
		static size_t CalculatePaddingBeforeElement(size_t offset, size_t size) noexcept;

	private:
		std::unordered_map<std::string, LayoutElement*> m_map; // ��Ա����Ӧ�Ĳ���
		std::vector<std::unique_ptr<LayoutElement>> m_elements; // ��Ա����
	};

	class Array : public LayoutElement
	{
		friend LayoutElement;
	public:
		size_t GetOffsetEnd() const noexcept override final;

		void Set(std::unique_ptr<LayoutElement> pElement, size_t size_in) noexcept;

		// �õ������Ԫ�ز���
		LayoutElement& LayoutEle() noexcept override final;

		const LayoutElement& LayoutEle() const noexcept override final;

		std::string GetSignature() const noexcept override;

		bool IndexInBounds(size_t index) const noexcept;

	protected:
		// Array() = default;
		// ����ĩ�˵�offset
		size_t Finalize(size_t offset) noexcept override final;

		size_t ComputeSize() const noexcept override final;

	private:
		size_t m_size = 0u; // �����С
		std::unique_ptr<LayoutElement> m_upElement; // ����Ԫ�صĲ���
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

	// ���沼�ֵķ�װ��
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

	/* ԭʼ���� ��ʾ��δ����ȷ���Ĳ��֣�����ͨ����Ӳ��ֽڵ����༭ע��ṹ */
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
		// ��������
		std::shared_ptr<LayoutElement> DeliverLayout() noexcept;
		void ClearLayout() noexcept;
	};

	class CookedLayout : public Layout
	{
		friend class LayoutCodex;
		friend class Buffer;
	public:
		const LayoutElement& operator[](const std::string& key);
		// ���Թ���ָ���������ӵ���������
		std::shared_ptr<LayoutElement> ShareRoot() const noexcept;
	private:
		// this ctor used by Codex to return cooked layouts
		CookedLayout(std::shared_ptr<LayoutElement> pLayout) noexcept;
		// ������ȡ������
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
		// ��������
		Buffer(const Buffer& buffer) noexcept;
		Buffer(Buffer&& buffer) noexcept;

		ElementRef operator[](const std::string& key);

		ConstElementRef operator[](const std::string& key) const noexcept;

		const char* GetData() const noexcept;

		size_t GetSizeInByte() const noexcept;

		const LayoutElement& GetLayout() const noexcept;

		// ��������
		void CopyFrom(const Buffer& buffer) noexcept;

		std::shared_ptr<LayoutElement> ShareLayout() const noexcept;

	private:
		std::shared_ptr<LayoutElement> m_pLayout; // ��������Ĳ���
		std::vector<char> m_bytes; // �������������
	};
}
