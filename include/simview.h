#ifndef SIMVIEW_H
#define SIMVIEW_H

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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
public:
	SimView( Simulation& s );
	int init();
	void setup( Simulation& s );
	void draw();
	void drawObject( GeometricObject *go );
	void createBuffer( GeometricObject *go );
	GLfloat* collectObjectShape( GeometricObject *go );
	void notify( Simulation& s, std::string message );

	// Destructor
	~SimView();
};

}}

#endif