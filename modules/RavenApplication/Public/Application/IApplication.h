#pragma once
#include "Platform/Interfaces/IPlatformWindow.h"
#include "Platform/Interfaces/IWindowMessageHandler.h"

#include <vector>

namespace Raven {
	/*!
	\class IApplication
	\brief Main interface for implementing applications.
	*/
	class IApplication {
	protected:
		IApplication() = default;
		IApplication(const IApplication&) = delete; /*!< Delete copy constuctor */
		IApplication& operator= (const IApplication&) = delete; /*!< Delete assignment constuctor */

	public:
		virtual void OnStart(int argc, char *argv[]) = 0;
		virtual void OnDestroy() = 0;
		virtual void OnUpdate() = 0;
	};
}


#define APPLICATION_START(appClass)		\
int main(int argc, char *argv[])		\
{										\
	appClass application;				\
										\
return 0;								\
}