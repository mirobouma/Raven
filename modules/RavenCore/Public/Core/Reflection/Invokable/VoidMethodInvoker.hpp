#pragma once

namespace Raven
{
	namespace Meta
	{
		template<typename ClassType, typename ...ArgTypes>
		class RMethodInvoker<ClassType, void, ArgTypes...> : public RMethodInvokerBase
		{
		public:
			typedef void (ClassType::*Signature)(ArgTypes...);
			typedef void (ClassType::*ConstSignature)(ArgTypes...) const;

			static_assert(THIS_ARG_COUNT <= MaxArgumentCount,
				"Method has too many arguments. It's time to generate more overloads."
				);

			RMethodInvoker(Signature method)
				: _method(reinterpret_cast<ConstSignature>(method)) { }

			RMethodInvoker(ConstSignature method)
				: _method(method) { }

			RVariant Invoke(RVariant &obj, const ArgumentList &arguments) override
			{
				RASSERT(arguments.size() == THIS_ARG_COUNT,
					"Invalid method arguments.\nExpected %i args but got %i.",
					THIS_ARG_COUNT,
					arguments.size()
				);

				invoke<void, ArgTypes...>(obj, arguments);

				return { };
			}

		private:
			template<typename X>
			void invoke(RVariant &obj, const ArgumentList &arguments)
			{
				auto &instance = obj.GetValue<ClassType>();

				(instance.*_method)();
			}

			template<typename X, typename A1>
			void invoke(RVariant &obj, const ArgumentList &arguments)
			{
				auto &instance = obj.GetValue<ClassType>();

				(instance.*_method)(
					arguments[0].GetValue<A1>()
					);
			}

			template<typename X, typename A1, typename A2>
			void invoke(RVariant &obj, const ArgumentList &arguments)
			{
				auto &instance = obj.GetValue<ClassType>();

				(instance.*_method)(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>()
					);
			}

			template<typename X, typename A1, typename A2, typename A3>
			void invoke(RVariant &obj, const ArgumentList &arguments)
			{
				auto &instance = obj.GetValue<ClassType>();

				(instance.*_method)(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>()
					);
			}

			template<typename X, typename A1, typename A2, typename A3, typename A4>
			void invoke(RVariant &obj, const ArgumentList &arguments)
			{
				auto &instance = obj.GetValue<ClassType>();

				(instance.*_method)(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>()
					);
			}

			template<typename X, typename A1, typename A2, typename A3, typename A4, typename A5>
			void invoke(RVariant &obj, const ArgumentList &arguments)
			{
				auto &instance = obj.GetValue<ClassType>();

				(instance.*_method)(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>()
					);
			}

			template<typename X, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
			void invoke(RVariant &obj, const ArgumentList &arguments)
			{
				auto &instance = obj.GetValue<ClassType>();

				(instance.*_method)(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>(),
					arguments[5].GetValue<A6>()
					);
			}

			template<typename X, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
			void invoke(RVariant &obj, const ArgumentList &arguments)
			{
				auto &instance = obj.GetValue<ClassType>();

				(instance.*_method)(
					arguments[0].GetValue<A1>(),
					arguments[1].GetValue<A2>(),
					arguments[2].GetValue<A3>(),
					arguments[3].GetValue<A4>(),
					arguments[4].GetValue<A5>(),
					arguments[5].GetValue<A6>(),
					arguments[6].GetValue<A7>()
					);
			}

			template<typename X, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
			void invoke(RVariant &obj, const ArgumentList &arguments)
			{
				auto &instance = obj.GetValue<ClassType>();

				(instance.*_method)(
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

			template<typename X, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
			void invoke(RVariant &obj, const ArgumentList &arguments)
			{
				auto &instance = obj.GetValue<ClassType>();

				(instance.*_method)(
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

			template<typename X, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
			void invoke(RVariant &obj, const ArgumentList &arguments)
			{
				auto &instance = obj.GetValue<ClassType>();

				(instance.*_method)(
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

			ConstSignature _method;
		};
	}
}