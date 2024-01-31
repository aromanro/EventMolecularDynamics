#pragma once

#include <queue>

#include "ComputationThread.h"
#include "MolecularDynamics.h"
#include "ComputationResult.h"
#include "MolecularDynamicsThread.h"

class CMolecularDynamicsDoc;

namespace MolecularDynamics {

	class StatisticsThread :
		public ComputationThread
	{
	public:
		StatisticsThread();
		~StatisticsThread() override;
		
		void Start() override;

		void SetDocument(CMolecularDynamicsDoc* d)
		{
			doc = d;
			molecularDynamicsThread.SetDocument(doc);
			molecularDynamicsThread.SetStatisticsThread(this);
		}

		int Init();

		// the following two are to be called after init but before having the thread running
		double GetNextEventTime();
		std::vector<Particle> GetParticles() const;

		std::atomic_bool Terminate;

		std::mutex dataSection;
		std::queue<MolecularDynamics::ComputationResult> resultsQueue;

		std::atomic_bool CalculateStatistics;

		std::vector<double> results1;
		std::vector<double> results2;

	private:
		CMolecularDynamicsDoc* doc;

		MolecularDynamics::MolecularDynamicsThread molecularDynamicsThread;

		void Calculate() override;
		bool PostDataToOtherThread();
	};

}

