#ifndef SIMVIEW_H
#define SIMVIEW_H

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"
#include "view.h"
#include "gobject.h"

using namespace std;

namespace morph{ namespace animats{

typedef struct{
	glm::vec3 position;
	float hAngle;
	float vAngle;
	float fov;
	float speed;
	float mouseSpeed;
	float lastTime;
	glm::vec3 direction;
	glm::vec3 up;
} CameraParams; 

class SimView : public View{
private:
	GLFWwindow* window;
	GLuint shaderId;
	// Uniform identifiers
	GLuint colorId;
	GLuint matrixId;
	GLuint viewId;
	GLuint modelId;
	GLuint lightPowerId;
	GLuint lightColorId;
	GLuint lightPositionId;

	glm::mat4 MVP;
	glm::mat4 M;
	glm::mat4 V;
	glm::mat4 P;
	int width, height;
	float lightPower;
	GLfloat *lightColor;
	GLfloat *lightPosition;
	// PArameters for user control
	CameraParams cParams;

public:
	SimView( Simulation& s );
	int init();
	void setup( Simulation& s );
	void draw(  Simulation& s );
	void drawObject( GeometricObject go[], int colorId);
	void createBuffer( GeometricObject *go );
	void collectObjectShape( GeometricObject *go,  GLfloat *shape, GLfloat *normals );
	void notify( Simulation& s, std::string message );
	char* checkErrors();
	void transformations();
	void computeTransformationsFromInput();
	void setViewPort( vec p );

	// Destructor
	~SimView();
};

void GLAPIENTRY MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam );

}}

#endif