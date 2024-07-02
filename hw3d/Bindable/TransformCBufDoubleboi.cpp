#include "TransformCBufDoubleboi.h"

namespace Bind
{
	TransformCBufDoubleboi::TransformCBufDoubleboi(Graphics& gfx, const Drawable& parent, UINT slotV /*= 0u*/, UINT slotP /*= 0u*/)
		:
		TransformCBuf(gfx, parent, slotV)
	{
		if (nullptr == s_pPixelCBuff)
		{
			s_pPixelCBuff = std::make_shared<PixelConstantBuffer<TransformCBuf::Transforms>>(gfx, slotP);
		}
	}

	void TransformCBufDoubleboi::Bind(Graphics& gfx) noexcept
	{
		const auto tempTransforms = GetTransforms(gfx);
		TransformCBuf::UpdateBindImpl(gfx, tempTransforms);
		UpdateBindImpl(gfx, tempTransforms);
	}

	void TransformCBufDoubleboi::UpdateBindImpl(Graphics& gfx, const Transforms& tranforms) noexcept
	{
		s_pPixelCBuff->Update(gfx, tranforms);
		s_pPixelCBuff->Bind(gfx);
	}

	std::shared_ptr<PixelConstantBuffer<TransformCBuf::Transforms>> TransformCBufDoubleboi::s_pPixelCBuff;
}



