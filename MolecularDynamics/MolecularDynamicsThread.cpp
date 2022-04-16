#include "stdafx.h"
#include "MolecularDynamicsThread.h"

#include "MolecularDynamics.h"
#include "Options.h"
#include "MolecularDynamicsDoc.h"
#include "ComputationResult.h"
#include "StatisticsThread.h"


namespace MolecularDynamics {


	MolecularDynamicsThread::MolecularDynamicsThread()
		: Terminate(false), doc(nullptr), statisticsThread(nullptr)
	{
	}


	MolecularDynamicsThread::~MolecularDynamicsThread()
	{
		Terminate = true; // should be true here anyway, but it doesn't hurt to set it
		join();
	}


	void MolecularDynamicsThread::Calculate()
	{
		bool needsData = true;

		while (!Terminate)
		{
			if (needsData) simulation.Advance();

			needsData = PostDataToOtherThread();

			if (!needsData) WaitForWork();
		}
	}


	bool MolecularDynamicsThread::PostDataToOtherThread()
	{
		if (!doc || !statisticsThread) return false;

		const double nextSimulationTime = simulation.NextEventTime();

		if (doc->simulationTime >= nextSimulationTime)
			return true; // don't bother adding this, it needs data for the future already

		if (statisticsThread->resultsQueue.size() > 10) return false; //queue full

		{
			std::unique_lock<std::mutex> lock(statisticsThread->dataSection);

			// now copy data
			ComputationResult result(simulation.particles, nextSimulationTime);
			
			lock.unlock();

			statisticsThread->resultsQueue.emplace(std::move(result));
		}

		// let the main thread know that new processed data is available
		SignalMoreData();

		return true;
	}



	int MolecularDynamicsThread::Init()
	{
		simulation.GenerateRandom(theApp.options.nrSpheres, theApp.options.interiorRadius);
		simulation.BuildEventQueue();

		return simulation.count();
	}


	double MolecularDynamicsThread::GetNextEventTime()
	{
		return simulation.NextEventTime();
	}

	std::vector<Particle> MolecularDynamicsThread::GetParticles() const
	{
		return simulation.particles;
	}

}


