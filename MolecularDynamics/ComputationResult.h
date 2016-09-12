#pragma once

#include <vector>

#include "Particle.h"

namespace MolecularDynamics {


	class ComputationResult
	{
	public:
		ComputationResult();
		~ComputationResult();

		std::vector<Particle> particles;

		double nextEventTime;
	};

}