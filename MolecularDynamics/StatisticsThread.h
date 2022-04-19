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
		virtual ~StatisticsThread();
		
		void Start() override;

		std::atomic_bool Terminate;

		std::mutex dataSection;
		std::queue<MolecularDynamics::ComputationResult> resultsQueue;

		std::atomic_bool CalculateStatistics;

		std::vector<unsigned int> results1;
		std::vector<unsigned int> results2;

	protected:
		CMolecularDynamicsDoc* doc;

		MolecularDynamics::MolecularDynamicsThread molecularDynamicsThread;

		virtual void Calculate();
		bool PostDataToOtherThread();
	public:
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
	};

}

