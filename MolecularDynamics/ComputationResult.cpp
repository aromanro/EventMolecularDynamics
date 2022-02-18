#include "stdafx.h"
#include "ComputationResult.h"

namespace MolecularDynamics {

	ComputationResult::ComputationResult()
		: nextEventTime(0)
	{
	}

	ComputationResult::ComputationResult(const std::vector<Particle>& part, double ne)
		: particles(part), nextEventTime(ne)
	{
	}


	ComputationResult::~ComputationResult()
	{
	}

}