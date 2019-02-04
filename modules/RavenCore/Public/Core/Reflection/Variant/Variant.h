#pragma once
#include <functional>
#include <memory>
#include "VariantBase.h"
#include "VariantPolicy.h"
#include "VariantContainer.h"
#include "Core/Containers/Array.h"
#include "Core/Object/Object.h"

#define DISABLE_VARIANT                  \
    typename std::enable_if<             \
        !std::is_same<Raven::Meta::RVariant, T>::value \
    >::type*                             \

#define DISABLE_VARIANT_DECL DISABLE_VARIANT = nullptr

#define DISABLE_ARGUMENT                  \
    typename std::enable_if<              \
        !std::is_same<Raven::Meta::RArgument, T>::value \
    >::type*                              \

#define DISABLE_ARGUMENT_DECL DISABLE_ARGUMENT = nullptr

#define DISABLE_CONST            \
    typename std::enable_if<     \
        !std::is_const<T>::value \
    >::type*                     \

#define DISABLE_CONST_DECL DISABLE_CONST = nullptr

namespace Raven
{
	class RObject;

	namespace Meta
	{
		class RArgument;
		class RVariant
		{
		public:
			RVariant(void);

			template<typename T>
			RVariant(
				T *data,
				Variant_Policy::WrapObject,
				typename std::enable_if<
				std::is_base_of<RObject, T>::value
				>::type* = nullptr
			);

			template<typename T>
			RVariant(
				T &data,
				DISABLE_VARIANT_DECL
			);

			template<typename T>
			RVariant(
				T &data,
				Variant_Policy::NoCopy,
				DISABLE_VARIANT_DECL
			);

			template<typename T>
			RVariant(T &&data,
				DISABLE_VARIANT_DECL,
				DISABLE_ARGUMENT_DECL,
				DISABLE_CONST_DECL
			);

			// array types (non-const)
			template<typename T>
			RVariant(TArray<T> &rhs);

			// array types (const)
			template<typename T>
			RVariant(const TArray<T> &rhs);

			// r-value array types (non-const)
			template<typename T>
			RVariant(TArray<T> &&rhs);

			// r-value array types (const)
			template<typename T>
			RVariant(const TArray<T> &&rhs);

			// copy constructor
			RVariant(const RVariant &rhs);

			// r-value
			RVariant(RVariant &&rhs);
			~RVariant(void);

			template<typename T>
			RVariant &operator=(T &&rhs);
			RVariant &operator=(const RVariant &rhs);
			RVariant &operator=(RVariant &&rhs);

			operator bool(void) const;

			RType GetType(void) const;
			void Swap(RVariant &other);

			int ToInt(void) const;
			bool ToBool(void) const;
			float ToFloat(void) const;
			double ToDouble(void) const;
			std::string ToString(void) const;

			template<typename T>
			T &GetValue(void) const;
			RArrayWrapper GetArray(void) const;


			bool IsValid(void) const;
			bool IsConst(void) const;
			bool IsArray(void) const;

		private:
			friend class RType;
			friend class RArgument;
			friend class RDestructor;

			void *GetPtr(void) const;

			bool _isConst;
			RVariantBase* _data;
		};
	}
}

#include "Variant.hpp"

#undef DISABLE_VARIANT
#undef DISABLE_VARIANT_DECL
#undef DISABLE_ARGUMENT
#undef DISABLE_ARGUMENT_DECL
#undef DISABLE_CONST
#undef DISABLE_CONST_DECL