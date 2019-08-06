#pragma once

namespace Raven
{
	typedef enum ECommandListType
	{
		COMMAND_LIST_TYPE_PRIMARY,
		COMMAND_LIST_TYPE_SECONDARY,
		MAX_COMMAND_LIST
	} ECommandListType;

	/*! \struct ICommandList
	\brief Interface for a command list. */
	struct ICommandList {};
}