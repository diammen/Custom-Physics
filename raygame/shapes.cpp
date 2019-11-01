#include "shapes.h"
#include "raylib.h"
#include "glm/glm.hpp"
#include "physics.h"
#include <iostream>

bool checkCircleCircle(vec2 posA, Circle circleA, vec2 posB, Circle circleB)
{
	// Returns true if the distance is less than the sum of the circles' radii
	return glm::length(posB - posA) < (circleA.radius + circleB.radius);
}

bool checkAABBAABB(vec2 posA, AABB aabbA, vec2 posB, AABB aabbB)
{
	return posA.x - aabbA.halfExtents.x < posB.x + aabbB.halfExtents.x &&		// L/R
				posA.x + aabbA.halfExtents.x > posB.x - aabbB.halfExtents.x &&	// R/L
				posA.y - aabbA.halfExtents.y < posB.y + aabbB.halfExtents.y &&	// T/B
				posA.y + aabbA.halfExtents.y > posB.y - aabbB.halfExtents.y;	// B/T
}

bool checkCircleAABB(vec2 posA, Circle circle, vec2 posB, AABB aabb)
{
	float distX = posA.x - glm::clamp(posA.x, posB.x - aabb.halfExtents.y, posB.x + aabb.halfExtents.x);
	float distY = posA.y - glm::clamp(posA.y , posB.y - aabb.halfExtents.y, posB.y + aabb.halfExtents.y);
	return (distX * distX + distY * distY) < (circle.radius * circle.radius);
}

bool checkCircleX(vec2 posA, Circle lhs, vec2 posB, Shape rhs)
{
	return rhs.match([posA, lhs, posB](Circle s) { return checkCircleCircle(posA, lhs, posB, s); },
					 [posA, lhs, posB](AABB s) { return checkCircleAABB(posA, lhs, posB, s); });
}

bool checkAABBX(vec2 posA, AABB lhs, vec2 posB, Shape rhs)
{
	return rhs.match([posA, lhs, posB](Circle s) { return checkCircleAABB(posA, s, posB, lhs); },
					 [posA, lhs, posB](AABB s) { return checkAABBAABB(posA, lhs, posB, s); });
}

void resolvePhysBodies(physObject & lhs, physObject & rhs)
{
	vec2 resImpulses[2];

	vec2 normal = { 0,0 };
	float pen = 0.0f;

	normal = lhs.collider.match(
	[lhs, rhs, &pen](Circle c)
	{
		return rhs.collider.match(
		[lhs, rhs, &pen](Circle c)		// Circle-Circle
		{
			float dist = glm::length(lhs.pos - rhs.pos);
			float sum = lhs.collider.get<Circle>().radius + rhs.collider.get<Circle>().radius;

			pen = sum - dist;

			return glm::normalize(lhs.pos - rhs.pos);
		},
		[lhs, rhs, &pen](AABB a)		// Circle-AABB
		{
			// lhs = circle
			// rhs = aabb

			vec2 dir = glm::normalize(rhs.pos - lhs.pos);
			float dist = glm::length(dir) * lhs.collider.get<Circle>().radius;
			vec2 closest = dir * dist;
			vec2 penDir = rhs.pos - closest;
			vec2 rhsCollider = rhs.collider.get<AABB>().halfExtents;

			float distToEdgeX = (rhs.pos.x + rhsCollider.x - closest.x);
			float distToEdgeY = (rhs.pos.y + rhsCollider.y - closest.y);
			penDir.x = glm::clamp(closest.x, -distToEdgeX, distToEdgeX);
			penDir.y = glm::clamp(closest.y, -distToEdgeY, distToEdgeY);

			pen = glm::length(penDir);

			return glm::normalize(penDir);
		}
		);
	},
	[lhs, rhs, &pen](AABB a)
	{
		return rhs.collider.match(
		[lhs, rhs, &pen](AABB a)		// AABB-AABB
		{
			vec2 clampedVec1;
			vec2 clampedVec2;
			vec2 lhsCollider = lhs.collider.get<AABB>().halfExtents;
			vec2 rhsCollider = rhs.collider.get<AABB>().halfExtents;

			clampedVec1 = glm::clamp(lhs.pos, -lhsCollider + lhs.pos, lhsCollider + lhs.pos);
			clampedVec1 = glm::clamp(lhs.pos, -rhsCollider + rhs.pos, rhsCollider + rhs.pos);
			clampedVec2 = glm::clamp(rhs.pos, -lhsCollider + lhs.pos, lhsCollider + lhs.pos);
			clampedVec2 = glm::clamp(rhs.pos, -rhsCollider + rhs.pos, rhsCollider + rhs.pos);

			pen = glm::length(clampedVec1 - clampedVec2) / 2;

			return glm::normalize(clampedVec1 - clampedVec2);
		},
		[lhs, rhs, &pen](Circle c)		// AABB-Circle
		{
			// lhs = aabb
			// rhs = circle

			vec2 dir = glm::normalize(lhs.pos - rhs.pos);
			float dist = glm::length(dir) * rhs.collider.get<Circle>().radius;
			vec2 closest = dir * dist;
			vec2 penDir = lhs.pos - closest;
			vec2 lhsCollider = lhs.collider.get<AABB>().halfExtents;

			float distToEdgeX = (lhs.pos.x + lhsCollider.x - closest.x);
			float distToEdgeY = (lhs.pos.y + lhsCollider.y - closest.y);
			penDir.x = glm::clamp(closest.x, -distToEdgeX, distToEdgeX);
			penDir.y = glm::clamp(closest.y, -distToEdgeY, distToEdgeY);

			pen = glm::length(penDir);

			return glm::normalize(penDir);
		}
		);
	}
	);

	resolveCollision(lhs.pos, lhs.vel, lhs.mass,
					 rhs.pos, rhs.vel, rhs.mass,
					 1.0f, normal, resImpulses);

	lhs.pos += normal * pen;
	rhs.pos -= normal * pen;

	lhs.vel = resImpulses[0];
	rhs.vel = resImpulses[1];
}

void resolveCollision(vec2 posA, vec2 velA, float massA,
					  vec2 posB, vec2 velB, float massB,
					  float elasticity, vec2 normal, vec2 * dst)
{
	vec2 relVel = velA - velB;
	float impulseMag = glm::dot(-(1.0f + elasticity) * relVel, normal) /
					   glm::dot(normal, normal * (1 / massA + 1 / massB));

	dst[0] = velA + (impulseMag / massA) * normal;
	dst[1] = velB - (impulseMag / massB) * normal;
}

void drawCircle(vec2 pos, float radius)
{
	DrawCircleLines(pos.x, pos.y, radius, RED);
}

void drawRect(vec2 pos, vec2 halfExtents)
{
	DrawRectangleLines(pos.x - halfExtents.x, pos.y - halfExtents.y, halfExtents.x * 2, halfExtents.y * 2, BLUE);
}