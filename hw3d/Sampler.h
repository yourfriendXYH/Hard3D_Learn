#pragma once
#include "Bindable.h"
#include <memory>

// ÎÆÀí²ÉÑù
namespace Bind
{
	class Sampler : public Bindable
	{
	public:
		Sampler(Graphics& gfx, bool anisoEnable, bool reflect);
		void Bind(Graphics& gfx) noexcept override;

		static std::shared_ptr<Sampler> Resolve(Graphics& gfx, bool anisoEnable = true, bool reflect = false);
		static std::string GenerateUID(bool anisoEnable, bool reflect);
		std::string GetUID() const noexcept;

	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
		bool m_anisoEnable;
		bool m_reflect;
	};
}

