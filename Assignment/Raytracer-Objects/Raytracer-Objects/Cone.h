/*----------------------------------------------------------
* COSC 363 Assignment 2 Ray Tracer
*
* The Cone Class.
* This is a subclass of Object, and hence implements the
* methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_Cone
#define H_Cone

#include <glm/glm.hpp>
#include "SceneObject.h"

/**
 * Defines a simple Cone located at 'center' 
 * with the specified radius
 */
class Cone : public SceneObject
{

private:
    glm::vec3 center = glm::vec3(0);
    float radius = 1;
    float height = 1;

public:	
	Cone() {}; //Default constructor creates a unit cylinder
		
        Cone(glm::vec3 c, float r, float h) : center(c), radius(r), height(h) {}
	
	float intersect(glm::vec3 pos, glm::vec3 dir);

	glm::vec3 normal(glm::vec3 p);

};

#endif //!H_Cone