#pragma once

#include "Core/Reflection/Type/Type.h"
#include "MetaProperty.h"
#include <map>

namespace Raven
{
	namespace Meta
	{
		class RVariant;

		/*!\class RMetaData
		\brief Structure holding metadata properties defined for reflected types.*/
		class RMetaData
		{
		public:
			typedef std::initializer_list<
				std::pair<RType, const IMetaProperty *>
			> Initializer;

			typedef std::vector<RVariant> PropertyList;

			// Constructors
			RMetaData();
			RMetaData(const RMetaData &rhs);
			RMetaData(const RMetaData &&rhs);
			RMetaData(const Initializer &properties);
			~RMetaData();

			const RMetaData &operator=(const RMetaData &rhs);

			// Get property variant by type ID.
			RVariant GetProperty(FTypeID typeId) const;

			// Get property by type.
			template<typename T>
			const T *GetProperty() const;

			// Gets all properties for this type.
			std::vector<RVariant> GetProperties(void) const;

		protected:
			// Set a property of this type to a value.
			void SetProperty(FTypeID type, const IMetaProperty* value);

		private:
			// Copy this metadata into another metadata object.
			void Copy(const RMetaData &rhs);

			// List of properties for the specified type.
			std::map<FTypeID, const IMetaProperty*> _properties;
		};
	}
}

#include "Metadata.hpp"