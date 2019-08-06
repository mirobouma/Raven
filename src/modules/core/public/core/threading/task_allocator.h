#pragma once
#include "hal/threading.h"
#include "core/threading/task.h"
#include "core/memory/linear_allocator.h"

namespace Raven
{
	/*! \class RTaskAllocator
	\brief Allocator for tasks that have to be executed on fibers.
	*/
	class RTaskAllocator {
	public:
		RTaskAllocator(uint32_t blockSize = 0, uint32_t reservedTaskAllocations = 128) : _allocator(blockSize) 
		{ 
			_allocatedTasks.reserve(reservedTaskAllocations);
		}

		/*! Create a task and allocate memory to store it's contents */
		template <typename F, typename... Args>
		FTask<Args...>* MakeTask(F&& f, Args... args)
		{
			//constexpr size_t expectedSize = sizeof(FTask<Args...>);

			//// Allocate memory for the task.
			//FLinearAllocation memory = _allocator.Allocate(expectedSize);

			// Initialize the task in the memory of the task.
			FTask<Args...>* task = new FTask<Args...>(std::forward<F>(f), std::forward<Args>(args)...);
			_allocatedTasks.push_back(task);
			return task;
		}

		/*! Reset the memory allocated for the tasks. */
		void Reset(bool freeMemory = false)
		{ 
			for (uint32_t i = 0; i < _allocatedTasks.size(); i++)
			{
				// call destructor.
				delete _allocatedTasks[i];
			}
			_allocatedTasks.clear();

			//_allocator.Reset(freeMemory);
		}

	protected:
		RLinearAllocator _allocator; /*! memory allocator for the task. */
		std::vector<FTaskBase*> _allocatedTasks; /*! List of allocations made for this allocator so we can call the proper destructor. */
	};
}