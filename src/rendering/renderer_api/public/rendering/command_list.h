#pragma once

namespace Raven
{
    typedef enum CommandListType
    {
        COMMAND_LIST_TYPE_PRIMARY,
        COMMAND_LIST_TYPE_SECONDARY,
        MAX_COMMAND_LIST
    } CommandListType;


    struct ICommandList {};
}