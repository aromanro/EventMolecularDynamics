#include "stdafx.h"
#include "MolecularDynamicsThread.h"

#include "MolecularDynamics.h"
#include "Options.h"
#include "MolecularDynamicsDoc.h"
#include "ComputationResult.h"


namespace MolecularDynamics {


	MolecularDynamicsThread::MolecularDynamicsThread()
		: doc(nullptr), Terminate(false),
		wakeup(false), processed(false)
	{
	}


	MolecularDynamicsThread::~MolecularDynamicsThread()
	{
		join();
	}


	void MolecularDynamicsThread::Calculate()
	{
		bool needsData = true;

		while (!Terminate)
		{
			if (needsData) simulation.Advance();

			needsData = PostDataToMainThread();

			if (!needsData) WaitForWork();
		}
	}


	bool MolecularDynamicsThread::PostDataToMainThread()
	{
		if (!doc) return false;

		double nextSimulationTime = simulation.NextEventTime();

		if (doc->simulationTime >= nextSimulationTime)
			return true; // don't bother adding this, it needs data for the future already

		if (doc->resultsQueue.size() > 10) return false; //queue full

		{
			ComputationResult result;

			std::lock_guard<std::mutex> lock(doc->dataSection);

			if (doc->resultsQueue.size() && doc->resultsQueue.back().nextEventTime == nextSimulationTime)
				return true;

			// now copy data

			result.particles = simulation.particles;
			result.nextEventTime = nextSimulationTime;

			doc->resultsQueue.push(result);
		}

		// let the main thread know that new processed data is available
		SignalMoreData();

		return true;
	}


	void MolecularDynamicsThread::WakeUp()
	{
		// signal the worker thread to wake up if it's waiting
		{
			std::lock_guard<std::mutex> lk(mw);
			wakeup = true;
		}
		cvw.notify_one();
	}

	void MolecularDynamicsThread::SignalMoreData()
	{
		std::unique_lock<std::mutex> lk(mp);
		processed = true;
		lk.unlock();
		cvp.notify_one();
	}

	void MolecularDynamicsThread::WaitForData()
	{
		//wait for the worker thread to finish some work
		std::unique_lock<std::mutex> lk(mp);
		cvp.wait(lk, [this] { return processed; });
		processed = false;
	}

	void MolecularDynamicsThread::WaitForWork()
	{
		std::unique_lock<std::mutex> lk(mw);
		cvw.wait(lk, [this] { return wakeup; });
		wakeup = false;
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


