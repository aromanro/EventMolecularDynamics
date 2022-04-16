#pragma once



#include "ComputationThread.h"
#include "Simulation.h"

class CMolecularDynamicsDoc;


namespace MolecularDynamics {

	class StatisticsThread;

	class MolecularDynamicsThread :
		public ComputationThread
	{
	public:
		MolecularDynamicsThread();
		virtual ~MolecularDynamicsThread();

		std::atomic_bool Terminate;
	protected:
		CMolecularDynamicsDoc* doc;
		StatisticsThread* statisticsThread;

		Simulation simulation;

		std::mutex mw;
		std::mutex mp;
		std::condition_variable cvw;
		std::condition_variable cvp;

		bool wakeup;
		bool processed;


		virtual void Calculate();
		bool PostDataToOtherThread();
	public:
		void SetDocument(CMolecularDynamicsDoc* d) { doc = d; }
		void SetStatisticsThread(StatisticsThread* t) { statisticsThread = t; }

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