#pragma once

/*! Queue type for command pools and command queues. */
typedef enum ECommandType {
	COMMAND_TYPE_DIRECT,
	COMMAND_TYPE_BUNDLE,
	COMMAND_TYPE_COMPUTE,
	COMMAND_TYPE_COPY,
	MAX_COMMAND_TYPE
} ECommandType;
