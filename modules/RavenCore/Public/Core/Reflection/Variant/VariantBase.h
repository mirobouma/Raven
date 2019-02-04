#pragma once
#include <string>

namespace Raven
{
	namespace Meta
	{
		class RType;
		class RArrayWrapper;

		class RVariantBase
		{
		public:
			virtual ~RVariantBase(void) { }

			virtual RType GetType(void) const = 0;
			virtual void* GetPtr(void) const = 0;

			virtual int ToInt(void) const = 0;
			virtual bool ToBool(void) const = 0;
			virtual float ToFloat(void) const = 0;
			virtual double ToDouble(void) const = 0;
			virtual std::string ToString(void) const = 0;

			virtual bool IsArray() const;
			virtual RArrayWrapper GetArray() const;

			virtual RVariantBase *Clone(void) const = 0;
		};
	}
}