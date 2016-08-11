#pragma once

class Event
{
public:
	Event();
	~Event();

	double eventTime;

	typedef enum EventType {
		particleCollision,
		wallCollision,
		// here other kind of events could be added - of course they must be handled appropriately
		// for example crossings into other sectors

		noEvent
	} EventType;

	EventType type;

	int particle1;
	int particle2;

	bool InvolvesParticle(int particle) { return particle1 == particle || (type == particleCollision && particle2 == particle); }

	bool operator<(const Event& other) const 
	{ 
		return eventTime > other.eventTime; 
	}
};

