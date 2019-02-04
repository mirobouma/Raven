#pragma once

#include "InvokableConfig.h"
#include "ArgumentConfig.h"

#include <vector>

namespace Raven
{
	namespace Meta
	{
		class RType;
		class RInvokable
		{
		public:
			RInvokable(const std::string &name = "INVALID");

			template<typename ...Types>
			static RInvokableSignature CreateSignature(void);

			const RInvokableSignature &GetSignature(void) const;

			const std::string &GetName(void) const;

		protected:
			std::string _name;
			RInvokableSignature _signature;
		};
	}
}

#include "Invokable.hpp"