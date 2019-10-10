#pragma once

#if defined(__APPLE__) || defined(__linux__)
#include <stdlib.h>
#elif defined(_WIN32)
#include <crtdbg.h>
#include <malloc.h>
#else
#include <stdlib.h>
#endif

template<typename T>
inline T memory_align_up_with_mask(T value, size_t mask) {
    return (T)(((size_t)value + mask) & ~mask);
}

template<typename T>
inline T memory_align_down_with_mask(T value, size_t mask) {
    return (T)((size_t)value & ~mask);
}

template<typename T>
inline T memory_align_up(T value, size_t alignment) {
    return memory_align_up_with_mask(value, alignment - 1);
}

template<typename T>
inline T memory_align_down(T value, size_t alignment) {
    return memory_align_down_with_mask(value, alignment - 1);
}

template<typename T>
inline bool memory_is_aligned(T value, size_t alignment) {
    return 0 == ((size_t)value & (alignment - 1));
}

template<typename T>
inline T memory_divide_by(T value, size_t alignment) {
    return (T)((value + alignment - 1) / alignment);
}

constexpr inline uint32_t memory_align_up_to_power_of_two(uint32_t value) {
    value--;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    value++;
    return value;
}

#if defined(__APPLE__) || defined(__linux__)
void* aligned_malloc(size_t size, size_t alignment) {
    void* pointer;
    posix_memalign(&pointer, alignment, size);
    return pointer;
}

#elif defined(_WIN32)
void* aligned_malloc(size_t size, size_t alignment) {
    return _aligned_malloc_dbg(size, alignment, __FILE__, __LINE__); // This is reduced to a call to `_aligned_malloc` when _DEBUG is not defined
}

#else
void* aligned_malloc(size_t size, size_t alignment) {
    void* p1; // original block
    void** p2; // aligned block
    int offset = alignment - 1 + sizeof(void*);
    if ((p1 = (void*)malloc(size + offset)) == NULL)
        return NULL;
    p2 = (void**)(((size_t)(p1)+offset) & ~(alignment - 1));
    p2[-1] = p1;
    return p2;
}

#endif

#if defined(__APPLE__) || defined(__linux__)

void aligned_free(void* pointer) {
    free(pointer);
}

#elif defined(_WIN32)
void aligned_free(void* pointer) {
    _aligned_free_dbg(pointer); // This is reduced to a call to `_aligned_free` when _DEBUG is not defined
}

#else
void aligned_free(void* pointer) {
    free(((void**)pointer)[-1]);
}
#endif