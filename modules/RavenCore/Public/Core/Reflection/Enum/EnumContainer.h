#pragma once

#include "EnumBase.h"
#include "Core/Reflection/Variant/Variant.h"
#include <unordered_map>

namespace Raven
{
	namespace Meta
	{
		template<typename EnumType>
		class REnumContainer : public REnumBase
		{
		public:
			typedef std::initializer_list<std::pair<std::string, EnumType>> Initializer;
			typedef std::unordered_map<std::string, EnumType> Table;

			REnumContainer(const std::string &name, const Initializer &initializer);

			REnumContainer(
				const std::string &name,
				const Initializer &initializer,
				FTypeID owner
			);

			RType GetType(void) const override;
			RType GetUnderlyingType(void) const override;

			const std::vector<std::string> &GetKeys(void) const override;
			std::vector<RVariant> GetValues(void) const override;

			std::string GetKey(const RArgument &value) const override;
			RVariant GetValue(const std::string &key) const override;

		private:
			Table _keyToValue;
			std::vector<std::string> _keys;
		};
	}
}

#include "EnumContainer.hpp"