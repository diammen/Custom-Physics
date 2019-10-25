#pragma once

#include "glm//vec2.hpp"
#include "mapbox/variant.hpp"
using glm::vec2;

struct Circle
{
	float radius;
};

struct AABB
{
	glm::vec2 halfExtents;
};

using Shape = mapbox::util::variant<Circle, AABB>;

bool checkCircleCircle(vec2 posA, Circle circleA, vec2 posB, Circle circleB);
bool checkAABBAABB(vec2 posA, AABB aabbA, vec2 posB, AABB aabbB);
bool checkCircleAABB(vec2 posA, Circle circle, vec2 PosB, AABB aabb);

bool checkCircleX(vec2 posA, Circle lhs, vec2 posB, Shape rhs);
bool checkAABBX(vec2 posA, AABB lhs, vec2 posB, Shape rhs);

void resolvePhysBodies(class physObject& lhs, class physObject& rhs);

void resolveCollision(vec2 posA, vec2 velA, float massA,
					  vec2 posB, vec2 velB, float massB,
					  float elasticity, vec2 normal, vec2* dst);

void drawCircle(vec2 pos, float radius);

void drawRect(vec2 pos, vec2 halfExtents);