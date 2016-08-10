#include "stdafx.h"
#include "Event.h"


Event::Event()
	: eventTime(0), type(particleCollision),
	particle1(-1), particle2(-1)
{
}


Event::~Event()
{
}
