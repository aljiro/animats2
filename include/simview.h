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

class SimView : public View{
private:
	GLFWwindow* window;
	GLuint shaderId;
	GLuint colorId;
	GLuint matrixId;
	glm::mat4 mvp;
	int width, height;

public:
	SimView( Simulation& s );
	int init();
	void setup( Simulation& s );
	void draw(  Simulation& s );
	void drawObject( GeometricObject go[], int colorId);
	void createBuffer( GeometricObject *go );
	GLfloat* collectObjectShape( GeometricObject *go );
	void notify( Simulation& s, std::string message );
	char* checkErrors();
	void transformations( GLuint programId );

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