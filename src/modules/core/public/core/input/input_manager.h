#pragma once
#include <memory>
#include <string>
#include <vector>

namespace Raven 
{
	class RInputMapping;

	/*!
	\class RInputManager
	\brief Input manager, only one is allowed to exist per application.
	*/
	class RInputManager 
	{
		friend class RInputMapping;
	private:
		/*! Initialize the input manager.
		\param minSeverity Minimum severity that is logged.
		*/
		RInputManager();

		RInputManager(const RInputManager&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy the input manager instance. */
		RInputManager& operator= (const RInputManager&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite the input manager. */

	public:
		/*! Initialize the global log manager.
		\param minSeverity Minimum severity that is logged.
		*/
		static bool Initialize();

		/*! Destroy the global log manager. */
		static void Destroy();

		/*! Set display size for input. */
		void SetDisplaySize(uint32_t width, uint32_t height);

		/*! Create an input mapping. */
		RInputMapping* CreateMapping();
	protected:
		std::vector <std::unique_ptr<RInputMapping>> _mMappings;
	};

	class RInputMapping
	{
	public:
		RInputMapping(RInputManager* manager) {}
	};

	extern RInputManager* GInputManager; /*!< Global reference to input manager. */


}





