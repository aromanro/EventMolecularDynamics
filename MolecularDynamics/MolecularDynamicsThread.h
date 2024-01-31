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
		~MolecularDynamicsThread() override;

		void SetDocument(CMolecularDynamicsDoc* d) { doc = d; }
		void SetStatisticsThread(StatisticsThread* t) { statisticsThread = t; }

		int Init();

		// the following two are to be called after init but before having the thread running
		double GetNextEventTime();
		std::vector<Particle> GetParticles() const;

		std::atomic_bool Terminate;

	private:
		CMolecularDynamicsDoc* doc;
		StatisticsThread* statisticsThread;

		Simulation simulation;

		void Calculate() override;
		bool PostDataToOtherThread();
	};

}