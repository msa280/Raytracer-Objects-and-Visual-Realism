/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Cylinders's intersection method.  The input is a ray.
*/
float Cylinder::intersect(glm::vec3 pos, glm::vec3 dir)
{
	glm::vec3 d = pos - center;

	// From Cylinder Intersection Equation, calculate the coefficients of the equation
	float a = (dir.x * dir.x) + (dir.z * dir.z);
	float b = 2 * (dir.x * d.x + dir.z * d.z);
	float c = d.x * d.x + d.z * d.z - (radius * radius);

	// Determine the number and nature of intersection 
	float discr = b * b - 4 * (a * c);

	//  If discriminant is negative or very close to zero, there is no intersection
	if (discr < 0.0 or fabs(discr) < 0.001) {
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

	// Check intersection points
	float y1 = pos.y + dir.y * tSmall;
	float y2 = pos.y + dir.y * tLarge;

	// Intersection with the side of the cylinder at y1
	if (y1 >= center.y && y1 <= center.y + height) {
		return tSmall;
	}
	// Intersection with the side of the cylinder at y2
	else if (y2 >= center.y && y2 <= center.y + height) {
		return tLarge;
	}
	else {
		return -1;
	}
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
	glm::vec3 d = p - center;
	glm::vec3 n = glm::vec3(d.x, 0, d.z);
	n = glm::normalize(n); //normalize
	return n;
}