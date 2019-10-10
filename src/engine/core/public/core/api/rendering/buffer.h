#pragma once
#include "core/api/rendering/defines.h"
#include "util/enum_as_flags.h"

typedef enum BufferUsageFlags
{
    BUFFER_USAGE_TRANSFER_SRC = 1 << 0,
    BUFFER_USAGE_TRANSFER_DST = 1 << 1,
    BUFFER_USAGE_UNIFORM_TEXEL_BUFFER = 1 << 2,
    BUFFER_USAGE_STORAGE_TEXEL_BUFFER = 1 << 3,
    BUFFER_USAGE_UNIFORM_BUFFER = 1 << 4,
    BUFFER_USAGE_STORAGE_BUFFER = 1 << 5,
    BUFFER_USAGE_INDEX_BUFFER = 1 << 6,
    BUFFER_USAGE_VERTEX_BUFFER = 1 << 7,
    BUFFER_USAGE_INDIRECT_BUFFER = 1 << 8,
    MAX_BUFFER_TARGET_USAGE = 9
} BufferUsageFlags;
DECL_ENUM_AS_FLAGS(BufferUsageFlags)

struct BufferCreateDesc
{
    MemoryType				memoryType;
    BufferUsageFlags		usage;
    size_t					size;
    void*                   pData;
    size_t					dataSize;
};

struct IBuffer
{
    virtual size_t get_size() = 0;
};

struct DynamicUniformAllocation
{
    IBuffer* buffer;
    size_t offset;
    size_t range;
};
