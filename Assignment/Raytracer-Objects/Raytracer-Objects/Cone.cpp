/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Cone Class.
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

/**
* Cone's intersection method.  The input is a ray.
*/
float Cone::intersect(glm::vec3 pos, glm::vec3 dir)
{
	// Calculate vector from center of cone to given position pos
	glm::vec3 d = pos - center;

	// Compute distance between height of cone and y - coordinate of pos.
	float dist = height - pos.y + center.y;

	// Compute squared ratio of radius to the height of the cone.
	float ratio = (radius / height) * (radius / height);

	// Computes coefficient a for the discriminant 
	float a = (dir.x * dir.x) + (dir.z * dir.z) - (ratio * (dir.y * dir.y));
	float b = 2 * (d.x * dir.x + d.z * dir.z + ratio * dist * dir.y);
	float c = (d.x * d.x) + (d.z * d.z) - (ratio * (dist * dist));
	float discr = (b * b) - 4 * (a * c);

	//  If discriminant is negative or very close to zero, there is no intersection
	if (discr < 0 or fabs(discr) < 0.001) {
		return -1.0;
	}
	
	float tSmall;
	float tLarge;
	float t1 = (-b - sqrt(discr)) / (2 * a); // Intersection Distance 1
	float t2 = (-b + sqrt(discr)) / (2 * a); // Intersection Distance 2

	// Determine which t is larger and which is smaller
	if (t1 > t2) {
		tSmall = t2;
		tLarge = t1;
	}
	else {
		tSmall = t1;
		tLarge = t2;
	}

	// Checks if the computed intersection point lies within the height range of the cone.

	// Get the y coordinate of the first intersection point.
	float y1 = pos.y + dir.y * tSmall;
	float y2 = pos.y + dir.y * tLarge;

	// Check if the computed y coordinate of the intersection point is within the range of 
	// the cone's height. Indicating an intersection with the side surface of the cone
	if ((y1 >= center.y) && (y1 <= center.y + height)) {
		return tSmall;
	}
	else if ((y1 >= center.y) && (y1 <= center.y + height)) {
		return tLarge;
	}
	else {
		return -1;
	}

}
/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cone.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
	glm::vec3 d = p - center;
	float r = sqrt(d.x * d.x + d.z * d.z);
	glm::vec3 n = glm::vec3(d.x, r * (radius / height), d.z);
	n = glm::normalize(n);
	return n;
}