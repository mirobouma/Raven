#pragma once

namespace Raven
{
	namespace Meta
	{
		template<typename EnumType>
		REnumContainer<EnumType>::REnumContainer(
			const std::string &name,
			const Initializer &initializer
		)
			: EnumBase(name, InvalidTypeID)
		{
			for (auto &pair : initializer)
			{
				m_keys.emplace_back(pair.first);

				m_keyToValue.emplace(pair);
			}
		}

		template<typename EnumType>
		REnumContainer<EnumType>::REnumContainer(
			const std::string &name,
			const Initializer &initializer,
			FTypeID owner
		)
			: EnumBase(name, owner)
		{
			for (auto &pair : initializer)
			{
				_keys.emplace_back(pair.first);

				_keyToValue.emplace(pair);
			}
		}

		template<typename EnumType>
		RType REnumContainer<EnumType>::GetType(void) const
		{
			return typeof(EnumType);
		}

		template<typename EnumType>
		RType REnumContainer<EnumType>::GetUnderlyingType(void) const
		{
			return typeof(typename std::underlying_type<EnumType>::type);
		}

		template<typename EnumType>
		const std::vector<std::string> &REnumContainer<EnumType>::GetKeys(void) const
		{
			return m_keys;
		}

		template<typename EnumType>
		std::vector<RVariant> REnumContainer<EnumType>::GetValues(void) const
		{
			std::vector<RVariant> values;

			for (auto &entry : _keyToValue)
				values.emplace_back(entry.second);

			return values;
		}


		template<typename EnumType>
		std::string REnumContainer<EnumType>::GetKey(const RArgument &value) const
		{
			typedef
				typename std::underlying_type< EnumType >::type UnderlyingType;

			auto type = value.GetType();

			// invalid type
			if (type != typeof(EnumType) &&
				type != typeof(UnderlyingType))
			{
				return std::string();
			}

			auto &converted = value.GetValue<EnumType>();

			for (auto &entry : _keyToValue)
			{
				if (entry.second == converted)
					return entry.first;
			}

			return std::string();
		}

		template<typename EnumType>
		RVariant REnumContainer<EnumType>::GetValue(const std::string &key) const
		{
			auto search = _keyToValue.find(key);

			// not found
			if (search == _keyToValue.end())
				return RVariant();

			return RVariant(search->second);
		}
	}
}
