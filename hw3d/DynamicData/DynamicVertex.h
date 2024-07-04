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

	// 单个顶点的结构布局
	class VertexLayout
	{
	public:
		// 顶点数据类型
		enum ElementType
		{
			Position2D, // 顶点坐标
			Position3D,
			Texture2D,	// 纹理坐标
			Normal,		// 法线方向
			Tangent, // 切线方向
			Bitangent, // 与切线垂直的方向?
			Float3Color,// 颜色
			Float4Color,
			BGRAColor,
			Count, // ???
		};

		// 顶点数据类型的map
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
			static constexpr const char* m_semantic = "TexCoord"; // 纹理坐标
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

		// 顶点数据对象
		class Element
		{
		public:
			Element(ElementType type, size_t offset)
				:
				m_type(type),
				m_offset(offset)
			{
			}
			// 末端的偏移值
			size_t GetOffsetAfter() const
			{
				return m_offset + Size(); // 加上自身长度
			}
			// 起始偏移值
			size_t GetOffset() const
			{
				return m_offset;
			}

			ElementType GetType() const
			{
				return m_type;
			}

			// 数据大小
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

			// 获取单个数据的描述
			D3D11_INPUT_ELEMENT_DESC GetDesc() const;

			const char* GetCode() const noexcept;

		private:
			// 给顶点布局描述赋值
			template<VertexLayout::ElementType Type>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc(size_t offset)
			{
				return { Map<Type>::m_semantic, 0, Map<Type>::m_dxgiFormat, 0, (UINT)offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			}

		private:
			ElementType m_type; // 类型
			size_t m_offset; // 起始偏移值
		};

	public:

		// 根据类型获取对应的Element
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
		// 根据索引获取
		const Element& ResolveByIndex(size_t index) const
		{
			assert(index < m_elements.size());
			return m_elements[index];
		}

		// 添加顶点的数据结构
		//template<ElementType type>
		VertexLayout& Append(VertexLayout::ElementType type)
		{
			m_elements.emplace_back(type, Size());
			return *this;
		}

		// 获取数据类型的总大小
		size_t Size() const
		{
			return m_elements.empty() ? 0u : m_elements.back().GetOffsetAfter(); // 叠加获取
		}

		size_t GetElementCount() const noexcept
		{
			return m_elements.size();
		}

		// 生成D3D的顶点布局描述
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
		std::vector<Element> m_elements; // 顶点数据结构

	};

	// 顶点数据的转换类
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
		// 获取单个顶点中的某一个属性值
		template<DynamicData::VertexLayout::ElementType Type>
		auto& Attr()
		{
			const auto& element = m_vertexLayout.Resolve<Type>();
			auto attribute = m_pData + element.GetOffset();
			return *reinterpret_cast<VertexLayout::Map<Type>::m_sysType*>(attribute);
		}


		// 给顶点数据赋值
		template<typename T>
		void SetAttributeByIndex(size_t index, T&& value)
		{
			// 获取顶点中的某个数据
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

		// 模板递归给单个顶点赋值
		template<typename First, typename ...Rest>
		void SetAttributeByIndex(size_t index, First&& first, Rest&&... rest)
		{
			SetAttributeByIndex(index, std::forward<First>(first));
			SetAttributeByIndex(index + 1, std::forward<Rest>(rest)...);
		}

		// 给顶点中的数据赋值
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

	// Vertex的const封装
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

	// 顶点数据集合
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

		// 顶点的数量
		size_t VerticesSize() const
		{
			return m_buffer.size() / m_vertexLayout.Size();
		}

		// 顶点数据大小
		size_t Size() const
		{
			return m_buffer.size();
		}

		// 添加顶点数据 每次添加一个顶点的所有数据
		template<typename ...Params>
		void EmplaceBack(Params&&... params)
		{
			// 传入的参数数量与顶点结构匹配
			assert(sizeof...(params) == m_vertexLayout.GetElementCount() && "Param count doesn't match number of vertex elements");

			m_buffer.resize(m_buffer.size() + m_vertexLayout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
		}

		// 所有顶点数据的尾部
		Vertex Back()
		{
			assert(m_buffer.size() != 0u);
			return Vertex{ m_buffer.data() + m_buffer.size() - m_vertexLayout.Size(), m_vertexLayout };
		}
		// 所有顶点数据的首部
		Vertex Front()
		{
			return Vertex{ m_buffer.data(), m_vertexLayout };
		}

		// 索引获取顶点数据
		Vertex operator[](size_t index)
		{
			assert(index < VerticesSize());
			return Vertex{ m_buffer.data() + m_vertexLayout.Size() * index, m_vertexLayout };
		}

		// const封装
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
		std::vector<char> m_buffer; // 所有的顶点数据
		VertexLayout m_vertexLayout; // 顶点结构
	};

}