#include "Core/Reflection/Metadata/Metadata.h"
#include "Core/Reflection/Type/Type.h"
#include "Core/Reflection/Variant/Variant.h"
#include <algorithm>
#include "Core/Debug/LogManager.h"

namespace Raven
{
	namespace Meta
	{
		RMetaData::RMetaData(void) { }

		RMetaData::RMetaData(const RMetaData &rhs)
		{
			Copy(rhs);
		}

		RMetaData::RMetaData(const RMetaData &&rhs)
			: _properties(std::move(rhs._properties))
		{

		}

		const RMetaData &RMetaData::operator=(const RMetaData &rhs)
		{
			Copy(rhs);

			return *this;
		}

		///////////////////////////////////////////////////////////////////////

		RMetaData::RMetaData(const Initializer &properties)
		{
			for (auto &prop : properties)
			{
				if (!prop.first.IsValid())
				{
					RLOG_ERROR(,
						"Invalid meta property registered."
						"Make sure the property type is enabled in reflection."
					);
				}

				SetProperty(prop.first, prop.second);
			}
		}

		RMetaData::~RMetaData(void)
		{
			for (auto &prop : _properties)
				delete prop.second;
		}

		///////////////////////////////////////////////////////////////////////

		RVariant RMetaData::GetProperty(FTypeID type) const
		{
			auto search = _properties.find(type);

			if (search == _properties.end())
				return { };

			return ObjectVariant(search->second);
		}

		///////////////////////////////////////////////////////////////////////

		void RMetaData::SetProperty(FTypeID type, const IMetaProperty *prop)
		{
			auto search = _properties.find(type);

			// delete old property if it exists
			if (search != _properties.end())
				delete search->second;

			_properties[type] = prop;
		}

		RMetaData::PropertyList RMetaData::GetProperties(void) const
		{
			PropertyList properties;

			for (auto &property : _properties)
				properties.emplace_back(ObjectVariant(property.second));

			return properties;
		}

	
		void RMetaData::Copy(const RMetaData &rhs)
		{
			for (auto &prop : rhs._properties)
			{
				_properties[prop.first] =
					static_cast<IMetaProperty*>(prop.second->Clone());
			}
		}
	}
}
