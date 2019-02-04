#pragma once

#include "Core/Reflection/Type/TypeConfig.h"
#include <vector>

namespace Raven
{
	namespace Meta
	{
		class RType;
		class RVariant;

		class RArgument
		{
		public:
			RArgument(void);
			RArgument(const RArgument &rhs);
			RArgument(RVariant &obj);
			RArgument(const RVariant &obj);

			template<typename T>
			RArgument(const T &data);

			template<typename T>
			RArgument(T &data);

			RArgument &operator=(const RArgument &rhs);

			RType GetType(void) const;
			void *GetPtr(void) const;

			template<typename T>
			T &GetValue(void) const;

		private:
			const FTypeID _typeID;
			const bool _isArray;

			const void *_data;
		};
	}
}

#include "Argument.hpp"