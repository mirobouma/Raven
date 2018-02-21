#pragma once

namespace Raven {
	/** 
	\class IService
	\brief Interface for a service that can be registered to the global service provider. 
	\sa RServiceProvider
	*/
	template <typename T>
	class IService {
	public:
		IService() = default;

		/*! Initialize the system.
		\return true if initialization succeeded.
		*/
		virtual bool Initialize() = 0;

		/*! Destroy the system.
		\return true if initialization succeeded.
		*/
		virtual bool Destroy() = 0;

		/*! Register a service to the service provider.
		\return true if the service has successfully been registered.
		\sa RServiceProvider
		*/
		inline bool RegisterService() const { return false; }

		/*! Deregister a service from the service provider.
		\return true if the service has successfully been unregistered.
		\sa RServiceProvider
		*/
		inline bool DeregisterService() const { return false; }
	};
}
