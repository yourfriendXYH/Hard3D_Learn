#pragma once
#include "../Bindable.h"
#include "../DynamicData/DynamicConstant.h"
#include "TechniqueProbe.h"

namespace Bind
{
	// 扩展的像素常数缓存，适配DynamicConstant
	class ConstantBufferEx : public Bindable
	{
	public:
		// 更新像素常数缓存
		void Update(Graphics& gfx, const DynamicData::BufferEx& buf)
		{
			// 判断是否为同一块数据
			assert(&buf.GetRootLayoutElement() == &GetLayout());

			D3D11_MAPPED_SUBRESOURCE msr;
			GetContext(gfx)->Map(m_pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
			memcpy(msr.pData, buf.GetData(), buf.GetSizeInBytes());
			GetContext(gfx)->Unmap(m_pConstantBuffer.Get(), 0u);
		}

	protected:
		ConstantBufferEx(Graphics& gfx, const DynamicData::LayoutElementEx& layoutRoot, UINT slot, const DynamicData::BufferEx* pBuf)
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
				// 创建常数缓存
				GetDevice(gfx)->CreateBuffer(&cbd, &csd, &m_pConstantBuffer);
			}
			else
			{
				GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &m_pConstantBuffer);
			}
		}

	private:
		virtual const DynamicData::LayoutElementEx& GetLayout() const noexcept = 0;

	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;
		UINT m_slot; // 插槽索引
	};

	class PixelConstantBufferEx : public ConstantBufferEx
	{
	public:
		using ConstantBufferEx::ConstantBufferEx;

		void Bind(Graphics& gfx) noexcept override
		{
			// 绑定常数缓存到像素着色器
			GetContext(gfx)->PSSetConstantBuffers(m_slot, 1u, m_pConstantBuffer.GetAddressOf());
		}
	};

	class VertexConstantBufferEx : public ConstantBufferEx
	{
	public:
		using ConstantBufferEx::ConstantBufferEx;

		void Bind(Graphics& gfx) noexcept override
		{
			// 绑定常数缓存到像素着色器
			GetContext(gfx)->VSSetConstantBuffers(m_slot, 1u, m_pConstantBuffer.GetAddressOf());
		}
	};

	template<class T>
	class CachingConstantBufferEx : public T
	{
	public:
		CachingConstantBufferEx(Graphics& gfx, const DynamicData::CookedLayoutEx& layout, UINT slot)
			:
			T(gfx, *layout.ShareRoot(), slot, nullptr),
			m_buf(DynamicData::BufferEx(layout))
		{
		}

		CachingConstantBufferEx(Graphics& gfx, const DynamicData::BufferEx& buf, UINT slot)
			:
			T(gfx, buf.GetRootLayoutElement(), slot, &buf),
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
				T::Update(gfx, m_buf);
				m_dirty = false;
			}
			T::Bind(gfx);
		}

		void Accept(TechniqueProbe& probe)
		{
			if (probe.VisitBuffer(m_buf))
			{
				m_dirty = true;
			}
		}

	private:
		bool m_dirty = false; // 缓存数据是否置脏
		DynamicData::BufferEx m_buf;
	};

	using CachingPixelConstantBufferEx = CachingConstantBufferEx<PixelConstantBufferEx>;
	using CachingVertexConstantBufferEx = CachingConstantBufferEx<VertexConstantBufferEx>;

	//class NoCachePixelConstantBufferEx : public PixelConstantBufferEx
	//{
	//public:
	//	NoCachePixelConstantBufferEx(Graphics& gfx, const DynamicData::CookedLayoutEx& layout, UINT slot)
	//		:
	//		PixelConstantBufferEx(gfx, *layout.ShareRoot(), slot, nullptr),
	//		m_pLayoutRoot(layout.ShareRoot())
	//	{
	//	}
	//	NoCachePixelConstantBufferEx(Graphics& gfx, const DynamicData::BufferEx& buf, UINT slot)
	//		:
	//		PixelConstantBufferEx(gfx, buf.GetRootLayoutElement(), slot, &buf),
	//		m_pLayoutRoot(buf.ShareLayoutRoot())
	//	{
	//	}

	//	const DynamicData::LayoutElementEx& GetLayout() const noexcept override
	//	{
	//		return *m_pLayoutRoot;
	//	}

	//private:
	//	std::shared_ptr<DynamicData::LayoutElementEx> m_pLayoutRoot;
	//};
}