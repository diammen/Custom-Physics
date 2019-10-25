#include "physics.h"
#include "raylib.h"

physObject::physObject()
{
	pos = { 0,0 };
	vel = { 0,0 };
	forces = { 0,0 };

	mass = 1.0f;
	drag = 0.5f;
}

void physObject::tickPhys(float delta)
{
	// integrate forces into velocity
	vel += forces * delta;
	forces = { 0,0 };

	// integrate linear drag
	vel *= 1.0f - delta * drag;

	// integrate velocity into position
	pos += vel * delta;
}

void physObject::draw() const
{
	collider.match([this](Circle s) { drawCircle(pos, s.radius); },
				   [this](AABB s)   { drawRect(pos, s.halfExtents); });
}

void physObject::addForce(glm::vec2 force)
{
	forces += force * 1.0f / mass;
}

void physObject::addImpulse(glm::vec2 impulse)
{
	vel += impulse * (1.0f / mass);
}

void physObject::addAccel(glm::vec2 accel)
{
	forces += accel;
}

void physObject::addVelocityChange(glm::vec2 delta)
{
	vel += delta;
}
