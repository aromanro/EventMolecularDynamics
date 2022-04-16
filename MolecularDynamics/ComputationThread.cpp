#include "stdafx.h"
#include "ComputationThread.h"

ComputationThread::ComputationThread()
	: wakeup(false), processed(false)
{
}

ComputationThread::~ComputationThread()
{
}


void ComputationThread::Start()
{
	mThread = std::thread([this]() {
		Calculate();		
	});
}

void ComputationThread::join()
{
	if (mThread.joinable()) mThread.join();
}


void ComputationThread::WakeUp()
{
	// signal the worker thread to wake up if it's waiting
	{
		std::lock_guard<std::mutex> lk(mw);
		wakeup = true;
	}
	cvw.notify_one();
}

void ComputationThread::SignalMoreData()
{
	std::unique_lock<std::mutex> lk(mp);
	processed = true;
	lk.unlock();
	cvp.notify_one();
}

void ComputationThread::WaitForData()
{
	//wait for the worker thread to finish some work
	std::unique_lock<std::mutex> lk(mp);
	cvp.wait(lk, [this] { return processed; });
	processed = false;
}

void ComputationThread::WaitForWork()
{
	std::unique_lock<std::mutex> lk(mw);
	cvw.wait(lk, [this] { return wakeup; });
	wakeup = false;
}
