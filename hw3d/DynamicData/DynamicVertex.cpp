#define DVTX_SOURCE_FILE
#include "DynamicVertex.h"

namespace DynamicData
{
	//const char* VertexLayout::Element::GetCode() const noexcept
	//{
	//	switch (m_type)
	//	{
	//	case Position2D:
	//		return Map<Position2D>::m_code;
	//	case Position3D:
	//		return Map<Position3D>::m_code;
	//	case Texture2D:
	//		return Map<Texture2D>::m_code;
	//	case Normal:
	//		return Map<Normal>::m_code;
	//	case Tangent:
	//		return Map<Tangent>::m_code;
	//	case Bitangent:
	//		return Map<Bitangent>::m_code;
	//	case Float3Color:
	//		return Map<Float3Color>::m_code;
	//	case Float4Color:
	//		return Map<Float4Color>::m_code;
	//	case BGRAColor:
	//		return Map<BGRAColor>::m_code;
	//	}

	//	assert("Invalid element type" && false);

	//	return "Invalid";
	//}

	template<VertexLayout::ElementType type>
	struct SysSizeLookup
	{
		static constexpr auto Exec() noexcept
		{
			return sizeof(VertexLayout::Map<type>::m_sysType);
		}
	};

	constexpr size_t VertexLayout::Element::SizeOf(ElementType type) const noexcept
	{
		return Bridge<SysSizeLookup>(type);
	}

	template<VertexLayout::ElementType type>
	struct CodeLookup
	{
		static constexpr auto Exec() noexcept
		{
			return VertexLayout::Map<type>::m_code;
		}
	};

	const char* VertexLayout::Element::GetCode() const noexcept
	{
		return Bridge<CodeLookup>(m_type);
	}

	template<VertexLayout::ElementType type> 
	struct DescGenerate
	{
		static constexpr D3D11_INPUT_ELEMENT_DESC Exec(size_t offset) noexcept
		{
			return {
				VertexLayout::Map<type>::m_semantic,0,
				VertexLayout::Map<type>::m_dxgiFormat,
				0,(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0
			};
		}
	};

	D3D11_INPUT_ELEMENT_DESC VertexLayout::Element::GetDesc() const
	{
		return Bridge<DescGenerate>(m_type, GetOffset());
	}
	//{
	//	switch (m_type)
	//	{
	//	case DynamicData::VertexLayout::Position2D:
	//		return GenerateDesc<VertexLayout::Position2D>(GetOffset());
	//		break;
	//	case DynamicData::VertexLayout::Position3D:
	//		return GenerateDesc<VertexLayout::Position3D>(GetOffset());
	//		break;
	//	case DynamicData::VertexLayout::Texture2D:
	//		return GenerateDesc<VertexLayout::Texture2D>(GetOffset());
	//		break;
	//	case DynamicData::VertexLayout::Normal:
	//		return GenerateDesc<VertexLayout::Normal>(GetOffset());
	//		break;
	//	case DynamicData::VertexLayout::Tangent:
	//		return GenerateDesc<VertexLayout::Tangent>(GetOffset());
	//		break;
	//	case DynamicData::VertexLayout::Bitangent:
	//		return GenerateDesc<VertexLayout::Bitangent>(GetOffset());
	//		break;
	//	case DynamicData::VertexLayout::Float3Color:
	//		return GenerateDesc<VertexLayout::Float3Color>(GetOffset());
	//		break;
	//	case DynamicData::VertexLayout::Float4Color:
	//		return GenerateDesc<VertexLayout::Float4Color>(GetOffset());
	//		break;
	//	case DynamicData::VertexLayout::BGRAColor:
	//		return GenerateDesc<VertexLayout::BGRAColor>(GetOffset());
	//		break;
	//	case DynamicData::VertexLayout::Count:
	//		break;
	//	default:
	//		break;
	//	}
	//}

	std::string VertexLayout::GetCode() const noexcept
	{
		std::string code;
		for (const auto& e : m_elements)
		{
			code += e.GetCode();
		}

		return code;
	}

	VerticesBuffer::VerticesBuffer(VertexLayout layout, size_t size /*= 0u*/) noexcept
		:
		m_vertexLayout(std::move(layout))
	{
		Resize(size);
	}

	void VerticesBuffer::Resize(size_t newSize) noexcept
	{
		// 将顶点数据设置为指定大小
		const auto currnetSize = VerticesSize();
		if (currnetSize < newSize)
		{
			m_buffer.resize(m_buffer.size() + m_vertexLayout.Size() * (newSize - currnetSize));
		}
	}

}