#pragma once
#include "MetaData.h"

namespace Raven
{
	namespace Meta
	{
		/*! Interface type for types which hold meta data. */
		class IMetaType
		{
		public:
			/*! Retrieve constant reference to the meta data object for this type. */
			const RMetaData& GetMeta()
			{
				return _meta;
			}

		protected:
			RMetaData _meta;
		};
	}
}