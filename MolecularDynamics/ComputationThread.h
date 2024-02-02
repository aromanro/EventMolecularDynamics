#pragma once

#include <thread>
#include <atomic>
#include <mutex>

class ComputationThread
{
public:
	virtual void Start();

	void join();

	void WakeUp();
	void WaitForData();
	void WaitForWork();
	void SignalMoreData();

protected:
	virtual ~ComputationThread() = default;

private:
	virtual void Calculate() = 0;

	std::thread mThread;

	std::mutex mw;
	std::mutex mp;
	std::condition_variable cvw;
	std::condition_variable cvp;

	bool wakeup = false;
	bool processed = false;
};

