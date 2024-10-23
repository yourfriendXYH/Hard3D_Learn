#pragma once
#include "../Bindable.h"
#include <memory>
#include <array>
#include <optional>

namespace Bind
{

	/// <summary>
	/// 混合器
	/// </summary>
	class Blender : public Bindable
	{
	public:
		Blender(Graphics& gfx, bool blending, std::optional<float> factor_in = {});
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<Blender> Resolve(Graphics& gfx, bool blending, std::optional<float> factor = {});
		static std::string GenerateUID(bool blending, std::optional<float> factor);
		std::string GetUID() const noexcept override;

		void SetFactor(float factor) noexcept;
		float GetFactor() const noexcept;

	protected:
		Microsoft::WRL::ComPtr<ID3D11BlendState> m_pBlender; // 混合状态
		bool m_blending; // 是否混合
		std::optional<std::array<float, 4>> m_factors; // 混合因子
	};
}