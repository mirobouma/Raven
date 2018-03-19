#pragma once
/*! \file Enums.h
\brief Collection of enumerations used in platform implementations.
*/

#include "Core/Utility/Enum.h"

namespace Raven 
{
	/*!
	\enum EPlatformType
	\brief enumerator used to determine which platform we're running on.
	*/
	enum class EPlatformType {
		EPlatformType_Undefined = 0, /*!< Undefined platform. */
		EPlatformType_Windows, /*!< Windows based platform. */
		EPlatformType_Linux, /*!< Linux based platform. */
	};

	/*!
	Enumerator to define flags for window creation.
	*/
	enum EPlatformWindowFlags
	{
		EPlatformWindowFlags_None = 0, /*!< No special window creation flags specified. */
		EPlatformWindowFlags_Borderless = 1 << 0, /*!< Create the window without borders. */
		EPlatformWindowFlags_NoResize = 1 << 1, /*!< Do not allow the window to be resized. */
		EPlatformWindowFlags_Shown = 1 << 2, /*!< Show the window on creation by default. */
		EPlatformWindowFlags_AlwaysOnTop = 1 << 3, /*!< The window should always be on top. */
		EPlatformWindowFlags_AutoPosition = 1 << 4, /*!< Automatically choose location of the window. */
	};
	DECL_ENUM_AS_FLAGS(EPlatformWindowFlags);

	typedef void* RWindowHandle; /*!< Represents handle to a platform window. */
}

