#pragma once

#include <vector>
#include <set>

#include "Event.h"
#include "Particle.h"


class Simulation
{
public:
	Simulation();
	~Simulation();

	std::vector<Particle> particles;

	void GenerateRandom(int numParticles = 5000, double sphereRadius = 30);

	void BuildEventQueue();

	unsigned int count() const { return (unsigned int)particles.size(); }

	void Advance();

	inline double NextEventTime() const
	{
		if (!eventsQueue.empty()) return eventsQueue.begin()->eventTime;

		return std::numeric_limits<double>::infinity();
	}

protected:
	std::multiset<Event> eventsQueue;


	static inline void AdjustVelocitiesForCollision(Particle& particle1, Particle& particle2)
	{
		double totalMass = particle1.mass + particle2.mass;
		Vector3D<double> velDif = particle1.velocity - particle2.velocity;
		Vector3D<double> dir = (particle1.position - particle2.position).Normalize();


		Vector3D<double> mult = 2 * (velDif * dir) * dir / totalMass;

		particle1.velocity -= particle2.mass * mult;
		particle2.velocity += particle1.mass * mult;
	}

	static inline void AdjustVelocityForCollision(Particle& particle, Walls wall)
	{
		switch (wall)
		{
		case top:
		case bottom:
			particle.velocity.Z *= -1;
			break;
		case left:
		case right:
			particle.velocity.X *= -1;
			break;
		case front:
		case back:
			particle.velocity.Y *= -1;
			break;
		}
	}
	
	
	inline void AddWallCollisionToQueue(int particle)
	{
		//return;
		
		double colTime = particles[particle].WallCollisionTime();

		if (std::numeric_limits<double>::infinity() != colTime)
		{
			Event wallEvent;
			wallEvent.particle1 = particle;
			wallEvent.type = Event::wallCollision;
			wallEvent.eventTime = colTime;
			eventsQueue.insert(wallEvent);
		}
	}


	inline void AddCollision(int i, int j)
	{		
		//return;  // to test only collisions with the walls

		double colTime = particles[i].CollisionTime(particles[j]);

		if (std::numeric_limits<double>::infinity() != colTime)
		{
			Event collisionEvent;
			collisionEvent.particle1 = i;
			collisionEvent.particle2 = j;
			collisionEvent.eventTime = colTime;

			eventsQueue.insert(collisionEvent);
		}
	}


	inline void AdjustPartner(const Event& colEvent, std::multiset<Event>::iterator it)
	{
		if (it->type == Event::EventType::particleCollision)
		{
			int partner;
			if (it->particle1 == colEvent.particle1 || it->particle1 == colEvent.particle2)
				partner = it->particle2;
			else partner = it->particle1;

			if (partner != colEvent.particle1 && partner != colEvent.particle2 && particles[partner].particleTime < colEvent.eventTime)
			{
				particles[partner].position += particles[partner].velocity * (colEvent.eventTime - particles[partner].particleTime);
				particles[partner].particleTime = colEvent.eventTime;
			}
		}
	}

	inline void EraseParticleEventsFromQueue(const Event& colEvent)
	{
		if (colEvent.type == Event::particleCollision)
		{
			for (auto it = eventsQueue.begin(); it != eventsQueue.end();)
			{
				if (it->particle1 == colEvent.particle1 || (it->type == Event::EventType::particleCollision && it->particle2 == colEvent.particle1) ||
					it->particle1 == colEvent.particle2 || (it->type == Event::EventType::particleCollision && it->particle2 == colEvent.particle2))
				{
					AdjustPartner(colEvent, it);
					it = eventsQueue.erase(it);
				}
				else ++it;
			}
		}
		else
		{
			for (auto it = eventsQueue.begin(); it != eventsQueue.end();)
			{
				if (it->particle1 == colEvent.particle1 || (it->type == Event::EventType::particleCollision && it->particle2 == colEvent.particle1))
				{
					AdjustPartner(colEvent, it);
					it = eventsQueue.erase(it);
				}
				else ++it;
			}
		}
	}


	inline void AdjustForNextEvent(const Event& nextEvent)
	{
		particles[nextEvent.particle1].position += particles[nextEvent.particle1].velocity * (nextEvent.eventTime - particles[nextEvent.particle1].particleTime);

		if (nextEvent.type == Event::EventType::particleCollision)
		{
			particles[nextEvent.particle2].position += particles[nextEvent.particle2].velocity * (nextEvent.eventTime - particles[nextEvent.particle2].particleTime);

			AdjustVelocitiesForCollision(particles[nextEvent.particle1], particles[nextEvent.particle2]);

			particles[nextEvent.particle2].particleTime = nextEvent.eventTime;
		}
		else
		{
			AdjustVelocityForCollision(particles[nextEvent.particle1], particles[nextEvent.particle1].CollisionWall(nextEvent));
		}

		particles[nextEvent.particle1].particleTime = nextEvent.eventTime;
	}
};

