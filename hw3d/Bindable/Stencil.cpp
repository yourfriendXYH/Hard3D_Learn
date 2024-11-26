#include "Stencil.h"

namespace Bind
{
	std::shared_ptr<Stencil> Stencil::Resolve(Graphics& gfx, Mode mode)
	{
		return BindCodex::Resolve<Stencil>(gfx, mode);
	}

	std::string Stencil::GenerateUID(Mode mode)
	{
		using namespace std::string_literals;
		const auto modeName = [mode]() {
			switch (mode)
			{
			case Bind::Stencil::Mode::Off:
				return "off"s;
			case Bind::Stencil::Mode::Write:
				return "write"s;
			case Bind::Stencil::Mode::Mask:
				return "mask"s;
			}
			return "ERROR"s;
			};

		return typeid(Stencil).name() + "#"s + modeName();
	}

	std::string Bind::Stencil::GetUID() const noexcept
	{
		return GenerateUID(m_mode);
	}

}

