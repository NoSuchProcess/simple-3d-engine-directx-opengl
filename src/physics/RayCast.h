#ifndef S3DE_GLOBALS_H
#include "../globals.h"
#endif

#ifndef S3DE_RAYCAST_H
#define S3DE_RAYCAST_H

class RayCast
{
public:
	RayCast(int x, int y);
	RayCast();
	~RayCast() {}

private:
	glm::vec3 direction;
	glm::vec3 inverseDirection;
	glm::vec3 origin;

public:
	bool RayIntersectAABB(const   glm::vec3 &boxMin, const glm::vec3 &boxMax);
	bool RayIntersectSphere(const glm::vec3 &boxMin, const glm::vec3 &boxMax);

private:
	glm::vec3 calculateRay(int x, int y);

};

#endif
