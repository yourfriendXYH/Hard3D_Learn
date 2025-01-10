#pragma once

#include <memory>
#include "Graphics.h"
#include "Bindable/GraphicsResource.h"

class Drawable;
class TechniqueProbe;

namespace Bind
{
	class Bindable : public GraphicsResource
	{
	public:
		virtual void Bind(Graphics& gfx) noexcept = 0;
		virtual std::string GetUID() const noexcept
		{
			assert(false);
			return "";
		}
		virtual ~Bindable() = default;
	public:
		virtual void InitializeParentReference(const Drawable& parent) noexcept {};

		virtual void Accept(TechniqueProbe& probe) {};
	};

	class CloningBindable : public Bindable
	{
	public:
		virtual std::unique_ptr<CloningBindable> Clone() const noexcept = 0;
	};
}

