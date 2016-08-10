#pragma once

#include <atomic>
#include <mutex>

#include "ComputationThread.h"
#include "Simulation.h"



class CMolecularDynamicsDoc;

class MolecularDynamicsThread :
	public ComputationThread
{
public:
	MolecularDynamicsThread();
	virtual ~MolecularDynamicsThread();

	CMolecularDynamicsDoc *doc;

	std::atomic_bool Terminate;
protected:
	Simulation simulation;

	std::mutex m;
	std::condition_variable cv;

	bool wakeup;
	bool processed;


	virtual void Calculate();
	bool PostDataToMainThread();
public:
	void WakeUp();
	void WaitForData();
	int Init();
	double GetNextEventTime() const;
	std::vector<Particle> GetParticles() const;
	void WaitForWork();
	void SignalMoreData();
};

