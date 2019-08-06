#pragma once
#include "core/object/object.h"

namespace Raven 
{
	namespace Meta 
	{
		// Properties that extend from this class are automatically registered for reflection generation.
		class IMetaProperty : public RObject {};
	}
}

// Meta definitions for intellisense.
#if (!defined(__REFLECTION_PARSER__) && defined(__INTELLISENSE__))

/*!\class Enable
\brief Native meta property to explicitly enable reflection generation for this type.
*/
class Enable : public Raven::Meta::IMetaProperty { };

/*!\class Disable
\brief Native meta property to explicitly disable reflection generation for this type.
*/
class Disable : public Raven::Meta::IMetaProperty { };

#endif
