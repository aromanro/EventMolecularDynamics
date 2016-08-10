#pragma once

#include <vector>

#include "Particle.h"

class ComputationResult
{
public:
	ComputationResult();
	~ComputationResult();

	std::vector<Particle> particles;

	double nextEventTime;
};

