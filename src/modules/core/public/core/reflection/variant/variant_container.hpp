#pragma once
#include <string>

#define DEFAULT_TYPE_HANDLER_IMPL(typeName)                                                      \
    template<typename T>                                                                         \
    template<typename U>                                                                         \
    typeName RVariantContainer<T>::get##typeName(                                                 \
        typename std::enable_if<                                                                 \
            !std::is_convertible<typename Raven::Meta::TypeOrEnumType<U>::type, typeName>::value \
        >::type*                                                                                 \
        ) const                                                                                  \
    {                                                                                            \
        return typeName( );                                                                      \
    }                                                                                            \
    template<typename T>                                                                         \
    template<typename U>                                                                         \
    typeName RVariantContainer<T>::get##typeName(                                                 \
        typename std::enable_if<                                                                 \
            std::is_convertible<typename Raven::Meta::TypeOrEnumType<U>::type, typeName>::value  \
        >::type*                                                                                 \
    ) const                                                                                      \
    {                                                                                            \
        return static_cast<typeName>( _value );                                                 \
    }                                                                                            \

#define DECLARE_STANDARD_VARIANT(type)                                     \
    template<>                                                             \
    class RVariantContainer<type> : public RVariantBase                     \
    {                                                                      \
    public:                                                                \
        RVariantContainer(const type &value);                               \
        RVariantContainer(const type &&value);                              \
                                                                           \
        RType GetType(void) const override;                                 \
        void *GetPtr(void) const override;                                 \
                                                                           \
        int ToInt(void) const override;                                    \
        bool ToBool(void) const override;                                  \
        float ToFloat(void) const override;                                \
        double ToDouble(void) const override;                              \
        std::string ToString(void) const override;                         \
                                                                           \
        RVariantBase* Clone(void) const override;                           \
                                                                           \
    private:                                                               \
        friend class RVariant;                                              \
                                                                           \
        RVariantContainer &operator=(const RVariantContainer &rhs) = delete; \
                                                                           \
        type _value;                                                      \
    };                                                                     \


namespace Raven
{
	namespace Meta
	{
		template<typename T>
		RVariantContainer<T>::RVariantContainer(const NonRefType &value)
			: _value(const_cast<NonRefType&>(value))
		{

		}

		template<typename T>
		RVariantContainer<T>::RVariantContainer(const NonRefType &&value)
			: _value(std::move(const_cast<NonRefType&&>(value)))
		{

		}

		template<typename T>
		RType RVariantContainer<T>::GetType(void) const
		{
			return typeof(T);
		}

		template<typename T>
		int RVariantContainer<T>::ToInt(void) const
		{
			return getint();
		}

		template<typename T>
		bool RVariantContainer<T>::ToBool(void) const
		{
			return getbool();
		}

		template<typename T>
		float RVariantContainer<T>::ToFloat(void) const
		{
			return getfloat();
		}

		template<typename T>
		double RVariantContainer<T>::ToDouble(void) const
		{
			return getdouble();
		}

		template<typename T>
		std::string RVariantContainer<T>::ToString(void) const
		{
			return getString();
		}

		template<typename T>
		void *RVariantContainer<T>::GetPtr(void) const
		{
			return const_cast<void*>(
				reinterpret_cast<const void*>(
					std::addressof(_value)
					)
				);
		}

		template<typename T>
		RVariantBase* RVariantContainer<T>::Clone(void) const
		{
			// use the non reference type to ensure a copy is made
			return new RVariantContainer<NonRefType>(_value);
		}

		DEFAULT_TYPE_HANDLER_IMPL(int)
		DEFAULT_TYPE_HANDLER_IMPL(bool)
		DEFAULT_TYPE_HANDLER_IMPL(float)
		DEFAULT_TYPE_HANDLER_IMPL(double)

		template<typename T>
		template<typename U>
		std::string RVariantContainer<T>::getString(
			typename std::enable_if<
			!std::is_arithmetic<U>::value
			>::type*
		) const
		{
			return std::string();
		}

		template<typename T>
		template<typename U>
		std::string RVariantContainer<T>::getString(
			typename std::enable_if<
			std::is_arithmetic<U>::value
			>::type*
		) const
		{
			return std::to_string(_value);
		}


		template<>
		class RVariantContainer<void> : public RVariantBase
		{
		public:
			RVariantContainer(void);

			RType GetType(void) const override;
			void *GetPtr(void) const override;

			virtual int ToInt(void) const override;
			virtual bool ToBool(void) const override;
			virtual float ToFloat(void) const override;
			virtual double ToDouble(void) const override;
			virtual std::string ToString(void) const override;

			RVariantBase *Clone(void) const override;

		private:
			friend class RVariant;

			RVariantContainer &operator=(const RVariantContainer &rhs) = delete;
		};

		DECLARE_STANDARD_VARIANT(int)
		DECLARE_STANDARD_VARIANT(unsigned int)
		DECLARE_STANDARD_VARIANT(bool)
		DECLARE_STANDARD_VARIANT(float)
		DECLARE_STANDARD_VARIANT(double)
		DECLARE_STANDARD_VARIANT(std::string)
	}
}

#undef DECLARE_STANDARD_TYPE
#undef DEFAULT_TYPE_HANDLER_IMPL