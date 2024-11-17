#pragma once
#include <assert.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <DirectXMath.h>

#define DCB_RESOLVE_BASE(elementType) \
virtual size_t Resolve ## elementType() const noexcept;

// ����Ԫ�غ�
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

// ����ת������� �� ��ֵ����� ����д(__VA_ARGS__�����)
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
	// ����Ԫ�ز���
	class LayoutElement
	{
		friend class Struct;
		friend class Array;
		friend class Layout;
	public:
		virtual ~LayoutElement();

		// ��ȡ��Ԫ�ص��ַ���ǩ�����ݹ飩
		virtual std::string GetSignature() const noexcept = 0;

		virtual bool Exists() const noexcept
		{
			return true;
		}

		// [] �������ڽṹ�壻ͨ�����Ʒ��ʳ�Ա
		virtual LayoutElement& operator[](const std::string& key);

		virtual const LayoutElement& operator[](const std::string& key) const;

		// LayoutEle() �����������飻��ȡ�������Ͳ��ֶ���
		virtual LayoutElement& LayoutEle();

		virtual const LayoutElement& LayoutEle() const;

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
		virtual size_t Finalize(size_t offset) = 0;
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
	public:
		//using LayoutElement::LayoutElement; // ʹ�û���Ĺ��캯��
		LayoutElement& operator[](const std::string& key) override final;

		const LayoutElement& operator[](const std::string& key) const override final;

		size_t GetOffsetEnd() const noexcept override final;

		std::string GetSignature() const noexcept override;

		// ���Ԫ�ز���
		void Add(const std::string& name, std::unique_ptr<LayoutElement> pElement) noexcept;

	protected:
		size_t Finalize(size_t offset) override final;

		size_t ComputeSize() const noexcept override final;

	private:
		static size_t CalculatePaddingBeforeElement(size_t offset, size_t size) noexcept;

	private:
		std::unordered_map<std::string, LayoutElement*> m_map; // ��Ա����Ӧ�Ĳ���
		std::vector<std::unique_ptr<LayoutElement>> m_elements; // ��Ա����
	};

	class Array : public LayoutElement
	{
	public:
		size_t GetOffsetEnd() const noexcept override final;

		void Set(std::unique_ptr<LayoutElement> pElement, size_t size_in) noexcept;

		// �õ������Ԫ�ز���
		LayoutElement& LayoutEle() override final;

		const LayoutElement& LayoutEle() const override final;

		std::string GetSignature() const noexcept override;

		bool IndexInBounds(size_t index) const noexcept;

	protected:
		// ����ĩ�˵�offset
		size_t Finalize(size_t offset) override final;

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

	// ���沼�ֵķ�װ��
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
		std::shared_ptr<Struct> m_pLayout; // ��������Ĳ���
		std::vector<char> m_bytes; // �������������
	};
}
