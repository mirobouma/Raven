#pragma once
#include "core/reflection/type/type.h"

#define ObjectVariant(object) Raven::Meta::RVariant { object, Raven::Meta::Variant_Policy::WrapObject( ) }

namespace Raven
{
	class RObject
	{
	public:
		virtual ~RObject(void) { }

		virtual Meta::RType GetType(void) const = 0;
		virtual RObject* Clone(void) const = 0;
	};
	
}