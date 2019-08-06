#pragma once
#include <vector>
#include <cinttypes>
#include <vector>
#include <functional>
#include <future>
#include <atomic>

namespace Raven 
{
	class RTaskCounter;
	/*! Base class for a coroutine task so we can store non-specialized types in a vector. */
	class FTaskBase
	{
	public:
		virtual ~FTaskBase() {};
		virtual void Execute() {};

		RTaskCounter* GetCounter() const { return _mCounter; };

	protected:
		friend class RTaskScheduler;
		RTaskCounter* _mCounter;
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
			_args(std::forward<Args>(args)...)
		{
		}

		virtual void Execute() override
		{
			func(_args);
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
		std::packaged_task<void(Ts...)> _task;
		std::tuple<Ts...> _args;
	};

}
