#include "stdafx.h"

#include "Options.h"
#include "MolecularDynamicsDoc.h"
#include "ComputationResult.h"

namespace MolecularDynamics {

	StatisticsThread::StatisticsThread()
		: doc(nullptr), Terminate(false),
		wakeup(false), processed(false)
	{
	}


	StatisticsThread::~StatisticsThread()
	{
		molecularDynamicsThread.Terminate = true;
		molecularDynamicsThread.WakeUp();
		molecularDynamicsThread.join();
		Terminate = true;
		join();
	}


	void StatisticsThread::Calculate()
	{
		bool needsData = true;

		while (!Terminate)
		{
			if (needsData) {
				// grab data from molecular dynamics thread, compute statistics, whatever

			}

			needsData = PostDataToOtherThread();

			if (!needsData) WaitForWork();
		}
	}


	bool StatisticsThread::PostDataToOtherThread()
	{
		if (!doc) return false;

		//const double nextSimulationTime = simulation.NextEventTime();

		//if (doc->simulationTime >= nextSimulationTime)
		//	return true; // don't bother adding this, it needs data for the future already

		if (doc->resultsQueue.size() > 10) return false; //queue full

		{
			std::unique_lock<std::mutex> lock(doc->dataSection);

			//if (doc->resultsQueue.size() && doc->resultsQueue.back().nextEventTime == nextSimulationTime)
			//	return true;

			// now copy data
			//ComputationResult result(simulation.particles, nextSimulationTime);

			lock.unlock();

			//doc->resultsQueue.emplace(std::move(result));
		}

		// let the main thread know that new processed data is available
		SignalMoreData();

		return true;
	}


	void StatisticsThread::WakeUp()
	{
		// signal the worker thread to wake up if it's waiting
		{
			std::lock_guard<std::mutex> lk(mw);
			wakeup = true;
		}
		cvw.notify_one();
	}

	void StatisticsThread::SignalMoreData()
	{
		std::unique_lock<std::mutex> lk(mp);
		processed = true;
		lk.unlock();
		cvp.notify_one();
	}

	void StatisticsThread::WaitForData()
	{
		//wait for the worker thread to finish some work
		std::unique_lock<std::mutex> lk(mp);
		cvp.wait(lk, [this] { return processed; });
		processed = false;
	}

	void StatisticsThread::WaitForWork()
	{
		std::unique_lock<std::mutex> lk(mw);
		cvw.wait(lk, [this] { return wakeup; });
		wakeup = false;
	}


	int StatisticsThread::Init()
	{
		return molecularDynamicsThread.Init();
	}

	double StatisticsThread::GetNextEventTime()
	{
		return molecularDynamicsThread.GetNextEventTime();
	}

	std::vector<Particle> StatisticsThread::GetParticles() const
	{
		return molecularDynamicsThread.GetParticles();
	}

}

