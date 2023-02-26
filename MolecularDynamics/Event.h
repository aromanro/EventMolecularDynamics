#pragma once

namespace MolecularDynamics {

	class Event
	{
	public:
		Event();
		~Event();

		double eventTime;

		enum class EventType : unsigned char {
			particleCollision,
			wallCollision,
			// here other kind of events could be added - of course they must be handled appropriately
			// for example crossings into other sectors

			noEvent
		};

		EventType type;

		int particle1;
		int particle2;

		inline bool InvolvesParticle(int particle) const { return particle1 == particle || (type == EventType::particleCollision && particle2 == particle); }

		bool operator<(const Event& other) const
		{
			return eventTime > other.eventTime;
		}
	};

}