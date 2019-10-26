#pragma once

#include "glm/vec2.hpp"
#include "Shapes.h"

class physObject
{
public:
	physObject();
	
	glm::vec2 pos;
	glm::vec2 vel;

	glm::vec2 forces;

	float mass;
	float drag;

	Shape collider;

	void tickPhys(float delta);
	void draw() const;

	void addForce(glm::vec2 force);
	void addImpulse(glm::vec2 impulse);
	
	void addAccel(glm::vec2 accel);
	void addVelocityChange(glm::vec2 delta);

	void wrapScreen();
};