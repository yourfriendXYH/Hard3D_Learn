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
		static constexpr size_t m_hlslSize = sizeof(SysType); // ��ɫ���е��ֽڴ�С
		static constexpr const char* m_code = "BL"; // ����ǩ��
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
