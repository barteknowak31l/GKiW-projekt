#pragma once
#include <glm/glm.hpp>

class Transform;
class BoxCollider3D
{
public:
	
	glm::vec3 point;
	glm::vec3 size;
	Transform *transform;

	BoxCollider3D() {};
	BoxCollider3D(glm::vec3 p, float s) :point(p), size(s) {};

	bool collision(BoxCollider3D& other)
	{


		if (point.x - size.x/2.0f <= other.point.x + other.size.x/2.0f &&
			point.x + size.x/2.0f >= other.point.x - other.size.x/2.0f &&
			point.y - size.y/2.0f <= other.point.y + other.size.y/2.0f &&
			point.y + size.y/2.0f >= other.point.y - other.size.y/2.0f &&
			point.z - size.z/2.0f <= other.point.z + other.size.z/2.0f &&
			point.z + size.z/2.0f >= other.point.z - other.size.z/2.0f)
		{
			return true;
		}
		return false;
	}

};

