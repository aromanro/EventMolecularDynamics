#pragma once

#include <thread>
#include <atomic>
#include <mutex>

class ComputationThread
{
public:
	virtual void Start();
protected:
	std::thread mThread;
	virtual ~ComputationThread();

	virtual void Calculate() = 0;
public:
	void join();
};

