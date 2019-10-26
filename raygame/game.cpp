#include "game.h"
#include "raylib.h"
#include <iostream>

game::game()
{
	targetFixedStep = 1.0f / 60.0f;
	acculumatedFixedTime = 0.0f;
}

void game::init()
{
	// Initialization
	//--------------------------------------------------------------------------------------
	int screenWidth = 800;
	int screenHeight = 450;

	InitWindow(screenWidth, screenHeight, "Custom Physics Implementation");

	SetTargetFPS(60);
	//--------------------------------------------------------------------------------------
}

void game::tick()
{
	// do update
	acculumatedFixedTime += GetFrameTime();

	if (IsMouseButtonPressed(0) || IsMouseButtonPressed(1))
	{
		physObjects.emplace_back();
		std::cout << "Added a physics object!" << std::endl;

		auto& babyPhys = physObjects[physObjects.size() - 1];
		auto mousePos = GetMousePosition();
		babyPhys.pos = { mousePos.x, mousePos.y };

		if (IsMouseButtonPressed(0))
			babyPhys.collider = Circle { 15.0f };
		else
			babyPhys.collider = AABB { {15,15} };
	}
}

void game::tickPhys()
{
	// do physics
	acculumatedFixedTime -= targetFixedStep;

	for (auto& i : physObjects)
	{
		i.tickPhys(targetFixedStep);
		i.addAccel({ 0, 100 });

		i.wrapScreen();
	}

	for (auto& i : physObjects)
	{
		for (auto& j : physObjects)
		{
			if (&i == &j) continue;

			bool collision = false;

			i.collider.match([i, j, &collision](Circle c) { if (checkCircleX(i.pos, c, j.pos, j.collider)) collision = true; },
							 [i, j, &collision](AABB a)   { if (checkAABBX(i.pos, a, j.pos, j.collider)) collision = true; });

			if (collision) resolvePhysBodies(i, j);
		}
	}
}

void game::draw() const
{
	// Draw
	//----------------------------------------------------------------------------------
	BeginDrawing();

	ClearBackground(RAYWHITE);

	DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

	for (const auto& i : physObjects)
	{
		i.draw();
	}

	EndDrawing();
	//----------------------------------------------------------------------------------
}

void game::exit()
{
	// De-Initialization
	//--------------------------------------------------------------------------------------   
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------
}

bool game::shouldClose() const
{
	return WindowShouldClose();
}

bool game::shouldPhysics() const
{
	return acculumatedFixedTime >= targetFixedStep;
}