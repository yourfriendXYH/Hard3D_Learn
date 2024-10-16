#pragma once
#include "../Bindable.h"
#include <memory>

namespace Bind
{

	/// <summary>
	/// �����
	/// </summary>
	class Blender : public Bindable
	{
	public:
		Blender(Graphics& gfx, bool blending);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<Blender> Resolve(Graphics& gfx, bool blending);
		static std::string GenerateUID(bool blending);
		std::string GetUID() const noexcept override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11BlendState> m_pBlender; // ���״̬
		bool m_blending;
	};
}