#pragma once
#include "../Bindable.h"
#include "../DynamicData/DynamicConstant.h"

namespace Bind
{
	// ��չ�����س������棬����DynamicConstant
	class PixelConstantBufferEx : public Bindable
	{
	public:
		// �������س�������
		void Update(Graphics& gfx, const DynamicData::BufferEx& buf)
		{
			// �ж��Ƿ�Ϊͬһ������
			assert(&buf.GetRootLayoutElement() == &GetLayout());

			D3D11_MAPPED_SUBRESOURCE msr;
			GetContext(gfx)->Map(m_pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
			memcpy(msr.pData, buf.GetData(), buf.GetSizeInBytes());
			GetContext(gfx)->Unmap(m_pConstantBuffer.Get(), 0u);
		}

		void Bind(Graphics& gfx) noexcept override
		{
			// �󶨳������浽������ɫ��
			GetContext(gfx)->PSSetConstantBuffers(m_slot, 1u, m_pConstantBuffer.GetAddressOf());
		}

	protected:
		PixelConstantBufferEx(Graphics& gfx, const DynamicData::LayoutElementEx& layoutRoot, UINT slot, const DynamicData::BufferEx* pBuf)
			:
			m_slot(slot)
		{
			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = (UINT)layoutRoot.GetSizeInBytes();
			cbd.StructureByteStride = 0u;

			if (nullptr != pBuf)
			{
				D3D11_SUBRESOURCE_DATA csd = {};
				csd.pSysMem = pBuf->GetData();
				// ������������
				GetDevice(gfx)->CreateBuffer(&cbd, &csd, &m_pConstantBuffer);
			}
			else
			{
				GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &m_pConstantBuffer);
			}
		}

	private:
		virtual const DynamicData::LayoutElementEx& GetLayout() const noexcept = 0;

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;
		UINT m_slot; // �������
	};

	class CachingPixelConstantBufferEx : public PixelConstantBufferEx
	{
	public:
		CachingPixelConstantBufferEx(Graphics& gfx, const DynamicData::CookedLayoutEx& layout, UINT slot)
			:
			PixelConstantBufferEx(gfx, *layout.ShareRoot(), slot, nullptr),
			m_buf(DynamicData::BufferEx(layout))
		{
		}

		CachingPixelConstantBufferEx(Graphics& gfx, const DynamicData::BufferEx& buf, UINT slot)
			:
			PixelConstantBufferEx(gfx, buf.GetRootLayoutElement(), slot, &buf),
			m_buf(buf)
		{
		}

		const DynamicData::LayoutElementEx& GetLayout() const noexcept override
		{
			return m_buf.GetRootLayoutElement();
		}

		const DynamicData::BufferEx& GetBuffer() const noexcept
		{
			return m_buf;
		}

		void SetBuffer(const DynamicData::BufferEx& buf_in)
		{
			m_buf.CopyFrom(buf_in);
			m_dirty = true;
		}

		void Bind(Graphics& gfx) noexcept override
		{
			if (m_dirty)
			{
				Update(gfx, m_buf);
				m_dirty = false;
			}
			PixelConstantBufferEx::Bind(gfx);
		}

	private:
		bool m_dirty = false; // ���������Ƿ�����
		DynamicData::BufferEx m_buf;
	};

	class NoCachePixelConstantBufferEx : public PixelConstantBufferEx
	{
	public:
		NoCachePixelConstantBufferEx(Graphics& gfx, const DynamicData::CookedLayoutEx& layout, UINT slot)
			:
			PixelConstantBufferEx(gfx, *layout.ShareRoot(), slot, nullptr),
			m_pLayoutRoot(layout.ShareRoot())
		{
		}
		NoCachePixelConstantBufferEx(Graphics& gfx, const DynamicData::BufferEx& buf, UINT slot)
			:
			PixelConstantBufferEx(gfx, buf.GetRootLayoutElement(), slot, &buf),
			m_pLayoutRoot(buf.ShareLayoutRoot())
		{
		}

		const DynamicData::LayoutElementEx& GetLayout() const noexcept override
		{
			return *m_pLayoutRoot;
		}

	private:
		std::shared_ptr<DynamicData::LayoutElementEx> m_pLayoutRoot;
	};
}