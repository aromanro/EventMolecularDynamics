#pragma once

#include <vector>

#include "Particle.h"

namespace MolecularDynamics {


	class ComputationResult
	{
	public:
		ComputationResult();
		ComputationResult(const std::vector<Particle>& part, double ne);
		~ComputationResult();

		ComputationResult(const ComputationResult& other)
			: particles(other.particles),
			nextEventTime(other.nextEventTime)
		{					
		}

		ComputationResult(ComputationResult&& other) noexcept
			: nextEventTime(other.nextEventTime)
		{
			particles.swap(other.particles);
		}

		ComputationResult& operator=(const ComputationResult& other)
		{
			if (this != &other) 
			{ 
				particles = other.particles;
				nextEventTime = other.nextEventTime;
			}

			return *this;
		}

		ComputationResult& operator=(ComputationResult&& other)
		{
			if (this != &other) 
			{
				particles.swap(other.particles);
				nextEventTime = other.nextEventTime;
			}
			return *this;
		}

		std::vector<Particle> particles;

		double nextEventTime;
	};

}