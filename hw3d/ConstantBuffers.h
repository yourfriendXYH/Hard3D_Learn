#pragma once

#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	// �������棨������ɫ����
	template<typename C>
	class ConstantBuffer : public Bindable
	{
	public:
		void Update(Graphics& gfx, const C& constants)
		{
			// ��������Դ�����ݣ�
			D3D11_MAPPED_SUBRESOURCE msr;
			// ����bufferӳ������Դ�����ݣ����޸�
			GetContext(gfx)->Map(m_pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
			//size_t tempSize = sizeof(constants);
			//C tempMatrix = constants;
			memcpy(msr.pData, &constants, sizeof(constants));
			GetContext(gfx)->Unmap(m_pConstantBuffer.Get(), 0u);
		};
		ConstantBuffer(Graphics& gfx, const C& constants, UINT slot = 0u) : m_slot(slot)
		{
			D3D11_BUFFER_DESC cbd = {};
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(constants);
			cbd.StructureByteStride = 0u;
			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = &constants;
			GetDevice(gfx)->CreateBuffer(&cbd, &csd, &m_pConstantBuffer);
		};
		ConstantBuffer(Graphics& gfx, UINT slot = 0u)
			:
			m_slot(slot)
		{
			// ����������
			D3D11_BUFFER_DESC cbd = {};
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(C);
			cbd.StructureByteStride = 0u;
			GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &m_pConstantBuffer);
		};
	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer; // ������������
		UINT m_slot; // ��������Ĳ��
	private:
	};

	// ������ɫ����������
	template<typename C>
	class VertexConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::m_pConstantBuffer;
		using Bindable::GetContext;
		using ConstantBuffer<C>::m_slot;
	public:
		using ConstantBuffer<C>::ConstantBuffer;

		// �󶨳������浽��Ⱦ������
		void Bind(Graphics& gfx) noexcept override
		{
			GetContext(gfx)->VSSetConstantBuffers(m_slot, 1u, m_pConstantBuffer.GetAddressOf());
		};

		static std::shared_ptr<VertexConstantBuffer> Resolve(Graphics& gfx, const C& constants, UINT slot = 0u)
		{
			return BindCodex::Resolve<VertexConstantBuffer>(gfx, constants, slot);
		}

		static std::shared_ptr<VertexConstantBuffer> Resolve(Graphics& gfx, UINT slot = 0u)
		{
			return BindCodex::Resolve<VertexConstantBuffer>(gfx, slot);
		}

		static std::string GenerateUID(const C& constants, UINT slot = 0u)
		{
			return GenerateUID(slot);
		}

		static std::string GenerateUID(UINT slot = 0u)
		{
			using namespace std::string_literals;
			return typeid(VertexConstantBuffer).name() + "#"s + std::to_string(slot);
		}

		std::string GetUID() const noexcept override
		{
			return GenerateUID(m_slot);
		}
	};

	// ������ɫ����������
	template<typename C>
	class PixelConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::m_pConstantBuffer;
		using Bindable::GetContext;
		using ConstantBuffer<C>::m_slot;
	public:
		using ConstantBuffer<C>::ConstantBuffer;

		void Bind(Graphics& gfx) noexcept override
		{
			GetContext(gfx)->PSSetConstantBuffers(m_slot, 1u, m_pConstantBuffer.GetAddressOf());
		};

		static std::shared_ptr<PixelConstantBuffer> Resolve(Graphics& gfx, const C& constants, UINT slot = 0u)
		{
			return BindCodex::Resolve<PixelConstantBuffer>(gfx, constants, slot);
		}

		static std::shared_ptr<PixelConstantBuffer> Resolve(Graphics& gfx, UINT slot = 0u)
		{
			return BindCodex::Resolve<PixelConstantBuffer>(gfx, slot);
		}

		static std::string GenerateUID(const C& constants, UINT slot = 0u)
		{
			return GenerateUID(slot);
		}

		static std::string GenerateUID(UINT slot = 0u)
		{
			using namespace std::string_literals;
			return typeid(PixelConstantBuffer).name() + "#"s + std::to_string(slot);
		}

		std::string GetUID() const noexcept override
		{
			return GenerateUID(m_slot);
		}
	};
}
