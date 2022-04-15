#pragma once
#include "ComputationThread.h"
#include "MolecularDynamics.h"

class CMolecularDynamicsDoc;

namespace MolecularDynamics {

	class StatisticsThread :
		public ComputationThread
	{
	public:
		StatisticsThread();
		virtual ~StatisticsThread();

		CMolecularDynamicsDoc* doc;

		std::atomic_bool Terminate;
	protected:
		std::mutex mw;
		std::mutex mp;
		std::condition_variable cvw;
		std::condition_variable cvp;

		bool wakeup;
		bool processed;

		MolecularDynamics::MolecularDynamicsThread molecularDynamicsThread;

		virtual void Calculate();
		bool PostDataToOtherThread();
	public:
		void WakeUp();
		void WaitForData();
		int Init();

		// the following two are to be called after init but before having the thread running
		double GetNextEventTime();
		std::vector<Particle> GetParticles() const;

		void WaitForWork();
		void SignalMoreData();
	};

}

