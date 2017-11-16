#include "stdafx.h"
#include "Simulation.h"

#include "Constants.h"
#include "Options.h"
#include "MolecularDynamics.h"

#include <random>

namespace MolecularDynamics {


	Simulation::Simulation()
	{
	}


	Simulation::~Simulation()
	{
	}


	void Simulation::GenerateRandom(int numParticles, double sphereRadius)
	{
		std::mt19937 rndEngineX, rndEngineY, rndEngineZ;
		std::random_device rdev{};

		std::mt19937 rndEngineVX, rndEngineVY, rndEngineVZ;

		rndEngineX.seed(rdev());
		rndEngineY.seed(rdev());
		rndEngineZ.seed(rdev());

		rndEngineVX.seed(rdev());
		rndEngineVY.seed(rdev());
		rndEngineVZ.seed(rdev());

		particles.reserve(numParticles);


		double minSphereRadius = std::min<double>(theApp.options.interiorSpheresRadius, theApp.options.exteriorSpheresRadius);

		// now it's a cube, but it does not have to be cubic
		std::uniform_real_distribution<double> rndXYZ{ minSphereRadius, spaceSize - minSphereRadius };

		std::uniform_real_distribution<double> rndV{ -1, 1 };

		Vector3D<double> centerSpace(spaceSize / 2, spaceSize / 2, spaceSize / 2);

		// this is just some limit to prevent running forever
		const unsigned long int maxAttempts = 300000ul * numParticles;
		unsigned long int attempt = 0;

		for (int i = 0; i < numParticles; ++i)
		{
			Particle particle;

			particle.position.X = rndXYZ(rndEngineX);
			particle.position.Y = rndXYZ(rndEngineY);
			particle.position.Z = rndXYZ(rndEngineZ);


			// check if it's inside of sphere, if yes, make it bigger

			if ((particle.position - centerSpace).Length() < sphereRadius)
			{
				particle.mass = theApp.options.interiorSpheresMass;
				particle.radius = theApp.options.interiorSpheresRadius;
			}
			else
			{
				particle.mass = theApp.options.exteriorSpheresMass;
				particle.radius = theApp.options.exteriorSpheresRadius;
			}


			// check to see if it's not overlapping some already existing particle
			// or a part outside the walls - should not happen unless the sphereRadius is set too big

			bool overlap = false;
			for (const auto &part : particles)
			{
				if ((part.position - particle.position).Length() <= part.radius + particle.radius ||
					part.position.X > spaceSize - part.radius || part.position.Y > spaceSize - part.radius || part.position.Z > spaceSize - part.radius ||
					part.position.X < part.radius || part.position.Y < part.radius || part.position.Z < part.radius)
				{
					overlap = true;
					break;
				}
			}

			if (overlap)
			{
				--i;
				++attempt;
				if (attempt > maxAttempts)
				{
					particles.clear();
					break;
				}
				continue;
			}

			// now the velocity

			particle.velocity.X = rndV(rndEngineVX);
			particle.velocity.Y = rndV(rndEngineVY);
			particle.velocity.Z = rndV(rndEngineVZ);

			particle.velocity = particle.velocity.Normalize();

			// now the magnitude, can be a constant or distributed as a gaussian, for now just a single value
			particle.velocity *= theApp.options.initialSpeed;

			particles.push_back(particle);
		}
	}



	void Simulation::BuildEventQueue()
	{
		const int numParticles = static_cast<int>(particles.size());

		const size_t sizeGuess = static_cast<size_t>(1.2 * ceill(pow(numParticles*(numParticles - 1) / 2, 1. / 3.)) + numParticles);
		eventsQueue.reserve(sizeGuess);

		for (int i = 0; i < numParticles; ++i)
		{
			double colTime = particles[i].WallCollisionTime();

			if (std::numeric_limits<double>::infinity() != colTime)
			{
				Event wallEvent;

				wallEvent.type = Event::wallCollision;
				wallEvent.particle1 = i;
				wallEvent.eventTime = colTime;

				eventsQueue.push_back(wallEvent);
			}

			for (int j = i + 1; j < numParticles; ++j)
			{
				colTime = particles[i].CollisionTime(particles[j]);

				if (std::numeric_limits<double>::infinity() != colTime)
				{
					Event collisionEvent;

					collisionEvent.particle1 = i;
					collisionEvent.particle2 = j;
					collisionEvent.eventTime = colTime;

					eventsQueue.push_back(collisionEvent);
				}
			}
		}

		std::make_heap(eventsQueue.begin(), eventsQueue.end());
	}



	void Simulation::Advance()
	{
		RemoveNoEventsFromQueueFront();
		if (eventsQueue.empty()) return; // should not happen, except when there are no particles

		Event nextEvent = GetAndRemoveFirstEventFromQueue();

		const int numParticles = static_cast<int>(particles.size());

		AdjustForNextEvent(nextEvent);
		EraseParticleEventsFromQueue(nextEvent); // also adjusts 'partners'

		AddWallCollisionToQueue(nextEvent.particle1);

		if (Event::EventType::particleCollision == nextEvent.type)
		{
			AddWallCollisionToQueue(nextEvent.particle2);
			AddCollision(nextEvent.particle1, nextEvent.particle2);

			for (int i = 0; i < numParticles; ++i)
			{
				if (i != nextEvent.particle1 && i != nextEvent.particle2)
				{
					AddCollision(nextEvent.particle1, i);
					AddCollision(nextEvent.particle2, i);
				}
			}
		}
		else
		{
			for (int i = 0; i < numParticles; ++i)
			{
				if (i != nextEvent.particle1)
					AddCollision(nextEvent.particle1, i);
			}
		}
	}


}