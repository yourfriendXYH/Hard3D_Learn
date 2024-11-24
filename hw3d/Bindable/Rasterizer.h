#pragma once
#include "../Bindable.h"
#include <array>
#include <memory>

namespace Bind
{
	/// <summary>
	/// ¹âÕ¤»¯×´Ì¬
	/// </summary>
	class Rasterizer : public Bindable
	{
	public:
		Rasterizer(Graphics& gfx, bool twoSided);
		void Bind(Graphics& gfx) noexcept override;
		std::string GetUID() const noexcept;

		static std::shared_ptr<Rasterizer> Resolve(Graphics& gfx, bool twoSided = false);
		static std::string GenerateUID(bool twoSided);
	protected:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizer;
		bool m_twoSided;
	};
}
