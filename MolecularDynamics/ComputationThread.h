#pragma once

#include <thread>
#include <atomic>
#include <mutex>

class ComputationThread
{
public:
	ComputationThread();

	virtual void Start();
protected:
	std::thread mThread;
	virtual ~ComputationThread();

	virtual void Calculate() = 0;

	std::mutex mw;
	std::mutex mp;
	std::condition_variable cvw;
	std::condition_variable cvp;

	bool wakeup;
	bool processed;

public:
	void join();

	void WakeUp();
	void WaitForData();
	void WaitForWork();
	void SignalMoreData();
};

