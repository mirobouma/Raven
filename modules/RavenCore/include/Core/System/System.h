#pragma once

namespace Raven {
	/** \class ISystem
	\brief Interface wrapper around any system which requires initialization and destruction.
	*/
	class ISystem {
	public:
		ISystem() = default;

		/*! Initialize the system.
		\return true if initialization succeeded.
		*/
		virtual bool Initialize() = 0;

		/*! Destroy the system.
		\return true if initialization succeeded.
		*/
		virtual bool Destroy() = 0;
	};
}