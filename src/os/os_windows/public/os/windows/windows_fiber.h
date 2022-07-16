class WindowsFiber;

struct FWinFiberInitArgs {
	void* mArgs;
	WindowsFiber* mFiber;
	NativeFiberStart mStart;
};

/*! \class RPlatformWindowsFiber
\brief Interface for platform implementation of a fiber.
*/
class WindowsFiber : public INativeFiber
{
public:
	WindowsFiber() : mFiber(nullptr) {}
	WindowsFiber(std::size_t stackSize, NativeFiberStart startRoutine, void* arg)
	{
		mFiber = nullptr;
		mArgs.mArgs = arg;
		mArgs.mFiber = this;
		mArgs.mStart = startRoutine;
		mStackSize = stackSize;

		if (!startRoutine)
			return;

		mFiber = CreateFiber(stackSize, InitFiber, &mArgs);
	}

	/*! Delete copy constructor because it doesn't make sense to copy a fiber. */
	WindowsFiber(const WindowsFiber& other) = delete;

	/*! Switch to fiber. */
	virtual void SwitchToFiber(INativeFiber* fiber)
	{
		if (mFiber == 0)
		{
			mFiber = ConvertThreadToFiber(0);
		}

		::SwitchToFiber(((WindowsFiber*)fiber)->mFiber);
	}

	/*! Initialize the fiber and start. */
	static void _stdcall InitFiber(void* arg)
	{
		FWinFiberInitArgs* args = (FWinFiberInitArgs*)arg;
		args->mStart(args->mArgs);
	}

	virtual void Reset(NativeFiberStart startRoutine, void* arg)
	{
		mArgs.mArgs = arg;
		mArgs.mStart = startRoutine;
		mFiber = CreateFiber(mStackSize, InitFiber, &mArgs);
	}

protected:
	void* mFiber;
	FWinFiberInitArgs mArgs;
	size_t mStackSize;
};

typedef WindowsFiber RPlatformFiber;