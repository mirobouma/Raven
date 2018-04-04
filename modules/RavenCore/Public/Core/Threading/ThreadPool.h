#pragma once

#include "Core/Threading/Thread.h"
#include <memory>
#include <vector>

namespace Raven 
{
	/*! \class RThreadPool
	\brief ThreadPool manager which keeps track of threads.
	*/
	class RThreadPool 
	{
	public:
		RThreadPool(uint32_t numWorkers, std::function<void(RThread*)> func) 
		{
			_mThreads.reserve(numWorkers);

			for (uint32_t i = 0; i < numWorkers; i++)
			{
				RThread* thread = new RThread(func);
				_mThreads.push_back(std::unique_ptr<RThread>(std::move(thread)));
			}
		}

		void Destroy()
		{
			// todo
		}

	protected:
		std::vector<std::unique_ptr<RThread>> _mThreads;
	};
}