#pragma once

#include "Core/Reflection/Type/Type.h"

#include <string>
#include <vector>

namespace Raven
{
	namespace Meta
	{
		class RVariant;
		class RArgument;

		class REnumBase
		{
		public:
			REnumBase(const std::string &name, FTypeID owner);

			virtual ~REnumBase(void) { }

			virtual RType GetType(void) const = 0;
			virtual RType GetUnderlyingType(void) const = 0;

			virtual const std::vector<std::string> &GetKeys(void) const = 0;
			virtual std::vector<RVariant> GetValues(void) const = 0;

			virtual std::string GetKey(const RArgument &value) const = 0;
			virtual RVariant GetValue(const std::string &key) const = 0;

			RType GetParentType(void) const;

			const std::string &GetName(void) const;

		private:
			RType			_parentType;
			std::string		_name;
		};
	}
}