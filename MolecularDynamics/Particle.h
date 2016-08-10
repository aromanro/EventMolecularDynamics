#pragma once

#include <math.h>
#include <algorithm>
#include <limits>

#include "Vector3D.h"
#include "Constants.h"
#include "Wall.h"
#include "Event.h"

class Particle
{
public:
	Particle();
	~Particle();


	Vector3D<double> position;
	Vector3D<double> velocity;

	double mass;
	double radius;

	double particleTime;

	inline Vector3D<double> GetPosition(double simTime) const
	{
		return position + (simTime - particleTime) * velocity;
	}

	inline Vector3D<double> GetPosition(double simTime, double period) const
	{
		Vector3D<double> pos = GetPosition(simTime);

		return Vector3D<double>(AdjustValue(pos.X, period), AdjustValue(pos.Y, period), AdjustValue(pos.Z, period));
	}

	inline double CollisionTime(Particle& other) const
	{
		// start from whatever time is bigger, this is the reference time
		double refTime = std::max<double>(particleTime, other.particleTime);

		double difTime = other.particleTime - particleTime;
		
		Vector3D<double> difPos = position - other.position;

		// adjust because the particles might not have the same time
		// there are two cases here:

		// 1. difTime > 0 means that the 'other' particle time is bigger 
		// refTime is 'other' particle time
		// in such case 'position' (the position of 'this' particle) must be advanced in time
		// like this position = position + velocity * difTime

		// 2. difTime < 0 means that 'this' has the bigger reference time
		// the position of the 'other' particle must be advanced in time
		// like this: other.position = other.position + other.velocity * abs(difTime)
		// in difPos other.position is with '-', difTime is negative so the 'abs' is dropped
		
		difPos += (difTime > 0 ? velocity : other.velocity) * difTime;

		Vector3D<double> difVel = velocity - other.velocity;
		double b = difPos * difVel;
		double minDist = radius + other.radius; // collision distance
		double C = difPos * difPos - minDist * minDist;
		double difVel2 = difVel * difVel;

		double delta = b * b - difVel2 * C;

		// a delta < 0 means to complex solutions, that is, no real solution = the spheres do not collide
		// delta = 0 is the degenerate case, the spheres meet in the trajectory and depart at the same time = tangential touch, no need to handle it, they are smooth spheres
		// delta > 0 is the only interesting case

		// the two different solutions means that there are two times when the spheres are at the radius1 + radius2 distance on their trajectory, which is what we want to consider
		// the first time is the time they 'meet' and that's the needed time, the time of the collision event after which new velocities must be calculated

		// the b < 0 condition is the condition that the centers approach, b > 0 means that they are departing
		if (delta > 0 && b < 0)
		{
			double sdelta = sqrt(delta);

			double t1 = (-b - sdelta) / difVel2;
			// t2 is not needed, it's the bigger time when they would be again at the radius1+radius2 distance if they would pass through each other
			//double t2 = (-b + sdelta) / difVel2;

			// if this time would be negative, that is, it would be as if it took place in the past
			// perhaps there was a real collision after it that changed the trajectory?
			if (t1 >= 0) return refTime + t1;			
		}

		return std::numeric_limits<double>::infinity();
	}
	
	inline double WallCollisionTime() const
	{
		return particleTime + std::min<double>(WallCollisionTime(position.X, velocity.X), std::min<double>(WallCollisionTime(position.Y, velocity.Y), WallCollisionTime(position.Z, velocity.Z)));
	}

	inline Walls CollisionWall(const Event& collisionEvent) const
	{
		if (particleTime + WallCollisionTime(position.X, velocity.X) <= collisionEvent.eventTime)
			return velocity.X > 0 ? Walls::right : Walls::left;
		else if (particleTime + WallCollisionTime(position.Y, velocity.Y) <= collisionEvent.eventTime)
			return velocity.Y > 0 ? Walls::front : Walls::back;

		return velocity.Z > 0 ? Walls::top : Walls::bottom;
	}

private:
	inline double AdjustValue(double val, double period) const
	{
		if (val >= period) val = fmod(val, period);
		else if (val < 0) val = fmod(val, period) + period;

		return val;
	}

	inline double WallCollisionTime(double posComponent, double velComponent) const
	{
		if (velComponent > 0) return (spaceSize - posComponent - radius) / velComponent;
		else if (velComponent < 0) return -(posComponent - radius) / velComponent;

		return std::numeric_limits<double>::infinity();
	}
};

