#pragma once

#include "core/debug/log_manager.h"
#include "core/reflection/variant/variant.h"
#include "argument_config.h"
#include "argument.h"

namespace Raven
{
	namespace Meta
	{
		template<typename ClassType, bool IsObjectWrapped, typename ...ArgTypes>
		class RConstructorInvoker<ClassType, false, IsObjectWrapped, ArgTypes...> : public RConstructorInvokerBase
		{
		public:
			static_assert(THIS_ARG_COUNT <= MaxArgumentCount,
				"Constructor has too many arguments. It's time to generate more overloads."
				);

			RVariant Invoke(const ArgumentList &arguments) override
			{
				RASSERT_MSG(arguments.size() == THIS_ARG_COUNT,
					"Invalid constructor arguments.\nExpected %i args but got %i.",
					THIS_ARG_COUNT,
					arguments.size()
				);

				return invoke<void, ArgTypes...>(arguments);
			}

		private:
			template<typename _>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ClassType();
			}

			template<typename _, typename A1>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ClassType(
					arguments[0].GetValue<A1>()
				);
			}

			template<typename _, typename A1, typename A2>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>()
				);
			}

			template<typename _, typename A1, typename A2, typename A3>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>()
				);
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>()
				);
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>()
				);
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>(),
					arguments[5].GetValue<A6>()
				);
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>(),
					arguments[5].GetValue<A6>(),
					arguments[6].GetValue<A7>()
				);
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>(),
					arguments[5].GetValue<A6>(),
					arguments[6].GetValue<A7>(),
					arguments[7].GetValue<A8>()
				);
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>(),
					arguments[5].GetValue<A6>(),
					arguments[6].GetValue<A7>(),
					arguments[7].GetValue<A8>(),
					arguments[8].GetValue<A9>()
				);
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>(),
					arguments[5].GetValue<A6>(),
					arguments[6].GetValue<A7>(),
					arguments[7].GetValue<A8>(),
					arguments[8].GetValue<A9>(),
					arguments[9].GetValue<A10>()
				);
			}
		};

		template<typename ClassType, typename ...ArgTypes>
		class RConstructorInvoker<ClassType, true, false, ArgTypes...> : public RConstructorInvokerBase
		{
		public:

			static_assert(THIS_ARG_COUNT <= MaxArgumentCount,
				"Constructor has too many arguments. It's time to generate more overloads."
				);

			RVariant Invoke(const ArgumentList &arguments) override
			{
				RASSERT_MSG(arguments.size() == THIS_ARG_COUNT,
					"Invalid constructor arguments.\nExpected %i args but got %i.",
					THIS_ARG_COUNT,
					arguments.size()
				);

				return invoke<void, ArgTypes...>(arguments);
			}

		private:
			template<typename _>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return new ClassType();
			}

			template<typename _, typename A1>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return new ClassType(
					arguments[0].GetValue<A1>()
				);
			}

			template<typename _, typename A1, typename A2>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>()
				);
			}

			template<typename _, typename A1, typename A2, typename A3>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>()
				);
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>()
				);
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>()
				);
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>(),
					arguments[5].GetValue<A6>()
				);
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>(),
					arguments[5].GetValue<A6>(),
					arguments[6].GetValue<A7>()
				);
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>(),
					arguments[5].GetValue<A6>(),
					arguments[6].GetValue<A7>(),
					arguments[7].GetValue<A8>()
				);
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>(),
					arguments[5].GetValue<A6>(),
					arguments[6].GetValue<A7>(),
					arguments[7].GetValue<A8>(),
					arguments[8].GetValue<A9>()
				);
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>(),
					arguments[5].GetValue<A6>(),
					arguments[6].GetValue<A7>(),
					arguments[7].GetValue<A8>(),
					arguments[8].GetValue<A9>(),
					arguments[9].GetValue<A10>()
				);
			}
		};

	
		template<typename ClassType, typename ...ArgTypes>
		class RConstructorInvoker<ClassType, true, true, ArgTypes...> : public RConstructorInvokerBase
		{
		public:
			static_assert(THIS_ARG_COUNT <= MaxArgumentCount,
				"Constructor has too many arguments. It's time to generate more overloads."
				);

			RVariant Invoke(const ArgumentList &arguments) override
			{
				RASSERT_MSG(arguments.size() == THIS_ARG_COUNT,
					"Invalid constructor arguments.\nExpected %i args but got %i.",
					THIS_ARG_COUNT,
					arguments.size()
				);

				return invoke<void, ArgTypes...>(arguments);
			}

		private:
			template<typename _>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ObjectVariant(new ClassType());
			}

			template<typename _, typename A1>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ObjectVariant(new ClassType(
					arguments[0].GetValue<A1>()
				));
			}

			template<typename _, typename A1, typename A2>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ObjectVariant(new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>()
				));
			}

			template<typename _, typename A1, typename A2, typename A3>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ObjectVariant(new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>()
				));
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ObjectVariant(new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>()
				));
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ObjectVariant(new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>()
				));
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ObjectVariant(new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>(),
					arguments[5].GetValue<A6>()
				));
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ObjectVariant(new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>(),
					arguments[5].GetValue<A6>(),
					arguments[6].GetValue<A7>()
				));
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ObjectVariant(new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>(),
					arguments[5].GetValue<A6>(),
					arguments[6].GetValue<A7>(),
					arguments[7].GetValue<A8>()
				));
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ObjectVariant(new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>(),
					arguments[5].GetValue<A6>(),
					arguments[6].GetValue<A7>(),
					arguments[7].GetValue<A8>(),
					arguments[8].GetValue<A9>()
				));
			}

			template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
			RVariant invoke(const ArgumentList &arguments) const
			{
				return ObjectVariant(new ClassType(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>(),
					arguments[5].GetValue<A6>(),
					arguments[6].GetValue<A7>(),
					arguments[7].GetValue<A8>(),
					arguments[8].GetValue<A9>(),
					arguments[9].GetValue<A10>()
				));
			}
		};
	}
}
