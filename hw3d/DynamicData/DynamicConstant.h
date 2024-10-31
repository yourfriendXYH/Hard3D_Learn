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

// ����Ԫ�غ�
#define LEAF_ELEMENT(elementType, sysType) \
class elementType : public LayoutElement \
{ \
public: \
	using LayoutElement::LayoutElement; \
	size_t Resolve ## elementType() const noexcept override final \
	{ \
		return GetOffsetBegin(); \
	} \
	size_t GetOffsetEnd() const noexcept override final \
	{ \
		return GetOffsetBegin() + sizeof(sysType); \
	} \
};

// ����ת������� �� ��ֵ����� ����д
#define REF_CONVERSION(elementType, sysType) \
operator sysType& () noexcept \
{ \
	return *reinterpret_cast<sysType*>(m_pBytes + m_pLayout->Resolve ## elementType()); \
} \
sysType& operator=(const sysType& rhs) noexcept \
{ \
	return static_cast<sysType&>(*this) = rhs; \
}

namespace DynamicData
{
	// ����Ԫ�ز���
	class LayoutElement
	{
	public:
		LayoutElement(size_t offset)
			:
			m_offset(offset)
		{
		}
		virtual LayoutElement& operator[](const char* key)
		{
			assert(false);
			return *this;
		}
		virtual const LayoutElement& operator[](const char* key) const
		{
			assert(false);
			return *this;
		}
		size_t GetOffsetBegin() const noexcept
		{
			return m_offset;
		}
		virtual size_t GetOffsetEnd() const noexcept = 0;

		class Struct& AsStruct() noexcept;

		RESOLVE_BASE(Float3);
		RESOLVE_BASE(Float);

	private:
		size_t m_offset;
	};

	LEAF_ELEMENT(Float3, DirectX::XMFLOAT3);
	LEAF_ELEMENT(Float, float);

	// �ṹ�岼��
	class Struct : public LayoutElement
	{
	public:
		using LayoutElement::LayoutElement; // ʹ�û���Ĺ��캯��
		LayoutElement& operator[](const char* key) override final
		{
			return *m_map.at(key);
		}

		const LayoutElement& operator[](const char* key) const override final
		{
			return *m_map.at(key);
		}

		size_t GetOffsetEnd() const noexcept override final
		{
			return m_elements.empty() ? GetOffsetBegin() : m_elements.back()->GetOffsetEnd();
		}

		// ���Ԫ�ز���
		template<typename T>
		Struct& Add(const std::string& name)
		{
			m_elements.emplace_back(std::make_unique<T>(GetOffsetEnd()));
			if (!m_map.emplace(name, m_elements.back().get()).second)
			{
				assert(false);
			}
			return *this;
		}

	private:
		std::unordered_map<std::string, LayoutElement*> m_map; // ��Ա����Ӧ�Ĳ���
		std::vector<std::unique_ptr<LayoutElement>> m_elements; // ��Ա����
	};

	Struct& LayoutElement::AsStruct() noexcept
	{
		auto pStruct = dynamic_cast<Struct*>(this);
		assert(nullptr != pStruct);
		return *pStruct;
	}

	class ElementRef
	{
	public:
		ElementRef(const LayoutElement* pLayout, char* pBytes)
			:
			m_pLayout(pLayout),
			m_pBytes(pBytes)
		{
		}

		ElementRef operator[](const char* key) noexcept
		{
			return { &(*m_pLayout)[key], m_pBytes };
		}

		//// ����ת���������д
		//operator DirectX::XMFLOAT3& () noexcept
		//{
		//	return *reinterpret_cast<DirectX::XMFLOAT3*>(m_pBytes + m_pLayout->ResolveFloat3());
		//}

		//// ��ֵ�������д
		//DirectX::XMFLOAT3& operator=(const DirectX::XMFLOAT3& rhs) noexcept
		//{
		//	// ���ݲ����õ���Ӧ��Bytes���ݵ�����
		//	auto& ref = *reinterpret_cast<DirectX::XMFLOAT3*>(m_pBytes + m_pLayout->GetOffsetBegin());
		//	ref = rhs;
		//	return ref;
		//}

		REF_CONVERSION(Float3, DirectX::XMFLOAT3);
		REF_CONVERSION(Float, float);

	private:
		const class LayoutElement* m_pLayout;
		char* m_pBytes;
	};

	class Buffer
	{
	public:
		Buffer(const Struct& layout)
			:
			m_pLayout(&layout),
			m_bytes(layout.GetOffsetEnd()) // ���ݲ��ֳ�ʼ�������С
		{
		}
		ElementRef operator[](const char* key)
		{
			return { &(*m_pLayout)[key], m_bytes.data() };
		}

	private:
		const class LayoutElement* m_pLayout; // ��������Ĳ���
		std::vector<char> m_bytes; // �������������
	};
}
