#pragma once
#include "../Bindable.h"
#include "../DynamicData/DynamicConstant.h"

namespace Bind
{
	// 扩展的像素常数缓存，适配DynamicConstant
	class PixelConstantBufferEx : public Bindable
	{
	public:
		PixelConstantBufferEx(Graphics& gfx, std::shared_ptr<DynamicData::LayoutElement> layout, UINT slot)
			:
			PixelConstantBufferEx(gfx, layout, slot, nullptr)
		{
		}

		PixelConstantBufferEx(Graphics& gfx, const DynamicData::Buffer& buf, UINT slot)
			:
			PixelConstantBufferEx(gfx, buf.ShareLayout(), slot, &buf)
		{
		}

		// 更新像素常数缓存
		void Update(Graphics& gfx, const DynamicData::Buffer& buf)
		{
			// 判断是否为同一块数据
			assert(&buf.GetLayout() == &*m_pLayout);

			D3D11_MAPPED_SUBRESOURCE msr;
			GetContext(gfx)->Map(m_pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
			memcpy(msr.pData, buf.GetData(), buf.GetSizeInByte());
			GetContext(gfx)->Unmap(m_pConstantBuffer.Get(), 0u);
		}

		void Bind(Graphics& gfx) noexcept override
		{
			// 绑定常数缓存到像素着色器
			GetContext(gfx)->PSSetConstantBuffers(m_slot, 1u, m_pConstantBuffer.GetAddressOf());
		}

		const DynamicData::LayoutElement& GetLayout() const noexcept
		{
			return *m_pLayout;
		}
	private:
		PixelConstantBufferEx(Graphics& gfx, std::shared_ptr<DynamicData::LayoutElement> playout_in, UINT slot, const DynamicData::Buffer* pBuf)
			:
			m_slot(slot),
			m_pLayout(std::move(playout_in))
		{
			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = (UINT)m_pLayout->GetSizeInBytes();
			cbd.StructureByteStride = 0u;

			if (nullptr != pBuf)
			{
				D3D11_SUBRESOURCE_DATA csd = {};
				csd.pSysMem = pBuf->GetData();
				// 创建常数缓存
				GetDevice(gfx)->CreateBuffer(&cbd, &csd, &m_pConstantBuffer);
			}
			else
			{
				GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &m_pConstantBuffer);
			}
		}

	public:
		std::shared_ptr<DynamicData::LayoutElement> m_pLayout; // 缓存布局
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;
		UINT m_slot; // 插槽索引
	};
}