#pragma once
#include "Service.h"

namespace Raven {

	/*!
	\class RServiceProvider
	\brief Globally accessible class which allows for retrieval of game services.
	*/
	class RServiceProvider {
	public:
		friend class REngine;
	protected:
		RServiceProvider() = default;
	};

	extern RServiceProvider* GServiceProvider; //*!< Globablly accessible service provider.
}