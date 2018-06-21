#pragma once
#include "gainput/gainput.h"
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

		/*! Set display size for input. */
		RInputMapping* CreateMapping();
	

		gainput::DeviceId GetMouseId() const { return _mMouseId; }
		gainput::DeviceId GetKeyboardId() const { return _mKeyboardId; }
		gainput::DeviceId GetPadId() const { return _mPadId; }
		gainput::InputManager* GetInputManager() { return &_mInputManager; }

	protected:
		gainput::InputManager _mInputManager;
		gainput::DeviceId _mMouseId;
		gainput::DeviceId _mKeyboardId;
		gainput::DeviceId _mPadId;

		std::vector <std::unique_ptr<RInputMapping>> _mMappings;
	};

	class RInputMapping
	{
	public:
		RInputMapping(RInputManager* manager) : _mMapping(manager->_mInputManager) {}

		gainput::InputMap* getMap() { return &_mMapping; }

	protected:
		gainput::InputMap _mMapping;
	};

	extern RInputManager* GInputManager; /*!< Global reference to input manager. */


}





