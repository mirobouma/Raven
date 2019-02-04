#include "Core/Reflection/Variant/VariantContainer.h"
#include "Core/Reflection/Type/Type.h"
#include <algorithm>

namespace Raven
{
	namespace Meta
	{
		RVariantContainer<void>::RVariantContainer(void) { }

		RType RVariantContainer<void>::GetType(void) const
		{
			return typeof(void);
		}

		void *RVariantContainer<void>::GetPtr(void) const
		{
			return nullptr;
		}

		int RVariantContainer<void>::ToInt(void) const
		{
			return int();
		}

		bool RVariantContainer<void>::ToBool(void) const
		{
			return bool();
		}

		float RVariantContainer<void>::ToFloat(void) const
		{
			return float();
		}

		double RVariantContainer<void>::ToDouble(void) const
		{
			return double();
		}

		std::string RVariantContainer<void>::ToString(void) const
		{
			return std::string();
		}

		RVariantBase *RVariantContainer<void>::Clone(void) const
		{
			return new RVariantContainer<void>();
		}

		RVariantContainer<int>::RVariantContainer(const int &value)
			: _value(value) { }

		RVariantContainer<int>::RVariantContainer(const int &&value)
			: _value(std::move(value)) { }

		RType RVariantContainer<int>::GetType(void) const
		{
			return typeof(int);
		}

		void *RVariantContainer<int>::GetPtr(void) const
		{
			return const_cast<void*>(
				reinterpret_cast<const void*>(
					std::addressof(_value)
					)
				);
		}

		int RVariantContainer<int>::ToInt(void) const
		{
			return _value;
		}

		bool RVariantContainer<int>::ToBool(void) const
		{
			return _value == 0 ? false : true;
		}

		float RVariantContainer<int>::ToFloat(void) const
		{
			return static_cast<float>(_value);
		}

		double RVariantContainer<int>::ToDouble(void) const
		{
			return static_cast<double>(_value);
		}

		std::string RVariantContainer<int>::ToString(void) const
		{
			return std::to_string(_value);
		}

		RVariantBase *RVariantContainer<int>::Clone(void) const
		{
			return new RVariantContainer<int>(_value);
		}

		RVariantContainer<unsigned>::RVariantContainer(const unsigned &value)
			: _value(value) { }


		RVariantContainer<unsigned>::RVariantContainer(const unsigned &&value)
			: _value(std::move(value)) { }

		RType RVariantContainer<unsigned>::GetType(void) const
		{
			return typeof(unsigned);
		}

		void *RVariantContainer<unsigned>::GetPtr(void) const
		{
			return const_cast<void*>(
				reinterpret_cast<const void*>(
					std::addressof(_value)
					)
				);
		}

		int RVariantContainer<unsigned>::ToInt(void) const
		{
			return static_cast<int>(_value);
		}

		bool RVariantContainer<unsigned>::ToBool(void) const
		{
			return _value == 0 ? false : true;
		}

		float RVariantContainer<unsigned>::ToFloat(void) const
		{
			return static_cast<float>(_value);
		}

		double RVariantContainer<unsigned>::ToDouble(void) const
		{
			return static_cast<double>(_value);
		}

		std::string RVariantContainer<unsigned>::ToString(void) const
		{
			return std::to_string(_value);
		}

		RVariantBase *RVariantContainer<unsigned>::Clone(void) const
		{
			return new RVariantContainer<unsigned>(_value);
		}

		RVariantContainer<bool>::RVariantContainer(const bool &value)
			: _value(value) { }

		RVariantContainer<bool>::RVariantContainer(const bool &&value)
			: _value(std::move(value)) { }

		RType RVariantContainer<bool>::GetType(void) const
		{
			return typeof(bool);
		}

		void *RVariantContainer<bool>::GetPtr(void) const
		{
			return const_cast<void*>(
				reinterpret_cast<const void*>(
					std::addressof(_value)
					)
				);
		}

		int RVariantContainer<bool>::ToInt(void) const
		{
			return _value ? 1 : 0;
		}

		bool RVariantContainer<bool>::ToBool(void) const
		{
			return _value;
		}

		float RVariantContainer<bool>::ToFloat(void) const
		{
			return _value ? 1.0f : 0.0f;
		}

		double RVariantContainer<bool>::ToDouble(void) const
		{
			return _value ? 1.0 : 0.0;
		}

		std::string RVariantContainer<bool>::ToString(void) const
		{
			return _value ? "true" : "false";
		}

		RVariantBase *RVariantContainer<bool>::Clone(void) const
		{
			return new RVariantContainer<bool>(_value);
		}

		RVariantContainer<float>::RVariantContainer(const float &value)
			: _value(value) { }

		RVariantContainer<float>::RVariantContainer(const float &&value)
			: _value(std::move(value)) { }

		RType RVariantContainer<float>::GetType(void) const
		{
			return typeof(float);
		}

		void *RVariantContainer<float>::GetPtr(void) const
		{
			return const_cast<void*>(
				reinterpret_cast<const void*>(
					std::addressof(_value)
					)
				);
		}

		int RVariantContainer<float>::ToInt(void) const
		{
			return static_cast<int>(_value);
		}

		bool RVariantContainer<float>::ToBool(void) const
		{
			return _value == 0.0f ? false : true;
		}

		float RVariantContainer<float>::ToFloat(void) const
		{
			return _value;
		}

		double RVariantContainer<float>::ToDouble(void) const
		{
			return static_cast<double>(_value);
		}

		std::string RVariantContainer<float>::ToString(void) const
		{
			return std::to_string(_value);
		}

		RVariantBase *RVariantContainer<float>::Clone(void) const
		{
			return new RVariantContainer<float>(_value);
		}

		RVariantContainer<double>::RVariantContainer(const double &value)
			: _value(value) { }


		RVariantContainer<double>::RVariantContainer(const double &&value)
			: _value(std::move(value)) { }

		RType RVariantContainer<double>::GetType(void) const
		{
			return typeof(double);
		}

		void *RVariantContainer<double>::GetPtr(void) const
		{
			return const_cast<void*>(
				reinterpret_cast<const void*>(
					std::addressof(_value)
					)
				);
		}

		int RVariantContainer<double>::ToInt(void) const
		{
			return static_cast<int>(_value);
		}

		bool RVariantContainer<double>::ToBool(void) const
		{
			return _value == 0.0 ? false : true;
		}

		float RVariantContainer<double>::ToFloat(void) const
		{
			return static_cast<float>(_value);
		}

		double RVariantContainer<double>::ToDouble(void) const
		{
			return _value;
		}

		std::string RVariantContainer<double>::ToString(void) const
		{
			return std::to_string(_value);
		}

		RVariantBase *RVariantContainer<double>::Clone(void) const
		{
			return new RVariantContainer<double>(_value);
		}

		RVariantContainer<std::string>::RVariantContainer(
			const std::string &value
		)
			: _value(value) { }

		RVariantContainer<std::string>::RVariantContainer(
			const std::string &&value
		)
			: _value(std::move(value)) { }

		RType RVariantContainer<std::string>::GetType(void) const
		{
			return typeof(std::string);
		}

		void *RVariantContainer<std::string>::GetPtr(void) const
		{
			return const_cast<void*>(
				reinterpret_cast<const void*>(
					std::addressof(_value)
					)
				);
		}

		int RVariantContainer<std::string>::ToInt(void) const
		{
			return stoi(_value);
		}

		bool RVariantContainer<std::string>::ToBool(void) const
		{
			// 0 -> false
			// 1 -> true
			// "true" -> true (case insensitive)
			// "false" -> false (case insensitive)

			if (_value == "0")
				return false;

			if (_value == "1")
				return true;

			auto copy = _value;

			// convert to lowercase
			transform(copy.begin(), copy.end(), copy.begin(), tolower);

			if (copy == "true")
				return true;

			return false;
		}

		float RVariantContainer<std::string>::ToFloat(void) const
		{
			return stof(_value);
		}

		double RVariantContainer<std::string>::ToDouble(void) const
		{
			return stod(_value);
		}

		std::string RVariantContainer<std::string>::ToString(void) const
		{
			return _value;
		}

		RVariantBase *RVariantContainer<std::string>::Clone(void) const
		{
			return new RVariantContainer<std::string>(_value);
		}

	}
}
