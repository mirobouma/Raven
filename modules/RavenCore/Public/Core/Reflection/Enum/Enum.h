#pragma once

#include <string>
#include <vector>

#include "EnumBase.h"

namespace Raven
{
	namespace Meta
	{
		class REnum
		{
		public:
			bool IsValid(void) const;

			operator bool(void) const;

			bool operator ==(const REnum &rhs) const;
			bool operator !=(const REnum &rhs) const;

			std::string GetName(void) const;

			RType GetType(void) const;
			RType GetParentType(void) const;
			RType GetUnderlyingType(void) const;

			std::vector<std::string> GetKeys(void) const;
			std::vector<RVariant> GetValues(void) const;

			std::string GetKey(const RArgument &value) const;
			RVariant GetValue(const std::string &key) const;

		private:
			friend class RTypeData;

			REnum(const REnumBase* base);

			std::shared_ptr<const REnumBase> _base;
		};
	}
}