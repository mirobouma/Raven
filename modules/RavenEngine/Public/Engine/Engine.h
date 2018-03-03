#pragma once

namespace Raven {
	/*!
	\class REngine
	\brief Main engine object, only one is allowed to exist per application. 
	Globally accessible through the GEngine variable defined in "Engine/Globals.h"
	*/
	class REngine {
	private:
		REngine() = default;
		REngine(const REngine&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy the engine instance. */
		REngine& operator= (const REngine&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite the engine instance. */

	public:

		/*! Creates engine instances and initializes the minimal engine functionality, then registers it as GEngine. 
		/returns false if initialization failed, true if initialization succeeded.
		*/
		static REngine* Create();

		/*! Initializes the engine if it exists, and sets GEngine to null. 
		/returns false if destroyal failed, true if destroyal succeeded.
		*/
		static bool Destroy();

		/*! Initializes the minimal engine functionality.
		/returns false if initialization failed, true if initialization succeeded.
		*/
		bool InitializeMinimal();

		/*! Initializes the non-critical systems.
		/returns false if initialization failed, true if initialization succeeded.
		*/
		bool InitializeSystems();
		
	};
}