#ifndef SIMVIEW_H
#define SIMVIEW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "shader.hpp"
#include "view.h"
#include "gobject.h"

class SimView : public View{
private:
	GLFWwindow* window;
public:
	int init();
	void setup( Simulation &s );
	void draw();
	void createBuffer( GeometricObject *go );

	void notify( Simulation& s, std::string message );
};

#endif