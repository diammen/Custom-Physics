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

void physObject::wrapScreen()
{
	collider.match(
	[this](Circle s)
	{
		if (pos.y > GetScreenHeight() + s.radius) pos.y = 0.0f - s.radius;
		if (pos.y < 0 - s.radius) pos.y = GetScreenHeight() + s.radius;
		if (pos.x > GetScreenWidth() + s.radius) pos.x = 0.0f - s.radius;
		if (pos.x < 0 - s.radius) pos.x = GetScreenWidth() + s.radius;
	},
	[this](AABB s)
	{
		if (pos.y > GetScreenHeight() + s.halfExtents.y) pos.y = 0.0f - s.halfExtents.y;
		if (pos.y < 0 - s.halfExtents.y) pos.y = GetScreenHeight() + s.halfExtents.y;
		if (pos.x > GetScreenWidth() + s.halfExtents.x) pos.x = 0.0f - s.halfExtents.x;
		if (pos.x < 0 - s.halfExtents.x) pos.x = GetScreenWidth() + s.halfExtents.x;
	});
}
