#include "stdafx.h"
#include "Simulation.h"

#include "Constants.h"
#include "Options.h"
#include "MolecularDynamics.h"



namespace MolecularDynamics {


	Simulation::Simulation()
	{
	}


	void Simulation::GenerateRandom(int numParticles, double sphereRadius)
	{
		rndEngine.seed(rdev());

		particles.reserve(numParticles);


		const double minSphereRadius = theApp.GetOptions().rightSideInsteadOfSphere ? std::max<double>(theApp.GetOptions().interiorSpheresRadius, theApp.GetOptions().exteriorSpheresRadius) : std::min<double>(theApp.GetOptions().interiorSpheresRadius, theApp.GetOptions().exteriorSpheresRadius);


		// now it's a cube, but it does not have to be cubic
		std::uniform_real_distribution<double> rndXYZ{ minSphereRadius, spaceSize - minSphereRadius };

		std::uniform_real_distribution<double> rndV{ -1, 1 };

		const Vector3D<double> centerSpace(spaceSize / 2, spaceSize / 2, spaceSize / 2);

		// this is just some limit to prevent running forever
		const unsigned long int maxAttempts = 300000ul * numParticles;
		unsigned long int attempt = 0;

		for (int i = 0; i < numParticles; ++i)
		{
			Particle particle;

			particle.position.X = rndXYZ(rndEngine);
			particle.position.Y = rndXYZ(rndEngine);
			particle.position.Z = rndXYZ(rndEngine);

			InitParticle(particle, sphereRadius, centerSpace); // only mass and radius

			// check to see if it's not overlapping some already existing particle
			// or a part outside the walls - should not happen unless the sphereRadius is set too big

			if (HasOverlap(particle))
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

			particle.velocity.X = rndV(rndEngine);
			particle.velocity.Y = rndV(rndEngine);
			particle.velocity.Z = rndV(rndEngine);

			particle.velocity = particle.velocity.Normalize();

			// now the magnitude, can be a constant or distributed as a gaussian, for now just a single value
			particle.velocity *= theApp.GetOptions().initialSpeed;

			particles.emplace_back(particle);
		}
	}

	void Simulation::InitParticle(Particle& particle, double sphereRadius, const Vector3D<double>& centerSpace)
	{
		if (theApp.GetOptions().rightSideInsteadOfSphere)
		{
			if (particle.position.X < sphereRadius)
			{
				particle.mass = theApp.GetOptions().interiorSpheresMass;
				particle.radius = theApp.GetOptions().interiorSpheresRadius;
			}
			else
			{
				particle.mass = theApp.GetOptions().exteriorSpheresMass;
				particle.radius = theApp.GetOptions().exteriorSpheresRadius;
			}
		}
		else
		{
			// check if it's inside of sphere, if yes, make it bigger

			if ((particle.position - centerSpace).Length() < sphereRadius)
			{
				particle.mass = theApp.GetOptions().interiorSpheresMass;
				particle.radius = theApp.GetOptions().interiorSpheresRadius;
			}
			else
			{
				particle.mass = theApp.GetOptions().exteriorSpheresMass;
				particle.radius = theApp.GetOptions().exteriorSpheresRadius;
			}
		}
	}


	bool Simulation::HasOverlap(const Particle& particle) const
	{
		bool overlap = false;
		for (const auto& part : particles)
		{
			if ((part.position - particle.position).Length() <= part.radius + particle.radius ||
				part.position.X > spaceSize - part.radius || part.position.Y > spaceSize - part.radius || part.position.Z > spaceSize - part.radius ||
				part.position.X < part.radius || part.position.Y < part.radius || part.position.Z < part.radius)
			{
				overlap = true;
				break;
			}
		}

		return overlap;
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

				wallEvent.type = Event::EventType::wallCollision;
				wallEvent.particle1 = i;
				wallEvent.eventTime = colTime;

				eventsQueue.emplace_back(wallEvent);
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

					eventsQueue.emplace_back(collisionEvent);
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

			int p1 = nextEvent.particle1;
			int p2 = nextEvent.particle2;
			if (p1 > p2) std::swap(p1, p2);

			for (int i = 0; i < p1; ++i)
			{				
				AddCollision(nextEvent.particle1, i);
				AddCollision(nextEvent.particle2, i);
			}
			for (int i = p1 + 1; i < p2; ++i)
			{
				AddCollision(nextEvent.particle1, i);
				AddCollision(nextEvent.particle2, i);
			}
			for (int i = p2 + 1; i < numParticles; ++i)
			{
				AddCollision(nextEvent.particle1, i);
				AddCollision(nextEvent.particle2, i);
			}
		}
		else
		{
			for (int i = 0; i < nextEvent.particle1; ++i)
				AddCollision(nextEvent.particle1, i);
			for (int i = nextEvent.particle1 + 1; i < numParticles; ++i)
				AddCollision(nextEvent.particle1, i);
		}
	}


}