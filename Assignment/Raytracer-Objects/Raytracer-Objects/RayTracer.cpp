/*===============================================================
* COSC 363 - Assignment 2 - RayTracer Objects & Illumination
* Date: 28/05/2023
* Name: M. Haider Saeed
* ID: 68479094
* Build Command: /Desktop/msa280/Assignment/Raytracer-Objects/Raytracer-Objects$ g++ RayTracer.cpp Ray.cpp SceneObject.cpp Sphere.cpp Plane.cpp Cylinder.cpp Cone.cpp TextureBMP.cpp -lm -lGL -lGLU -lglut
                 /Desktop/msa280/Assignment/Raytracer-Objects/Raytracer-Objects$ ./a.out

*================================================================
*/

#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Cone.h"
#include "TextureBMP.h"
#include "SceneObject.h"
#include "Ray.h"
#include <GL/glut.h>

using namespace std;

float EDIST = 40;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -20.0;
const float XMAX = 20.0;
const float YMIN = -20.0;
const float YMAX = 20.0;
const float ETA = 1.002;
const float PI = 3.14159265359;
float lightIntensity = 0.72;
float eyex = 0.0;
float eyey = 0.0;
bool use_anti_aliasing = true;

TextureBMP jupiterTexture;
TextureBMP moonTexture;
TextureBMP wallTexture;
vector<SceneObject*> sceneObjects;



//-------------------------------- Trace Function---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//   closest point of intersection with objects in the scene.
//   Maps the texture color onto the object and also creates shadows.
//   It also handles the pattern mapping onto objects as well as reflectivity, 
//   transparency and refractivity.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0.529, 0.808, 0.922);   // Sky Blue Background Color						
	glm::vec3 lightPos(-50, 46, -10);				// Light 1 position
	glm::vec3 lightPos2(50, -15, 10);				// Light 2 position
	glm::vec3 color(0);                             // Initialising the color value for tracer
	SceneObject* obj;                               // Creating a scene object

    ray.closestPt(sceneObjects);					// Compare the ray with all objects in the scene
    if(ray.index == -1) return backgroundCol;		// No intersection
	obj = sceneObjects[ray.index];					// Object on which the closest point of intersection is found

	// Texture Map Jupiter on sphere
	if (ray.index == 2)
	{
		glm::vec3 center(24.0, 19.0, -80.0);
		glm::vec3 d = glm::normalize(ray.hit - center);
		float s = (0.5 - atan2(d.z, d.x) + PI) / (2 * PI); // Calculate s texture coordinate
		float t = 0.5 + asin(d.y) / PI;                    // Calculate t texture coordinate
		color = jupiterTexture.getColorAt(s, t);
		obj->setColor(color);
	}

	// Moon Texture on sphere
	if (ray.index == 3)
	{
		glm::vec3 center(13.0, 25.0, -65.0);
		glm::vec3 d = glm::normalize(ray.hit - center);
		float s = (0.5 - atan2(d.z, d.x) + PI) / (2 * PI); // Calculate s texture coordinate
		float t = 0.5 + asin(d.y) / PI; // Calculate t texture coordinate
		color = moonTexture.getColorAt(s, t);
		obj->setColor(color);
	}

	// Cos Wave pattern on Cylinder (cos)
	if (ray.index == 10)
	{
		if ((int(ray.hit.x + cos(ray.hit.y)) % 2 == 0)) {
			color = glm::vec3(1, 1, 0);
		}
		else {
			color = glm::vec3(1.0, 0.08, 0.58);
		}
		obj->setColor(color);
	}

	// Dotted Pattern On Cone (Cos + Sin)
	if (ray.index == 11)
	{
		if ((int(cos(ray.hit.x) + sin(ray.hit.y)) % 2 == 0)) {
			color = glm::vec3(1, 1, 0);
		}
		else {
			color = glm::vec3(0, 0, 0);
		}
		obj->setColor(color);
	}

	// Creating Chequered Pattern on floor
	if (ray.index == 4) 
	{
		int stripeWidth = 3;
		int ix = (ray.hit.x) / stripeWidth + 60;  // +60 Offset to avoid same tile colors
		int iz = (ray.hit.z) / stripeWidth;
		int checkerboard = (ix + iz) % 2; // checkerboard pattern

		if (checkerboard == 0) {
			color = glm::vec3(1, 1, 1); // white
		}
		else {
			color = glm::vec3(0, 0, 0); // black
		}
		obj->setColor(color);
	}


	// Calculating the Lighting of the object
	if (ray.index == 6 or ray.index == 7 or ray.index == 8) {
		color = obj->lighting(lightPos, ray.dir, -ray.hit);    // Walls are not 
		color += obj->lighting(lightPos2, ray.dir, -ray.hit);
	}
	else {
		color = obj->lighting(lightPos, -ray.dir, ray.hit);
		color += obj->lighting(lightPos2, -ray.dir, ray.hit);
	}
	
	// Calculating the Shadow lighting for Light 1
	glm::vec3 lightVec = lightPos - ray.hit;        // Vector from the point of intersection to the light source
	Ray shadowRay(ray.hit, lightVec);               // Create a shadow ray at the point of intersection
	shadowRay.closestPt(sceneObjects);              // Find the closest point of intersection on the shadow ray
	float lightDist = glm::length(lightVec);        // The distance to the light source

	// Calculating the Shadow lighting for Light 2
	glm::vec3 lightVec2 = lightPos2 - ray.hit;      
	Ray shadowRay2(ray.hit, lightVec2);             
	shadowRay2.closestPt(sceneObjects);             
	float lightDist2 = glm::length(lightVec2);      

	bool isShadow1 = (shadowRay.index > -1) && (shadowRay.index != 1) && (shadowRay.index != 12) && (shadowRay.dist < lightDist);
	bool isShadow2 = (shadowRay2.index > -1) && (shadowRay2.index != 1) && (shadowRay2.index != 12) && (shadowRay2.dist < lightDist2);
	bool isLigtherShadow1 = ((shadowRay.index == 1) || (shadowRay.index == 12)) && (shadowRay.dist < lightDist);
	bool isLigtherShadow2 = ((shadowRay2.index == 1) || (shadowRay2.index == 12)) && (shadowRay2.dist < lightDist2);

	if (isShadow1) { //If the shadow ray hits an object (shadowRay.index > -1) and the distance to the point of intersection on this  	                                                         // object is smaller than the distance to the light source (shadowRay.dist < lightDist),
		color = 0.2f * obj->getColor(); 
	}
	else if (isLigtherShadow1) {
		color = 0.7f * obj->getColor();
	}
	
	if (isShadow2) { //If the shadow ray hits an object (shadowRay.index > -1) and the distance to the point of intersection on this  	                                                         // object is smaller than the distance to the light source (shadowRay.dist < lightDist),
		color = 0.2f * obj->getColor(); 
	}
	else if (isLigtherShadow2) {
		color = 0.7f * obj->getColor();
	}


	// Ray calculations if object is transparent
	if (obj->isTransparent() && step < MAX_STEPS)
	{
		float tco = obj->getTransparencyCoeff();
		glm::vec3 normalVec = obj->normal(ray.hit);
		Ray transparentRay(ray.hit, ray.dir);
		transparentRay.closestPt(sceneObjects);
		glm::vec3 transparentColor = trace(transparentRay, step + 1);
		color = transparentColor * tco;
	}

	// Ray calculations if object is reflective
	if (obj->isReflective() && step < MAX_STEPS)
	{
		float rho = obj->getReflectionCoeff();
		glm::vec3 normalVec = obj->normal(ray.hit);
		glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
		Ray reflectedRay(ray.hit, reflectedDir);
		glm::vec3 reflectedColor = trace(reflectedRay, step + 1);
		color += (rho * reflectedColor);
	}

	// Ray calculations if object is refractive
	if (obj->isRefractive() && step < MAX_STEPS) {
		glm::vec3 normalVec = obj->normal(ray.hit);
		glm::vec3 refractedDir = glm::refract(ray.dir, normalVec, obj->getRefractiveIndex());
		Ray refractedRay(ray.hit, refractedDir);
		refractedRay.closestPt(sceneObjects);
		glm::vec3 refractedNormalVec = obj->normal(refractedRay.hit);
		glm::vec3 newRefractedDir = glm::refract(refractedDir, -refractedNormalVec, 1.0f / obj->getRefractiveIndex());
		Ray newRefractedRay(refractedRay.hit, newRefractedDir);
		glm::vec3 refractedColor = trace(newRefractedRay, step + 1);
		color = refractedColor;
	}

	return color * lightIntensity;
}




// --------------------- Anti-Aliasing Function -------------------------------
// Performs super - sampling anti - aliasing on a pixel using a 2x2 grid of 
// sub - pixel samples.
//-----------------------------------------------------------------------------
glm::vec3 anti_aliasing(glm::vec3 eye, float pixel_size, float xp, float yp) {

	// Offsets defining the sub-pixel positions to sample from each pixel.
	float offsetA = pixel_size * 0.25f;
	float offsetB = pixel_size * 0.75f;

	// To accumulate the color values from the samples.
	glm::vec3 color(0);

	// Subpixel positions to sample
	glm::vec2 sampleOffsets[] = {
		glm::vec2(offsetA, offsetA),
		glm::vec2(offsetA, offsetB),
		glm::vec2(offsetB, offsetA),
		glm::vec2(offsetB, offsetB)
	};

	// Iterate over the sample positions 
	for (int i = 0; i < 4; i++) 
	{
		glm::vec3 samplePos = glm::vec3(xp + sampleOffsets[i].x, yp + sampleOffsets[i].y, -EDIST); // Position to sample created
		Ray ray = Ray(eye, samplePos);
		ray.normalize();
		glm::vec3 sampleColor = trace(ray, 1); // Trace the primary ray and get the color value
		color += sampleColor;
	}

	color *= 0.25f; // Average the color value to implement pixel color change

	return color;
}



//------------------- The Main Display Module -------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX - XMIN) / NUMDIV;  //cell width
	float cellY = (YMAX - YMIN) / NUMDIV;  //cell height
	glm::vec3 eye(eyex, eyey, 0);
	glm::vec3 color(0);
	
	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a tiny quad.

	for (int i = 0; i < NUMDIV; i++)	// Scan every cell of the image plane
	{
		xp = XMIN + i * cellX;
		for (int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j * cellY;

			glm::vec3 dir(xp + 0.5 * cellX, yp + 0.5 * cellY, -EDIST);	// Direction of the primary ray

			Ray ray = Ray(eye, dir);

			if (use_anti_aliasing == false) {
				color = trace(ray, 1); // Trace the primary ray and get the colour value
			}
			else {
				ray.normalize();
				color = anti_aliasing(eye, cellX, xp, yp); // Anti-aliasing active
			}

			glColor3f(color.r, color.g, color.b);
			glVertex2f(xp, yp);				// Draw each cell with its color value
			glVertex2f(xp + cellX, yp);
			glVertex2f(xp + cellX, yp + cellY);
			glVertex2f(xp, yp + cellY);
		}
	}
    glEnd();
    glFlush();
}



//------------------------ This Initialize Function -------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//   and add them to the list of scene objects.
//   It also initializes the OpenGL 2D orthographc projection matrix for drawing the
//   the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

    glClearColor(0, 0, 0, 1);

	// 0.) Giant Reflective Sphere
	Sphere *sphere1 = new Sphere(glm::vec3(15.0, 10.0, -120.0), 29.0);  // Creating Sphere Object
	sphere1->setColor(glm::vec3(0, 0, 0.3));  // Set colour to light blue
	sphere1->setReflectivity(true, 0.99);     // Set reflectivity of the sphere
	sceneObjects.push_back(sphere1);		  // Add sphere object to scene objects

	// 1.) Hollow Transparent Sphere 
	Sphere* sphere2 = new Sphere(glm::vec3(0, -10.0, -50.0), 5.0);
	sphere2->setColor(glm::vec3(0, 0, 0.2));
	sphere2->setTransparency(true, 1);    // Set transparency - 0 means none and 1 means full transparency
	sphere2->setSpecularity(true);
	sphere2->setReflectivity(true, 0.05);
	sceneObjects.push_back(sphere2);		 

	// 2.) Jupiter Sphere 
	Sphere* sphere3 = new Sphere(glm::vec3(24.0, 19.0, -80.0), 6.0);
	sphere3->setSpecularity(true);   // Set specularity
	sceneObjects.push_back(sphere3);		 

	// 3.) Moon Sphere
	Sphere* sphere4 = new Sphere(glm::vec3(13.0, 25.0, -65.0), 2.0);  
	sphere3->setSpecularity(true);
	sceneObjects.push_back(sphere4);		 

	// 4.) Floor
	Plane* floor = new Plane(glm::vec3(-70, -20, -20),   // Creating a plane object
		glm::vec3(70., -20, -20),
		glm::vec3(70., -20, -200),
		glm::vec3(-70., -20, -200));
	sceneObjects.push_back(floor);

	// 5.) Front Wall
	Plane* front_wall = new Plane(glm::vec3(-70, -20, -200),
		glm::vec3(70., -20, -200),
		glm::vec3(70., 50, -200),
		glm::vec3(-70., 50, -200));
	front_wall->setColor(glm::vec3(0, 1, 1));   
	sceneObjects.push_back(front_wall);

	// 6.) Left Wall
	Plane* side_wall1 = new Plane(glm::vec3(-70., -21., -200.), 
		glm::vec3(-70., -21., -20.),  
		glm::vec3(-70., 51., -20.),   
		glm::vec3(-70., 51., -200.)); 
	side_wall1->setColor(glm::vec3(1, 0, 0));   
	sceneObjects.push_back(side_wall1);

	// 7.) Right Wall
	Plane* side_wall2 = new Plane(glm::vec3(70., -21., -20.),   
		glm::vec3(70., -21., -200.),  
		glm::vec3(70., 51., -200.),   
		glm::vec3(70., 51., -20.));   
	side_wall2->setColor(glm::vec3(0, 1, 0));   
	sceneObjects.push_back(side_wall2);

	// 8.) Ceiling
	Plane* ceiling = new Plane(glm::vec3(-70., 50., -20.),   
		glm::vec3(70., 50., -20.),     
		glm::vec3(70., 50., -200.),   
		glm::vec3(-70., 50., -200.)); 
	ceiling->setColor(glm::vec3(1, 1, 0));  
	sceneObjects.push_back(ceiling);

	// 9.) Mirror
	Plane* mirror = new Plane(glm::vec3(-70, 44, -170),
		glm::vec3(-70., -15, -170),
		glm::vec3(-10., -15, -200),
		glm::vec3(-10., 44, -200));
	mirror->setColor(glm::vec3(0, 0, 0.05));  
	mirror->setReflectivity(true, 1.0);   
	sceneObjects.push_back(mirror);

	// 10.) Cylinder
	Cylinder* cylinder = new Cylinder(glm::vec3(25, -20, -80), 5.0, 12.0);
	cylinder->setColor(glm::vec3(1, 1, 0)); 
	cylinder->setSpecularity(true);
	sceneObjects.push_back(cylinder);		

	// 11.) Cone
	Cone* cone= new Cone(glm::vec3(-20, -18.0, -70.0), 5.0, 11.0);
	cone->setColor(glm::vec3(1, 1, 0));   
	cone->setSpecularity(true);
	sceneObjects.push_back(cone);		 

	// 12.) Sphere5
	Sphere* sphere5 = new Sphere(glm::vec3(-34, -3.0, -90.0), 7.0);
	sphere5->setColor(glm::vec3(1, 1, 1));   
	sphere5->setRefractivity(true, 1, 1.016);  // Set refration to 1.02
	sceneObjects.push_back(sphere5);		

	// Loading Textures
	jupiterTexture = TextureBMP("Jupiter.bmp");
	moonTexture = TextureBMP("Moon.bmp");
}


/**
* Allows user to control the camera
*/
void controls(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) {
		EDIST += 7;
	}
	else if (key == GLUT_KEY_DOWN) {
		EDIST -= 7;
	}
	else if (key == GLUT_KEY_LEFT) {
		eyex -= 7;
	}
	else if (key == GLUT_KEY_RIGHT) {
		eyex += 7;
	}
	else if (key == GLUT_KEY_F1) {
		eyey -= 7;
	}
	else if (key == GLUT_KEY_F2) {
		eyey += 7;
	}
	glutPostRedisplay();
}


//--- This function initializes the window  ----------------------------------------
//   It creates a display module and a window. It also gives a call to the initialize
//   function to start the drawing of ray traced images.
//----------------------------------------------------------------------------------
int main(int argc, char *argv[]) 
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(670, 200);
    glutCreateWindow("RayTracer Objects & Illumination");

    glutDisplayFunc(display);
    initialize();
	glutSpecialFunc(controls);

    glutMainLoop();
    return 0;
}
