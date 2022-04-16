#include "stdafx.h"

#include "Options.h"
#include "MolecularDynamicsDoc.h"
#include "ComputationResult.h"

namespace MolecularDynamics {

	StatisticsThread::StatisticsThread()
		: Terminate(false), doc(nullptr)
	{
	}


	StatisticsThread::~StatisticsThread()
	{
		molecularDynamicsThread.Terminate = true;
		Terminate = true;
		molecularDynamicsThread.WakeUp();
		molecularDynamicsThread.join();
		join();
	}

	void StatisticsThread::Start()
	{
		molecularDynamicsThread.Start();
		ComputationThread::Start();
	}


	void StatisticsThread::Calculate()
	{
		bool needsData;

		while (!Terminate)
		{
			{
				std::lock_guard<std::mutex> lock(dataSection);
				needsData = resultsQueue.empty();

				if (!needsData)
				{
					// from time to time, compute statistics
					// for example, after 100 collision results or so (maybe depending on the number of particles)
				}
			}

			if (needsData) {
				molecularDynamicsThread.WakeUp();
				molecularDynamicsThread.WaitForData();
			}

			needsData = PostDataToOtherThread();
			if (!needsData) WaitForWork();
		}
	}


	bool StatisticsThread::PostDataToOtherThread()
	{
		if (!doc) return false;
		else if (doc->resultsQueue.size() > 10) return false; //queue full
		

		{
			std::unique_lock<std::mutex> lock(doc->dataSection);

			if (resultsQueue.empty()) return false;

			// now copy data
			ComputationResult result = std::move(resultsQueue.front());
			resultsQueue.pop();

			lock.unlock();

			doc->resultsQueue.emplace(std::move(result));
		}

		// let the main thread know that new processed data is available
		SignalMoreData();

		molecularDynamicsThread.WakeUp();

		return true;
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

