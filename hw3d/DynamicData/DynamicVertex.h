#pragma once
#include <vector>
#include <assert.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <string>

namespace DynamicData
{

	struct BGRAColor
	{
		unsigned char a;
		unsigned char r;
		unsigned char g;
		unsigned char b;
	};

	// ��������Ľṹ����
	class VertexLayout
	{
	public:
		// ������������
		enum ElementType
		{
			Position2D, // ��������
			Position3D,
			Texture2D,	// ��������
			Normal,		// ���߷���
			Tangent, // ���߷���
			Bitangent, // �����ߴ�ֱ�ķ���?
			Float3Color,// ��ɫ
			Float4Color,
			BGRAColor,
			Count, // ???
		};

		// �����������͵�map
		template<ElementType> struct Map;
		template<> struct Map<Position2D>
		{
			using m_sysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT m_dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* m_semantic = "Position";
			static constexpr const char* m_code = "P2";
		};
		template<> struct Map<Position3D>
		{
			using m_sysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT m_dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* m_semantic = "Position";
			static constexpr const char* m_code = "P3";
		};
		template<> struct Map<Texture2D>
		{
			using m_sysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT m_dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* m_semantic = "TexCoord"; // ��������
			static constexpr const char* m_code = "T2";
		};
		template<> struct Map<Normal>
		{
			using m_sysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT m_dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* m_semantic = "Normal";
			static constexpr const char* m_code = "N";
		};
		template<> struct Map<Tangent>
		{
			using m_sysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT m_dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* m_semantic = "Tangent";
			static constexpr const char* m_code = "Nt";
		};
		template<> struct Map<Bitangent>
		{
			using m_sysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT m_dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* m_semantic = "Bitangent";
			static constexpr const char* m_code = "Nb";
		};
		template<> struct Map<Float3Color>
		{
			using m_sysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT m_dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* m_semantic = "Color";
			static constexpr const char* m_code = "C3";
		};
		template<> struct Map<Float4Color>
		{
			using m_sysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT m_dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* m_semantic = "Color";
			static constexpr const char* m_code = "C4";
		};
		template<> struct Map<BGRAColor>
		{
			using m_sysType = DynamicData::BGRAColor;
			static constexpr DXGI_FORMAT m_dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* m_semantic = "Color";
			static constexpr const char* m_code = "C8";
		};

		// �������ݶ���
		class Element
		{
		public:
			Element(ElementType type, size_t offset)
				:
				m_type(type),
				m_offset(offset)
			{
			}
			// ĩ�˵�ƫ��ֵ
			size_t GetOffsetAfter() const
			{
				return m_offset + Size(); // ����������
			}
			// ��ʼƫ��ֵ
			size_t GetOffset() const
			{
				return m_offset;
			}

			ElementType GetType() const
			{
				return m_type;
			}

			// ���ݴ�С
			size_t Size() const
			{
				return SizeOf(m_type);
			}

			size_t SizeOf(ElementType type) const
			{
				//return sizeof(Map<type>::m_sysType);

				switch (type)
				{
				case VertexLayout::Position2D:
					return sizeof(Map<VertexLayout::Position2D>::m_sysType);
				case VertexLayout::Position3D:
					return sizeof(Map<VertexLayout::Position3D>::m_sysType);
				case VertexLayout::Texture2D:
					return sizeof(Map<VertexLayout::Texture2D>::m_sysType);
				case VertexLayout::Normal:
					return sizeof(Map<VertexLayout::Normal>::m_sysType);
				case VertexLayout::Tangent:
					return sizeof(Map<VertexLayout::Tangent>::m_sysType);
				case VertexLayout::Bitangent:
					return sizeof(Map<VertexLayout::Bitangent>::m_sysType);
				case VertexLayout::Float3Color:
					return sizeof(Map<VertexLayout::Float3Color>::m_sysType);
				case VertexLayout::Float4Color:
					return sizeof(Map<VertexLayout::Float4Color>::m_sysType);
				case VertexLayout::BGRAColor:
					return sizeof(Map<VertexLayout::BGRAColor>::m_sysType);
				}
				assert("Invalid element type" && false);
				return 0u;
			}

			// ��ȡ�������ݵ�����
			D3D11_INPUT_ELEMENT_DESC GetDesc() const;

			const char* GetCode() const noexcept;

		private:
			// �����㲼��������ֵ
			template<VertexLayout::ElementType Type>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc(size_t offset)
			{
				return { Map<Type>::m_semantic, 0, Map<Type>::m_dxgiFormat, 0, (UINT)offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			}

		private:
			ElementType m_type; // ����
			size_t m_offset; // ��ʼƫ��ֵ
		};

	public:

		// �������ͻ�ȡ��Ӧ��Element
		template<ElementType type>
		const Element& Resolve() const
		{
			for (auto& element : m_elements)
			{
				if (type == element.GetType())
				{
					return element;
				}
			}
			assert("Could not resolve element type" && false);
			return m_elements.front();
		}
		// ����������ȡ
		const Element& ResolveByIndex(size_t index) const
		{
			assert(index < m_elements.size());
			return m_elements[index];
		}

		// ��Ӷ�������ݽṹ
		//template<ElementType type>
		VertexLayout& Append(VertexLayout::ElementType type)
		{
			m_elements.emplace_back(type, Size());
			return *this;
		}

		// ��ȡ�������͵��ܴ�С
		size_t Size() const
		{
			return m_elements.empty() ? 0u : m_elements.back().GetOffsetAfter(); // ���ӻ�ȡ
		}

		size_t GetElementCount() const noexcept
		{
			return m_elements.size();
		}

		// ����D3D�Ķ��㲼������
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> descArr;
			descArr.reserve(GetElementCount());
			for (const auto& element : m_elements)
			{
				descArr.emplace_back(element.GetDesc());
			}
			return std::move(descArr);
		}

		std::string GetCode() const	noexcept;

	private:
		std::vector<Element> m_elements; // �������ݽṹ

	};

	// �������ݵ�ת����
	class Vertex
	{
		friend class VerticesBuffer;
	private:
		Vertex(char* data, const VertexLayout& layout)
			:
			m_pData(data),
			m_vertexLayout(layout)
		{
		}

	public:
		// ��ȡ���������е�ĳһ������ֵ
		template<DynamicData::VertexLayout::ElementType Type>
		auto& Attr()
		{
			const auto& element = m_vertexLayout.Resolve<Type>();
			auto attribute = m_pData + element.GetOffset();
			return *reinterpret_cast<VertexLayout::Map<Type>::m_sysType*>(attribute);
		}


		// ���������ݸ�ֵ
		template<typename T>
		void SetAttributeByIndex(size_t index, T&& value)
		{
			// ��ȡ�����е�ĳ������
			const auto& element = m_vertexLayout.ResolveByIndex(index);
			auto pAttribute = m_pData + element.GetOffset();
			switch (element.GetType())
			{
			case VertexLayout::Position2D:
				SetAttribute<VertexLayout::Position2D>(pAttribute, std::forward<T>(value));
				break;
			case VertexLayout::Position3D:
				SetAttribute<VertexLayout::Position3D>(pAttribute, std::forward<T>(value));
				break;
			case VertexLayout::Texture2D:
				SetAttribute<VertexLayout::Texture2D>(pAttribute, std::forward<T>(value));
				break;
			case VertexLayout::Normal:
				SetAttribute<VertexLayout::Normal>(pAttribute, std::forward<T>(value));
				break;
			case VertexLayout::Tangent:
				SetAttribute<VertexLayout::Tangent>(pAttribute, std::forward<T>(value));
				break;
			case VertexLayout::Bitangent:
				SetAttribute<VertexLayout::Bitangent>(pAttribute, std::forward<T>(value));
				break;
			case VertexLayout::Float3Color:
				SetAttribute<VertexLayout::Float3Color>(pAttribute, std::forward<T>(value));
				break;
			case VertexLayout::Float4Color:
				SetAttribute<VertexLayout::Float4Color>(pAttribute, std::forward<T>(value));
				break;
			case VertexLayout::BGRAColor:
				SetAttribute<VertexLayout::BGRAColor>(pAttribute, std::forward<T>(value));
				break;
			default:
				assert("Bad element type" && false);
			}
		}

	private:

		// ģ��ݹ���������㸳ֵ
		template<typename First, typename ...Rest>
		void SetAttributeByIndex(size_t index, First&& first, Rest&&... rest)
		{
			SetAttributeByIndex(index, std::forward<First>(first));
			SetAttributeByIndex(index + 1, std::forward<Rest>(rest)...);
		}

		// �������е����ݸ�ֵ
		template<VertexLayout::ElementType DestLayoutType, typename Src>
		void SetAttribute(char* attribute, Src&& value)
		{
			using Dest = typename VertexLayout::Map<DestLayoutType>::m_sysType;
			if constexpr (std::is_assignable<Dest, Src>::value)
			{
				*reinterpret_cast<Dest*>(attribute) = value;
			}
			else
			{
				assert("Parameter attribute type mismatch" && false);
			}
		}

	private:

		char* m_pData = nullptr;
		const VertexLayout& m_vertexLayout;
	};

	// Vertex��const��װ
	class ConstVertex
	{
	public:
		ConstVertex(const Vertex& vertex)
			:
			m_vertex(vertex)
		{
		}

		template<VertexLayout::ElementType Type>
		const auto& Attr() const noexcept
		{
			return const_cast<Vertex&>(m_vertex).Attr<Type>();
		}

	private:
		Vertex m_vertex;
	};

	// �������ݼ���
	class VerticesBuffer
	{
	public:
		VerticesBuffer(VertexLayout layout, size_t size = 0u) noexcept;

	public:
		const VertexLayout& GetVertexLayout() const
		{
			return m_vertexLayout;
		}

		void Resize(size_t newSize) noexcept;

		// ���������
		size_t VerticesSize() const
		{
			return m_buffer.size() / m_vertexLayout.Size();
		}

		// �������ݴ�С
		size_t Size() const
		{
			return m_buffer.size();
		}

		// ��Ӷ������� ÿ�����һ���������������
		template<typename ...Params>
		void EmplaceBack(Params&&... params)
		{
			// ����Ĳ��������붥��ṹƥ��
			assert(sizeof...(params) == m_vertexLayout.GetElementCount() && "Param count doesn't match number of vertex elements");

			m_buffer.resize(m_buffer.size() + m_vertexLayout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
		}

		// ���ж������ݵ�β��
		Vertex Back()
		{
			assert(m_buffer.size() != 0u);
			return Vertex{ m_buffer.data() + m_buffer.size() - m_vertexLayout.Size(), m_vertexLayout };
		}
		// ���ж������ݵ��ײ�
		Vertex Front()
		{
			return Vertex{ m_buffer.data(), m_vertexLayout };
		}

		// ������ȡ��������
		Vertex operator[](size_t index)
		{
			assert(index < VerticesSize());
			return Vertex{ m_buffer.data() + m_vertexLayout.Size() * index, m_vertexLayout };
		}

		// const��װ
		ConstVertex Back() const
		{
			return const_cast<VerticesBuffer*>(this)->Back();
		}
		ConstVertex Front() const
		{
			return const_cast<VerticesBuffer*>(this)->Back();
		}
		ConstVertex operator[](size_t index) const
		{
			return const_cast<VerticesBuffer&>(*this)[index];
		}

		const char* GetData() const noexcept
		{
			return m_buffer.data();
		}

	private:
		std::vector<char> m_buffer; // ���еĶ�������
		VertexLayout m_vertexLayout; // ����ṹ
	};

}