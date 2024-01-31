#pragma once

#include <thread>
#include <atomic>
#include <mutex>

class ComputationThread
{
public:
	ComputationThread();

	virtual void Start();

	void join();

	void WakeUp();
	void WaitForData();
	void WaitForWork();
	void SignalMoreData();

protected:
	virtual ~ComputationThread();

private:
	virtual void Calculate() = 0;

	std::thread mThread;

	std::mutex mw;
	std::mutex mp;
	std::condition_variable cvw;
	std::condition_variable cvp;

	bool wakeup;
	bool processed;
};

