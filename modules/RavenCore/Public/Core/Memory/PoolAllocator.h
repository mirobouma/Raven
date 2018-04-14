#pragma once
#include <cinttypes>
#include <vector>
#include <memory>
#include "Core/Debug/LogManager.h"
#include <algorithm>

namespace Raven
{

    /*! \struct FPoolAllocation
    \brief Structure describing an allocation in the pool allocator.
    */
    struct FPoolAllocation {
        uint32_t index;
        uint32_t amountOfObjects;
        void* memory;
    };

    /*! \class RPoolAllocator
    \brief Allocator which allocates memory for n amount of objects.
    \sa RBuddyAllocator
    \sa RFreeListAllocator
    \sa RLinearAllocator
    \sa RStackAllocator
    */
    class RPoolAllocator
    {
    public:
        /*! Linear allocator.
        \param numObjects amount of objects to allocate memory for.
        */
        RPoolAllocator(uint32_t numObjects, uint32_t objectSize)
        {
            mCurrIndex = 0;
            mObjectSize = objectSize;
            mMaxObjects = numObjects;

            mAllocation = malloc(numObjects * objectSize);
        }

        ~RPoolAllocator()
        {
            if(mAllocation != nullptr)
            {
                free(mAllocation);
                mAllocation = nullptr;
            }
        }

        /*! Allocates memory from a memory block.
        \param amount Amount of objects to allocate.
        */
        FPoolAllocation Allocate(uint32_t amount)
        {
            if(mAllocation == nullptr)
            {
               mAllocation = malloc(mMaxObjects * mObjectSize);
            }

            if(mCurrIndex + amount >= mMaxObjects)
            {
                RLOG_WARNING("Tried to allocate more memory than available in a pool allocator. Amount: %i", amount);
            }

            FPoolAllocation new_alloc;

            for(FPoolAllocation& alloc : mFreeAllocations)
            {
                if(alloc.amountOfObjects == amount)
                {
                    mFreeAllocations.erase(std::remove(mFreeAllocations.begin(), mFreeAllocations.end(), alloc));
                    return alloc;
                }
                else if(alloc.amountOfObjects > amount)
                {
                    new_alloc.memory = alloc.memory;
                    new_alloc.index = alloc.index;
                    new_alloc.amountOfObjects = amount;

                    alloc.index = new_alloc.index + amount;
                    alloc.amountOfObjects = alloc.amountOfObjects - amount;
                    alloc.memory = (char*)mAllocation + alloc.index;

                    return new_alloc;
                }
            }

            // If we couldn't allocate

            new_alloc.amountOfObjects = amount;
            new_alloc.index = mCurrIndex;
            new_alloc.memory = (char*)mAllocation + mCurrIndex;

            mCurrIndex += amount;

            return new_alloc;
        }

        /*! Free the allocations previously made for reusage */
        void Free(FPoolAllocation alloc)
        {
            mFreeAllocations.push_back(alloc);
        }

        /*! Resets memory allocations for this linear allocator.
        \param freeBlocks If freeBlocks is false, the previously allocated blocks won't be freed and can be reused by this linear allocator.
        */
        void Reset(bool freeMemory = true)
        {
            mFreeAllocations.clear();

            if(freeMemory)
            {
                free(mAllocation);
                mAllocation = nullptr;
            }

            mCurrIndex = 0;
        }

    protected:
        uint32_t mCurrIndex; /*! Current index in the dynamic allocation for the next allocation. */
        uint32_t mMaxObjects; /*! Max amount of objects this pool allocator can hold. */
        uint32_t mObjectSize; /*! Size of the object type for this pool allocator. */

        /*! Vector holding allocations that have been freed. */
        std::vector<FPoolAllocation> mFreeAllocations;

        void*   mAllocation; /*! Pointer to our allocation. */
    };

}