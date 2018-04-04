#pragma once
#include <vector>
#include <cinttypes>
#include <vector>
#include <functional>
#include <future>
#include <atomic>

namespace Raven 
{
	/*! Base class for a coroutine task so we can store non-specialized types in a vector. */
	class FTaskBase
	{
	public:
		virtual ~FTaskBase() {};
		virtual void Execute() {};
		virtual void WaitForTask() {};
		virtual bool IsTaskCompleted() { return false; };
	};

	/*! Helper namespace for storing variadic arguments in a class, from https://stackoverflow.com/questions/16868129/how-to-store-variadic-template-arguments */
	namespace HTaskHelper
	{
		template <int... Is>
		struct index {};

		template <int N, int... Is>
		struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};

		template <int... Is>
		struct gen_seq<0, Is...> : index<Is...> {};
	}


	/*! Wrapper object around a executable coroutine task. */
	template <typename... Ts>
	class FTask : public FTaskBase {
	public:

		template <typename F, typename... Args>
		explicit FTask(F&& func, Args&&... args) :
			_task(std::forward<F>(func)),
			_args(std::forward<Args>(args)...),
			_isCompleted(false)
		{
			_future = _task.get_future();
		}

		virtual void Execute() override
		{
			func(_args);
		}

		virtual void WaitForTask() override
		{
			if (_isCompleted)
				return;

			_future.wait();
			_isCompleted = true;
		}

		virtual bool IsTaskCompleted() override
		{
			if (_isCompleted)
			{
				return true;
			}

			if (_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				_isCompleted = true;
				return true;
			}

			return false;
		}

	protected:

		template <typename... Args, int... Is>
		void func(std::tuple<Args...>& tup, HTaskHelper::index<Is...>)
		{
			_task(std::get<Is>(tup)...);
		}

		template <typename... Args>
		void func(std::tuple<Args...>& tup)
		{
			func(tup, HTaskHelper::gen_seq<sizeof...(Args)>{});
		}

		// Stored packaged task

		bool _isCompleted;
		std::packaged_task<void(Ts...)> _task;
		std::tuple<Ts...> _args;
		std::future<void> _future;

	};
}
