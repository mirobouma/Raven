#pragma once
#include <ctime>
#include <chrono>

namespace Raven
{ 
	class RTimer 
	{
	public:
		RTimer() { mOld = mTimer.now(); mNew = mTimer.now(); mStart = mTimer.now(); mSecondCounter = 0.0f; }
		void Tick() {
			mOld = mNew;
			mNew = mTimer.now();
			mFrameCount++;
			mSecondCounter += GetDelta();

			if (mSecondCounter >= 1.0f)
			{
				mFramesPerSecond = mFrameCount;
				mFrameCount = 0;
				mSecondCounter = fmod(mSecondCounter, 1.0f);
			}
		}
		float GetTimeSinceStart() {
			using s = std::chrono::duration<float>;
			return std::chrono::duration_cast<s>(mTimer.now() - mStart).count();
		}
		float GetDelta() {
			using s = std::chrono::duration<float>;
			return std::chrono::duration_cast<s>(mNew - mOld).count();
		}
		void Reset() {
			mOld = mTimer.now();
			mNew = mTimer.now();
		}
		uint32_t GetFramesPerSecond() {
			return mFramesPerSecond;
		}

	private:
		std::chrono::high_resolution_clock mTimer;
		std::chrono::high_resolution_clock::time_point mOld;
		std::chrono::high_resolution_clock::time_point mNew;
		std::chrono::high_resolution_clock::time_point mStart;
		float mSecondCounter;
		uint32_t mFrameCount;
		uint32_t mFramesPerSecond;
	};
}
