#pragma once
#include "../Bindable.h"
#include <memory>
#include <array>
#include <optional>

namespace Bind
{

	/// <summary>
	/// �����
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
		Microsoft::WRL::ComPtr<ID3D11BlendState> m_pBlender; // ���״̬
		bool m_blending; // �Ƿ���
		std::optional<std::array<float, 4>> m_factors; // �������
	};
}