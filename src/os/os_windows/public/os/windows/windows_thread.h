#include "os/api/native_thread.h"
#include "os/os.h"
#include <functional>
#include <process.h>
#include <atomic>

namespace Raven
{
	/*! \class RPlatformWindowsThread
	\brief Windows implementation of a thread.
	*/
	class WindowsThreadHandle;
	class WindowsThread : public INativeThread
	{
	public:
		WindowsThread();
		WindowsThread(const WindowsThread&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy a thread. */
		virtual ~WindowsThread();

		virtual INativeThread* from_current_thread() override;

		static uint64_t get_current_thread_id()
		{
			return WindowsThread().from_current_thread()->get_id();
		}

		virtual uint64_t get_id() override;

		/*! Create the thread. */
		virtual bool create(uint32_t stackSize, NativeThreadStart* start, void* arg, size_t coreAffinity = -1) override;

		/*! Join the thread. */
		virtual void join() override;

		/*! Set thread affinity. */
		virtual void set_affinity(size_t affinity) override;

		virtual bool is_current_thread() const override;

		/*! Get platform-specific handle for this thread. */
		virtual INativeThreadHandle* get_handle() override;

	protected:
		WindowsThreadHandle* mHandle;
	};

	typedef WindowsThread Thread;
}