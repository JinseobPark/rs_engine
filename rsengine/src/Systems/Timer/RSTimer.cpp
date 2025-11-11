#include "pch.h"
#include "RSTimer.h"

namespace RS_Timer
{
    RSTimer* mRSTIMER = nullptr;

    RSTimer::RSTimer()
    {
        RS_ASSERT(mRSTIMER == nullptr, "only one timer");
        mRSTIMER = this;
    }

    RSTimer::~RSTimer() = default;

    void RSTimer::Initialize()
    {
        last_frame = static_cast<float>(glfwGetTime());
    }

    void RSTimer::Update(float)
    {
	    const float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
        total_time += delta_time;
    }

    void RSTimer::Shutdown()
    {
    }
}
