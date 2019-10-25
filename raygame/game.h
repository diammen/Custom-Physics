#pragma once

#include <vector>
#include "physics.h"
using std::vector;

class game
{
	float acculumatedFixedTime;

	vector<physObject> physObjects;

public:
	game();

	void init();
	void tick();
	void tickPhys();
	void draw() const;
	void exit();

	bool shouldClose() const;
	bool shouldPhysics() const;

	float targetFixedStep;
};