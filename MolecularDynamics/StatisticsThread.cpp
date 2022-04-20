#include "stdafx.h"

#include "Options.h"
#include "MolecularDynamicsDoc.h"
#include "ComputationResult.h"

namespace MolecularDynamics {

	StatisticsThread::StatisticsThread()
		: Terminate(false), CalculateStatistics(false), doc(nullptr)
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
				std::unique_lock<std::mutex> lock(dataSection);
				needsData = resultsQueue.empty();

				if (!needsData && CalculateStatistics && doc)
				{
					// from time to time, compute statistics
					// for example, after 100 collision results or so (maybe depending on the number of particles)
					// or maybe the main thread shuld ask for them from time to time (as in 5 seconds or so)

					const ComputationResult& res = resultsQueue.front();

					lock.unlock();

					std::unique_lock<std::mutex> dlock(doc->dataSection);
					
					const unsigned int nrBins = doc->options.nrBins;
					const double maxSpeed = doc->options.maxSpeed;
					
					dlock.unlock();

					const double vu = maxSpeed / nrBins;

					std::vector<double> res1(nrBins, 0);
					std::vector<double> res2(nrBins, 0);

					for (const Particle& p : res.particles)
					{
						const double v = p.velocity.Length();
						if (v >= maxSpeed) continue;

						const unsigned int bin = static_cast<unsigned int>(floor(v / vu));
						
						if (abs(p.mass - doc->options.interiorSpheresMass) < 1E-10)
							res1[bin] += 1.;
						else
							res2[bin] += 1.;
					}

					std::transform(res1.begin(), res1.end(), res1.begin(), [&](double v) { return v / doc->nrBigSpheres; });
					std::transform(res2.begin(), res2.end(), res2.begin(), [&](double v) { return v / doc->nrSmallSpheres; });

					results1.swap(res1);
					results2.swap(res2);
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

		{
			std::scoped_lock lock(dataSection, doc->dataSection);

			if (resultsQueue.empty()) return false;
			else if (doc->resultsQueue.size() > 10) return false; //queue full

			// now copy data
			doc->resultsQueue.emplace(std::move(resultsQueue.front()));
			resultsQueue.pop();

			if (!results1.empty() && !results2.empty() && CalculateStatistics.exchange(false))
			{
				doc->results1.swap(results1);
				doc->results2.swap(results2);
			}
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

