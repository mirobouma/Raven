#pragma once

namespace Raven
{
	namespace Meta
	{
		template<class... Types>
		struct FTypeUnpacker { };

		template<>
		struct FTypeUnpacker<>
		{
			static void Apply(std::vector<RType> &types) { }
		};

		template<class First, class... Types>
		struct FTypeUnpacker<First, Types...>
		{
			static void Apply(std::vector<RType> &types)
			{
				types.emplace_back(typeof(First));

				FTypeUnpacker<Types...>::Apply(types);
			}
		};
	}
}