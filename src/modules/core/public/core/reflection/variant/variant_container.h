#pragma once

#include "variant_base.h"
#include "core/reflection/metadata/meta_traits.h"
#include "core/object/object.h"

#define DEFAULT_TYPE_HANDLER(typeName)																			 \
    template<typename U = T>																					 \
    typeName get##typeName(																						 \
        typename std::enable_if<																				 \
            !std::is_convertible<typename Raven::Meta::TypeOrEnumType<U>::type, typeName>::value		 \
        >::type* = nullptr																						 \
    ) const;																									 \
    template<typename U = T>																					 \
    typeName get##typeName(																						 \
        typename std::enable_if<																				 \
            std::is_convertible<typename  Raven::Meta::TypeOrEnumType<U>::type, typeName>::value		 \
        >::type* = nullptr																						 \
    ) const;																									 


namespace Raven
{
	namespace Meta
	{
		template<typename T>
		class RVariantContainer : public RVariantBase
		{
		public:
			typedef typename std::remove_reference<T>::type NonRefType;

			RVariantContainer(const NonRefType& value);
			RVariantContainer(const NonRefType&& value);

			RType GetType(void) const override;
			void *GetPtr(void) const override;

			int ToInt(void) const override;
			bool ToBool(void) const override;
			float ToFloat(void) const override;
			double ToDouble(void) const override;
			std::string ToString(void) const override;
			RVariantBase* Clone(void) const override;

		private:
			friend class RVariant;

			T _value;

			RVariantContainer &operator=(const RVariantContainer &rhs) = delete;

			DEFAULT_TYPE_HANDLER(int)
			DEFAULT_TYPE_HANDLER(bool)
			DEFAULT_TYPE_HANDLER(float)
			DEFAULT_TYPE_HANDLER(double)

			template<typename U = T>
			std::string getString(
				typename std::enable_if<
				!std::is_arithmetic<U>::value
				>::type* = nullptr
			) const;

			template<typename U = T>
			std::string getString(
				typename std::enable_if<
				std::is_arithmetic<U>::value
				>::type* = nullptr
			) const;
		};
	}
}

#undef DEFAULT_TYPE_HANDLER

#include "variant_container.hpp"
