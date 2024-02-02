#pragma once

#include <vector>
#include <set>

#include "Event.h"
#include "Particle.h"

#include <random>

namespace MolecularDynamics {


	class Simulation
	{
	public:
		Simulation();

		std::vector<Particle> particles;

		void GenerateRandom(int numParticles = 5000, double sphereRadius = 30);

		void BuildEventQueue();

		unsigned int count() const { return static_cast<unsigned int>(particles.size()); }

		void Advance();

		inline double NextEventTime()
		{
			RemoveNoEventsFromQueueFront();

			if (eventsQueue.empty()) return std::numeric_limits<double>::infinity(); // should not happen, except when there are no particles

			return eventsQueue.front().eventTime;
		}

	private:
		std::vector<Event> eventsQueue;

		inline void RemoveFirstEventFromQueue()
		{
			std::pop_heap(eventsQueue.begin(), eventsQueue.end());
			eventsQueue.pop_back();
		}

		inline Event GetAndRemoveFirstEventFromQueue()
		{
			std::pop_heap(eventsQueue.begin(), eventsQueue.end());
			Event nextEvent = std::move(eventsQueue.back());
			eventsQueue.pop_back();

			return nextEvent;
		}

		inline void AddEventToQueue(Event&& event)
		{
			eventsQueue.emplace_back(std::move(event));
			std::push_heap(eventsQueue.begin(), eventsQueue.end());
		}

		inline void RemoveNoEventsFromQueueFront()
		{
			while (!eventsQueue.empty() && Event::EventType::noEvent == eventsQueue.front().type)
				RemoveFirstEventFromQueue();
		}

		static inline void AdjustVelocitiesForCollision(Particle& particle1, Particle& particle2)
		{
			const double totalMass = particle1.mass + particle2.mass;
			const Vector3D<double> velDif = particle1.velocity - particle2.velocity;
			const Vector3D<double> dir = (particle1.position - particle2.position).Normalize();


			const Vector3D<double> mult = 2 * (velDif * dir) * dir / totalMass;

			particle1.velocity -= particle2.mass * mult;
			particle2.velocity += particle1.mass * mult;
		}

		static inline void AdjustVelocityForCollision(Particle& particle, Walls wall)
		{
			switch (wall)
			{
			case Walls::top:
			case Walls::bottom:
				particle.velocity.Z *= -1;
				break;
			case Walls::left:
			case Walls::right:
				particle.velocity.X *= -1;
				break;
			case Walls::front:
			case Walls::back:
				particle.velocity.Y *= -1;
				break;
			default:
				break;
			}
		}


		inline void AddWallCollisionToQueue(int particle)
		{
			if (const double colTime = particles[particle].WallCollisionTime(); std::numeric_limits<double>::infinity() != colTime)
			{
				Event wallEvent;

				wallEvent.type = Event::EventType::wallCollision;
				wallEvent.particle1 = particle;
				wallEvent.eventTime = colTime;

				AddEventToQueue(std::move(wallEvent));
			}
		}


		inline void AddCollision(int i, int j)
		{
			if (const double colTime = particles[i].CollisionTime(particles[j]); std::numeric_limits<double>::infinity() != colTime)
			{
				Event collisionEvent;

				collisionEvent.particle1 = i;
				collisionEvent.particle2 = j;
				collisionEvent.eventTime = colTime;

				AddEventToQueue(std::move(collisionEvent));
			}
		}


		inline void AdjustPartner(const Event& colEvent, const Event& event)
		{
			if (Event::EventType::particleCollision == event.type)
			{
				int partner = event.particle1;

				if (event.particle1 == colEvent.particle1 || event.particle1 == colEvent.particle2)
					partner = event.particle2;

				if (partner != colEvent.particle1 && partner != colEvent.particle2 && particles[partner].particleTime < colEvent.eventTime)
				{
					particles[partner].position += particles[partner].velocity * (colEvent.eventTime - particles[partner].particleTime);
					particles[partner].particleTime = colEvent.eventTime;
				}
			}
		}

		// this is the place where one must look first for performance enhancements
		inline void EraseParticleEventsFromQueue(const Event& colEvent)
		{
			if (Event::EventType::particleCollision == colEvent.type)
			{
				for (Event& event : eventsQueue)
				{
					if (Event::EventType::noEvent == event.type) continue;

					if (event.particle1 == colEvent.particle1 || event.particle1 == colEvent.particle2 || 
						 (event.type == Event::EventType::particleCollision && (event.particle2 == colEvent.particle1 || event.particle2 == colEvent.particle2)))
					{
						AdjustPartner(colEvent, event);
						event.type = Event::EventType::noEvent;
					}
				}
			}
			else
			{
				for (Event& event : eventsQueue)
				{
					if (Event::EventType::noEvent == event.type) continue;

					if (event.particle1 == colEvent.particle1 || (event.type == Event::EventType::particleCollision && event.particle2 == colEvent.particle1))
					{
						AdjustPartner(colEvent, event);
						event.type = Event::EventType::noEvent;
					}
				}
			}
		}


		inline void AdjustForNextEvent(const Event& nextEvent)
		{
			particles[nextEvent.particle1].position += particles[nextEvent.particle1].velocity * (nextEvent.eventTime - particles[nextEvent.particle1].particleTime);

			if (Event::EventType::particleCollision == nextEvent.type)
			{
				particles[nextEvent.particle2].position += particles[nextEvent.particle2].velocity * (nextEvent.eventTime - particles[nextEvent.particle2].particleTime);

				AdjustVelocitiesForCollision(particles[nextEvent.particle1], particles[nextEvent.particle2]);

				particles[nextEvent.particle2].particleTime = nextEvent.eventTime;
			}
			else
				AdjustVelocityForCollision(particles[nextEvent.particle1], particles[nextEvent.particle1].CollisionWall(nextEvent));

			particles[nextEvent.particle1].particleTime = nextEvent.eventTime;
		}

		bool HasOverlap(const Particle& particle) const;
		void InitParticle(Particle& particle, double sphereRadius, const Vector3D<double>& centerSpace);


		std::mt19937 rndEngine;
		std::random_device rdev;
	};

}
